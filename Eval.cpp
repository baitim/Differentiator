#include <assert.h>
#include <math.h>

#include "ANSI_colors.h"
#include "Eval.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

const double EPSILON = 1e-9;

static ErrorCode tree_eval_ (Node* node, Variables* vars, double* ans_eval);
static ErrorCode node_eval_ (Node* node, Variables* vars, double* ans_eval,
                             double left_eval, double right_eval);
static ErrorCode var_eval   (Node* node, Variables* vars, double* ans_eval);
static ErrorCode op_eval    (Node* node, double* ans_eval, double left_eval, double right_eval);
static ErrorCode clean_stdin();
static double norm_double   (double x, double min, double max);
static int is_double_equal  (double x, double y);

ErrorCode tree_eval(Tree* tree, double* ans_eval)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_eval_(tree->root, tree->variables, ans_eval);
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
        var_eval(node, vars, ans_eval);
    } else if (node->type_value == TYPE_OP) {
        op_eval(node, ans_eval, left_eval, right_eval);
    }

    return ERROR_NO;
}

static ErrorCode var_eval(Node* node, Variables* vars, double* ans_eval)
{
    if (!vars->var[(int)node->value].valid) {
        printf(print_lcyan("Input value of %s: "), vars->var[(int)node->value].name);
        int count_read = scanf("%lf", &vars->var[(int)node->value].value);
        while (count_read != 1) {
            clean_stdin();
            printf(print_lred("Wrong argument, you should input double, try again: "));
            count_read = scanf("%lf", &vars->var[(int)node->value].value);
        }
        vars->var[(int)node->value].valid = 1;
    }
    *ans_eval = vars->var[(int)node->value].value;

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

    int max_ind = (int)((graph->right_board - graph->left_board) / graph->step_values) + 1;

    for (int i = 0; i < max_ind; i++) {
        double x_value = i * graph->step_values + graph->left_board;
        x[i] = x_value;
        y[i] = norm_double(30 * cos(double(x_value / 3)), graph->min_value, graph->max_value);
    }

    return tree_verify(tree->root);
}

static ErrorCode clean_stdin()
{
    while (getchar() != '\n')
        ;
    return ERROR_NO;
}

static double norm_double(double x, double min, double max)
{
    return MIN(MAX(x, min), max);
}

static int is_double_equal(double x, double y)
{
    return (fabs(x - y) <= EPSILON);
}