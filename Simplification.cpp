#include <assert.h>
#include <stdio.h>

#include "Math.h"
#include "Simplification.h"

static ErrorCode tree_simplify_ (TreeNode* node, int* is_change);
static ErrorCode node_simplify_ (TreeNode* node, int* is_change);
static ErrorCode num_simplify   (TreeNode* node, OperatorType operator_type, int* is_change);

ErrorCode tree_simplify(Tree* tree)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = ERROR_NO;

    int is_change = 0;
    while (1) {
        is_change = 0;
        err = tree_simplify_(tree->root, &is_change);
        if (err) return err;

        if (is_change == 0) break;
    }

    return ERROR_NO;
}

static ErrorCode tree_simplify_(TreeNode* node, int* is_change)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(node);
    if (err) return err;

    if (node->left)  err = tree_simplify_(node->left, is_change);
    if (err) return err;
    if (node->right) err = tree_simplify_(node->right, is_change);
    if (err) return err;

    err = node_simplify_(node, is_change);
    if (err) return err;

    return tree_verify(node);
}

static ErrorCode node_simplify_(TreeNode* node, int* is_change)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = ERROR_NO;

    if (node->type_value == TYPE_OP) {
        err = num_simplify(node, OPERATORS[(int)node->value].ops_num, is_change);
        if (err) return err;
        err = (*OPERATORS[(int)node->value].simp)(node, is_change);
        if (err) return err;
    }

    return ERROR_NO;
}

static ErrorCode num_simplify(TreeNode* node, OperatorType operator_type, int* is_change)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    if (operator_type == OP_TYPE_BIN && node->left->type_value == TYPE_NUM &&
        node->right->type_value == TYPE_NUM) {

        (*is_change)++;
        node->depth =       1;
        node->type_value =  TYPE_NUM;
        double eval_node = -1;
        err = (*OPERATORS[(int)node->value].eval)(&eval_node, node->left->value, node->right->value);
        if (err) return err;
        node->value = eval_node;

        for (TreeNode* np = node->parent; np; np = np->parent)
            np->depth = MAX(np->left->depth, np->right->depth) + 1;

        err = node_delete(node->left);
        if (err) return err;
        node->left = nullptr;
        err = node_delete(node->right);
        if (err) return err;
        node->right = nullptr;
    } else if (operator_type == OP_TYPE_UN && node->left->type_value == TYPE_NUM) {
        (*is_change)++;
        node->depth =       1;
        node->type_value =  TYPE_NUM;
        double eval_node = -1;
        double right_node_eval = 0;
        err = (*OPERATORS[(int)node->value].eval)(&eval_node, node->left->value, right_node_eval);
        if (err) return err;
        node->value = eval_node;

        for (TreeNode* np = node->parent; np; np = np->parent)
            np->depth = MAX(np->left->depth, np->right->depth) + 1;

        err = node_delete(node->left);
        if (err) return err;
        node->left =  nullptr;
    }

    return ERROR_NO;
}

ErrorCode err_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);

    printf("Called err_simplify with node = %p\n", node);

    return ERROR_NO;
}

ErrorCode add_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode sub_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode mul_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode div_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode pow_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode log_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode ln_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode sqrt_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode sin_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode cos_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode tg_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode ctg_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode asin_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode acos_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode atg_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode actg_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode sh_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode ch_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode th_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode cth_simplify(TreeNode* node, int* /*is_change*/)
{
    assert(node);
    return ERROR_NO;
}
