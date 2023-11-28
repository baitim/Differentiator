#include <assert.h>
#include <string.h>

#include "ANSI_colors.h"
#include "Eval.h"
#include "Math.h"
#include "Input.h"

static ErrorCode get_num_eval_var (Tree* tree, int* num_var);
static ErrorCode tree_eval_ (TreeNode* node, Variables* vars, double* eval_equation);
static ErrorCode node_eval_ (TreeNode* node, Variables* vars, double* eval_equation,
                             double left_eval, double right_eval);
static ErrorCode op_eval    (TreeNode* node, double* eval_equation, double left_eval, double right_eval);

ErrorCode tree_eval(Tree* tree, double* eval_equation)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = ERROR_NO;

    Tree* new_tree = nullptr;
    err = tree_copy(tree, "TreeEval", &new_tree);
    if (err) return err;

    err = tree_eval_(new_tree->root, new_tree->variables, eval_equation);
    if (err) return err;

    err = tree_delete(new_tree);
    if (err) return err;

    return ERROR_NO;
}

static ErrorCode tree_eval_(TreeNode* node, Variables* vars, double* eval_equation)
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

    err = node_eval_(node, vars, eval_equation, left_eval, right_eval);
    if (err) return err;

    return tree_verify(node);
}

static ErrorCode node_eval_(TreeNode* node, Variables* vars, double* eval_equation,
                            double left_eval, double right_eval)
{
    assert(node);
    assert(vars);

    if (node->type_value == TYPE_NUM) {
        *eval_equation = node->value;
    } else if (node->type_value == TYPE_VAR) {
        get_var(vars, (int)node->value);
        *eval_equation = vars->var[(int)node->value].value;
    } else if (node->type_value == TYPE_OP) {
        op_eval(node, eval_equation, left_eval, right_eval);
    }

    return ERROR_NO;
}

static ErrorCode op_eval(TreeNode* node, double* eval_equation, double left_eval, double right_eval)
{
    assert(node);

    switch ((int)node->value) {
        case (OP_ADD)  : *eval_equation = left_eval + right_eval;           break;
        case (OP_SUB)  : *eval_equation = left_eval - right_eval;           break;
        case (OP_MUL)  : *eval_equation = left_eval * right_eval;           break;
        case (OP_DIV)  :
            if (is_double_equal(right_eval, 0.f))
                return ERROR_DIVISION_ZERO;
            *eval_equation = left_eval / right_eval;
            break;
        case (OP_POW)  : *eval_equation = my_pow(left_eval, right_eval);    break;
        case (OP_LOG)  : *eval_equation = log(left_eval) / log(right_eval); break;
        case (OP_LN)   : *eval_equation = log(left_eval);                   break;
        case (OP_SQRT) : *eval_equation = sqrt(left_eval);                  break;
        case (OP_SIN)  : *eval_equation = sin(left_eval);                   break;
        case (OP_COS)  : *eval_equation = cos(left_eval);                   break;
        case (OP_TG)   : *eval_equation = tan(left_eval);                   break;
        case (OP_CTG)  : *eval_equation = 1 / tan(left_eval);               break;
        case (OP_ASIN) : *eval_equation = asin(left_eval);                  break;
        case (OP_ACOS) : *eval_equation = acos(left_eval);                  break;
        case (OP_ATG)  : *eval_equation = atan(left_eval);                  break;
        case (OP_ACTG) : *eval_equation = atan(1 / left_eval);              break;
        case (OP_SH)   : *eval_equation = sinh(left_eval);                  break;
        case (OP_CH)   : *eval_equation = cosh(left_eval);                  break;
        case (OP_TH)   : *eval_equation = tanh(left_eval);                  break;
        case (OP_CTH)  : *eval_equation = 1 / tanh(left_eval);              break;
        default : assert(0);
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

    int max_ind = (int)((graph->right_border - graph->left_border) / graph->step_values) + 1;

    int num_var = 0;
    err = get_num_eval_var(new_tree, &num_var);
    if (err) return err;

    for (int i = 0; i < max_ind; i++) {
        double x_value = i * graph->step_values + graph->left_border;
        new_tree->variables->var[num_var].value = x_value;
        x[i] = x_value;
        double y_value = 0;
        err = tree_eval(new_tree, &y_value);
        if (err) return err;
        double new_y = clamp_double(y_value, graph->min_value, graph->max_value);
        if (!is_double_equal(y_value, new_y))
            y[i] = graph->max_value * 2;
        else 
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
    while (1) {
        printf(print_lcyan("Input name of variable: "));
        int count_read = scanf("%s", name_var);

        if (count_read != 1) {
            clean_stdin();
            printf(print_lred("Wrong argument, you should input name of existing variable\n"));
        }

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