#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "ANSI_colors.h"
#include "Evaluation.h"
#include "Input.h"
#include "Math.h"

static ErrorCode tree_eval_ (TreeNode* node, Variables* vars, double* eval_equation);
static ErrorCode node_eval_ (TreeNode* node, Variables* vars, double* eval_equation,
                             double left_eval, double right_eval);
static ErrorCode op_eval    (TreeNode* node, double* eval_equation, double left_eval, double right_eval);

ErrorCode tree_eval(Tree* tree, double* eval_equation)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = ERROR_NO;

    err = tree_eval_(tree->root, tree->variables, eval_equation);
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
        if (vars->var[(int)node->value].valid == 0) return ERROR_INVALID_VAR_EVAL;
        *eval_equation = vars->var[(int)node->value].value;
    } else if (node->type_value == TYPE_OP) {
        op_eval(node, eval_equation, left_eval, right_eval);
    }

    return ERROR_NO;
}

static ErrorCode op_eval(TreeNode* node, double* eval_equation, double left_eval, double right_eval)
{
    assert(node);

    ErrorCode err = (*OPERATORS[(int)node->value].eval)(eval_equation, left_eval, right_eval);
    if (err) return err;

    return ERROR_NO;
}

ErrorCode tree_get_points(Tree* tree, int num_var, EvalPoints* graph, double* x, double* y)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(tree->root);
    if (err) return err;

    Tree* new_tree = nullptr;
    err = tree_copy(tree, "TreeGetPoints", &new_tree);
    if (err) return err;

    int max_ind = (int)((graph->right_border - graph->left_border) / graph->step_values) + 1;

    double old_value_var = 0;
    if (tree->variables->var[num_var].valid) old_value_var = tree->variables->var[num_var].value;
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
    if (tree->variables->var[num_var].valid) tree->variables->var[num_var].value = old_value_var;

    err = tree_delete(new_tree);
    if (err) return err;

    return tree_verify(tree->root);
}

ErrorCode err_eval(double* eval_equation, double left_eval, double right_eval)
{

    printf("Called err_eval with eval = %lf, left_eval = %lf, right_eval = %lf\n",
            *eval_equation, left_eval, right_eval);
    assert(0);
}

ErrorCode add_eval(double* eval_equation, double left_eval, double right_eval)
{
    *eval_equation = left_eval + right_eval;
    return ERROR_NO;
}

ErrorCode sub_eval(double* eval_equation, double left_eval, double right_eval)
{
    *eval_equation = left_eval - right_eval;
    return ERROR_NO;
}

ErrorCode mul_eval(double* eval_equation, double left_eval, double right_eval)
{
    *eval_equation = left_eval * right_eval;
    return ERROR_NO;
}

ErrorCode div_eval(double* eval_equation, double left_eval, double right_eval)
{
    if (is_double_equal(right_eval, 0.f))
        return ERROR_DIVISION_ZERO;
    *eval_equation = left_eval / right_eval;
    return ERROR_NO;
}

ErrorCode pow_eval(double* eval_equation, double left_eval, double right_eval)
{
    *eval_equation = my_pow(left_eval, right_eval);
    return ERROR_NO;
}

ErrorCode log_eval(double* eval_equation, double left_eval, double right_eval)
{
    *eval_equation = log(left_eval) / log(right_eval);
    return ERROR_NO;
}

ErrorCode ln_eval(double* eval_equation, double left_eval, double /*right_eval*/)
{
    *eval_equation = log(left_eval);
    return ERROR_NO;
}

ErrorCode sqrt_eval(double* eval_equation, double left_eval, double /*right_eval*/)
{
    *eval_equation = sqrt(left_eval);
    return ERROR_NO;
}

ErrorCode sin_eval(double* eval_equation, double left_eval, double /*right_eval*/)
{
    *eval_equation = sin(left_eval);
    return ERROR_NO;
}

ErrorCode cos_eval(double* eval_equation, double left_eval, double /*right_eval*/)
{
    *eval_equation = cos(left_eval);
    return ERROR_NO;
}

ErrorCode tg_eval(double* eval_equation, double left_eval, double /*right_eval*/)
{
    *eval_equation = tan(left_eval);
    return ERROR_NO;
}

ErrorCode ctg_eval(double* eval_equation, double left_eval, double /*right_eval*/)
{
    *eval_equation = 1 / tan(left_eval);
    return ERROR_NO;
}

ErrorCode asin_eval(double* eval_equation, double left_eval, double /*right_eval*/)
{
    *eval_equation = asin(left_eval);
    return ERROR_NO;
}

ErrorCode acos_eval(double* eval_equation, double left_eval, double /*right_eval*/)
{
    *eval_equation = acos(left_eval);
    return ERROR_NO;
}

ErrorCode atg_eval(double* eval_equation, double left_eval, double /*right_eval*/)
{
    *eval_equation = atan(left_eval);
    return ERROR_NO;
}

ErrorCode actg_eval(double* eval_equation, double left_eval, double /*right_eval*/)
{
    *eval_equation = atan(1 / left_eval);
    return ERROR_NO;
}

ErrorCode sh_eval(double* eval_equation, double left_eval, double /*right_eval*/)
{
    *eval_equation = sinh(left_eval);
    return ERROR_NO;
}

ErrorCode ch_eval(double* eval_equation, double left_eval, double /*right_eval*/)
{
    *eval_equation = cosh(left_eval);
    return ERROR_NO;
}

ErrorCode th_eval(double* eval_equation, double left_eval, double /*right_eval*/)
{
    *eval_equation = tanh(left_eval);
    return ERROR_NO;
}

ErrorCode cth_eval(double* eval_equation, double left_eval, double /*right_eval*/)
{
    *eval_equation = 1 / tanh(left_eval);
    return ERROR_NO;
}
