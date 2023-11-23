#include <assert.h>
#include <math.h>

#include "ANSI_colors.h"
#include "Diff.h"

const double EPSILON = 1e-9;

static ErrorCode tree_eval_ (Node* node, Variables* vars, double* ans_eval);
static ErrorCode node_eval_ (Node* node, Variables* vars, double* ans_eval,
                             double left_eval, double right_eval);
static ErrorCode var_eval   (Node* node, Variables* vars, double* ans_eval);
static ErrorCode op_eval    (Node* node, double* ans_eval, double left_eval, double right_eval);
static ErrorCode clean_stdin();
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
    if (!vars->valid[(int)node->value]) {
        printf(print_lcyan("Input value of %s: "), vars->names[(int)node->value]);
        int count_read = scanf("%lf", &vars->value[(int)node->value]);
        while (count_read != 1) {
            clean_stdin();
            printf(print_lred("Wrong argument, you should input double, try again: "));
            count_read = scanf("%lf", &vars->value[(int)node->value]);
        }
        vars->valid[(int)node->value] = 1;
    }
    *ans_eval = vars->value[(int)node->value];

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

static ErrorCode clean_stdin()
{
    while (getchar() != '\n')
        ;
    return ERROR_NO;
}

static int is_double_equal(double x, double y)
{
    return (fabs(x - y) <= EPSILON);
}