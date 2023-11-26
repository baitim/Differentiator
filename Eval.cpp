#include <assert.h>
#include <math.h>
#include <string.h>

#include "ANSI_colors.h"
#include "Eval.h"
#include "Input.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

const double EPSILON = 1e-9;

static ErrorCode get_num_eval_var (Tree* tree, int* num_var);
static ErrorCode tree_eval_ (Node* node, Variables* vars, double* ans_eval);
static ErrorCode node_eval_ (Node* node, Variables* vars, double* ans_eval,
                             double left_eval, double right_eval);
static ErrorCode op_eval    (Node* node, double* ans_eval, double left_eval, double right_eval);
static double norm_double   (double x, double min, double max);
static double powf          (double x, int st);
static int is_double_equal  (double x, double y);

ErrorCode tree_eval(Tree* tree, double* ans_eval)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = ERROR_NO;

    Tree* new_tree = nullptr;
    err = tree_copy(tree, "TreeEval", &new_tree);
    if (err) return err;

    err = tree_eval_(new_tree->root, new_tree->variables, ans_eval);
    if (err) return err;

    err = tree_delete(new_tree);
    if (err) return err;

    return ERROR_NO;
}

static ErrorCode tree_eval_(Node* node, Variables* vars, double* ans_eval)
{
    assert(vars);
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(node);
    if (err) return err;

    double left_eval =  -1;
    double right_eval = -1;
    if (node->left)  err = tree_eval_(node->left,  vars, &left_eval );
    if (err) return err;
    if (node->right) err = tree_eval_(node->right, vars, &right_eval);
    if (err) return err;

    err = node_eval_(node, vars, ans_eval, left_eval, right_eval);
    if (err) return err;

    return tree_verify(node);
}

static ErrorCode node_eval_(Node* node, Variables* vars, double* ans_eval,
                            double left_eval, double right_eval)
{
    assert(node);
    assert(vars);

    if (node->type_value == TYPE_NUM) {
        *ans_eval = node->value;
    } else if (node->type_value == TYPE_VAR) {
        get_var(vars, (int)node->value);
        *ans_eval = vars->var[(int)node->value].value;
    } else if (node->type_value == TYPE_OP) {
        op_eval(node, ans_eval, left_eval, right_eval);
    }

    return ERROR_NO;
}

static ErrorCode op_eval(Node* node, double* ans_eval, double left_eval, double right_eval)
{
    assert(node);

    switch ((int)node->value) {
        case (OP_ADD) :
            *ans_eval = left_eval + right_eval;
            break;
        case (OP_SUB) :
            *ans_eval = left_eval - right_eval;
            break;
        case (OP_MUL) :
            *ans_eval = left_eval * right_eval;
            break;
        case (OP_DIV) :
            if (is_double_equal(right_eval, 0.f))
                return ERROR_DIVIDED_NULL;
            *ans_eval = left_eval / right_eval;
            break;
        case (OP_SQRT) :
            *ans_eval = sqrt(left_eval);
            break;
        case (OP_POW) :
            if (is_double_equal(right_eval, int(right_eval)))
                *ans_eval = powf(left_eval, (int)right_eval);
            else
                *ans_eval = pow(left_eval, right_eval);
            break;
        case (OP_SIN) :
            *ans_eval = sin(left_eval);
            break;
        case (OP_COS) :
            *ans_eval = cos(left_eval);
            break;
        default :
            assert(0);
    }

    return ERROR_NO;
}

ErrorCode tree_get_points(Tree* tree, EvalPoints* graph, double* x, double* y)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(tree->root);
    if (err) return err;

    Tree* new_tree = nullptr;
    err = tree_copy(tree, "TreeGetPoints", &new_tree);
    if (err) return err;

    int max_ind = (int)((graph->right_board - graph->left_board) / graph->step_values) + 1;

    int num_var = 0;
    err = get_num_eval_var(new_tree, &num_var);
    if (err) return err;

    for (int i = 0; i < max_ind; i++) {
        double x_value = i * graph->step_values + graph->left_board;
        new_tree->variables->var[num_var].value = x_value;
        x[i] = x_value;
        double y_value = 0;
        err = tree_eval(new_tree, &y_value);
        if (err) return err;
        double new_y = norm_double(y_value, graph->min_value, graph->max_value);
        if (!is_double_equal(y_value, new_y)) {
            y[i] = graph->max_value * 2; printf("%lf\n", x_value);
        } else 
            y[i] = new_y;
    }

    err = tree_delete(new_tree);
    if (err) return err;

    return tree_verify(tree->root);
}

static ErrorCode get_num_eval_var(Tree* tree, int* num_var)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(tree->root);
    if (err) return err;

    printf(print_lcyan("You can choose this variables:\n"));
    for (int i = 0; i < tree->variables->count; i++) {
        if (!tree->variables->var[i].valid) {
            printf(print_lcyan("\t· %s\n"), tree->variables->var[i].name);
            tree->variables->var[i].value = 0;
        }
    }

    char name_var[MAX_SIZE_VAR] = "";
    while (true) {
        printf(print_lcyan("Input name of variable: "));
        int count_read = scanf("%s", name_var);

        int exist = 0;
        for (int i = 0; i < tree->variables->count; i++) {
            if (strcmp(tree->variables->var[i].name, name_var) == 0) {
                *num_var = i;
                tree->variables->var[i].valid = 1;
                exist = 1;
            }
        }

        if (exist == 1 && count_read == 1) {
            for (int i = 0; i < tree->variables->count; i++) {
                if (!tree->variables->var[i].valid) {
                    printf(print_lcyan("Also you should input variables:\n"));
                    break;
                }
            }
                
            for (int i = 0; i < tree->variables->count; i++)
                get_var(tree->variables, i);

            printf(print_lcyan("All variables have value\n"));
            break;
        } else {
            clean_stdin();
            printf(print_lred("Wrong argument, you should input name of existing variable\n"));
        }
    }

    return tree_verify(tree->root);
}

static double norm_double(double x, double min, double max)
{
    return MIN(MAX(x, min), max);
}

static double powf(double x, int st)
{
    if (st == 0) return 1;
    if (st % 2 == 1) return x * powf(x, st - 1);
    double z = powf(x, st / 2);
    return z * z;
}

static int is_double_equal(double x, double y)
{
    return (fabs(x - y) <= EPSILON);
}