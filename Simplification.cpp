#include <assert.h>

#include "Simplification.h"

#include "Node.h"

static ErrorCode tree_simplify_(TreeNode* node);
static ErrorCode node_simplify_(TreeNode* node);

ErrorCode tree_simplify(Tree* tree)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = ERROR_NO;

    err = tree_simplify_(tree->root);
    if (err) return err;

    return ERROR_NO;
}

static ErrorCode tree_simplify_(TreeNode* node)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(node);
    if (err) return err;

    if (node->left)  err = tree_simplify_(node->left);
    if (err) return err;
    if (node->right) err = tree_simplify_(node->right);
    if (err) return err;

    err = node_simplify_(node);
    if (err) return err;

    return tree_verify(node);
}

static ErrorCode node_simplify_(TreeNode* node)
{
    if (!node) return ERROR_INVALID_TREE;

    if (node->type_value == TYPE_OP) {
        ErrorCode err = (*OPERATORS[(int)node->value].simp)(node);
        if (err) return err;
    }

    return tree_verify(node);
}

ErrorCode err_simplify(TreeNode* node)
{
    assert(node);
    printf("Called err_simplify with node = %p\n", node);
    return ERROR_NO;
}

ErrorCode add_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode sub_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode mul_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode div_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode pow_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode log_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode ln_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode sqrt_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode sin_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode cos_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode tg_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode ctg_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode asin_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode acos_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode atg_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode actg_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode sh_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode ch_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode th_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode cth_simplify(TreeNode* node)
{
    assert(node);
    return ERROR_NO;
}
