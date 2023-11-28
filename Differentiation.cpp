#include <assert.h>
#include <string.h>

#include "ANSI_colors.h"
#include "Differentiation.h"
#include "Math.h"
#include "Input.h"

static ErrorCode tree_diff_     (TreeNode* node, int num_var);
static ErrorCode node_diff      (TreeNode* node, int num_var);
static ErrorCode var_diff       (TreeNode* node, int num_var);
static ErrorCode op_diff        (TreeNode* node, int num_var);

ErrorCode tree_diff(Tree* tree, int num_var)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = ERROR_NO;

    err = tree_diff_(tree->root, num_var);
    if (err) return err;

    return ERROR_NO;
}

static ErrorCode tree_diff_(TreeNode* node, int num_var)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(node);
    if (err) return err;

    err = node_diff(node, num_var);
    if (err) return err;

    return tree_verify(node);
}

static ErrorCode node_diff(TreeNode* node, int num_var)
{
    assert(node);

    if (node->type_value == TYPE_NUM) {
        node->value = 0;
    } else if (node->type_value == TYPE_VAR) {
        var_diff(node, num_var);
    } else if (node->type_value == TYPE_OP) {
        op_diff(node, num_var);
    }

    return ERROR_NO;
}

static ErrorCode var_diff(TreeNode* node, int num_var)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(node);
    if (err) return err;

    if (!is_double_equal((int)node->value, num_var)) {
        node->type_value = TYPE_NUM;
        node->value = 0;
    } else {
        node->type_value = TYPE_NUM;
        node->value = 1;
    }

    return ERROR_NO;
}

static ErrorCode op_diff(TreeNode* node, int num_var)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(node);
    if (err) return err;

    assert(node);

    err = (*OPERATORS[(int)node->value].diff)(node, num_var);
    if (err) return err;

    return err;
}

ErrorCode err_diff(TreeNode* node, int num_var)
{
    assert(node);
    printf("Called err_diff with node = %p and num = %d\n", node, num_var);
    assert(0);
}

ErrorCode add_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    err = node_diff(node->left, num_var);
    if (err) return err;
    err = node_diff(node->right, num_var);
    if (err) return err;

    return ERROR_NO;
}

ErrorCode sub_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    err = node_diff(node->left, num_var);
    if (err) return err;
    err = node_diff(node->right, num_var);
    if (err) return err;

    return ERROR_NO;
}

ErrorCode mul_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    TreeNode* node_left_copy =  nullptr;
    err = node_copy(node->left, &node_left_copy);
    if (err) return err;

    TreeNode* node_right_copy = nullptr;
    err = node_copy(node->right, &node_right_copy);
    if (err) return err;

    TreeNode* node_left_diff =  nullptr;
    err = node_copy(node_left_copy, &node_left_diff);
    if (err) return err;
    err = node_diff(node_left_diff, num_var);
    if (err) return err;

    TreeNode* node_right_diff =  nullptr;
    err = node_copy(node_right_copy, &node_right_diff);
    if (err) return err;
    err = node_diff(node_right_diff, num_var);
    if (err) return err;

    node->type_value =  TYPE_OP;
    node->value =       OP_ADD;
    err = node_insert_op(node->left,  OP_MUL, node_left_diff,  node_right_copy);
    if (err) return err;
    err = node_insert_op(node->right, OP_MUL, node_left_copy, node_right_diff);
    if (err) return err;
    
    for (TreeNode* np = node; np; np = np->parent)
        np->depth = MAX(np->left->depth, np->right->depth) + 1;

    err = node_delete(node_left_copy);
    if (err) return err;
    err = node_delete(node_right_copy);
    if (err) return err;
    err = node_delete(node_left_diff);
    if (err) return err;
    err = node_delete(node_right_diff);
    if (err) return err;

    return ERROR_NO;
}

ErrorCode div_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    TreeNode* node_right_copy = nullptr;
    err = node_copy(node->right, &node_right_copy);
    if (err) return err;

    TreeNode* node_value_two = nullptr;
    err = node_init(&node_value_two);
    if (err) return err;
    node_value_two->depth = 1;
    node_value_two->type_value = TYPE_NUM;
    node_value_two->value = 2;

    TreeNode* numerator = nullptr;
    err = node_copy(node, &numerator);
    if (err) return err;
    numerator->value = OP_MUL;
    err = node_diff(numerator, num_var);
    if (err) return err;
    numerator->value = OP_SUB;

    TreeNode* denominator = nullptr;
    err = node_init(&denominator);
    if (err) return err;
    err = node_insert_op(denominator, OP_POW, node_right_copy, node_value_two);
    if (err) return err;

    err = node_insert_op(node, OP_DIV, numerator, denominator);
    if (err) return err;
    
    for (TreeNode* np = node; np; np = np->parent) {
        int depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }

    err = node_delete(node_right_copy);
    if (err) return err;
    err = node_delete(node_value_two);
    if (err) return err;
    err = node_delete(numerator);
    if (err) return err;
    err = node_delete(denominator);
    if (err) return err;
    
    return err;
}

ErrorCode pow_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;
    
    return err;
}

ErrorCode log_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;
    
    return err;
}

ErrorCode ln_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;
    
    return err;
}

ErrorCode sqrt_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;
    
    return err;
}

ErrorCode sin_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;
    
    return err;
}

ErrorCode cos_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;
    
    return err;
}

ErrorCode tg_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;
    
    return err;
}

ErrorCode ctg_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;
    
    return err;
}

ErrorCode asin_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;
    
    return err;
}

ErrorCode acos_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;
    
    return err;
}

ErrorCode atg_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;
    
    return err;
}

ErrorCode actg_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;
    
    return err;
}

ErrorCode sh_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;
    
    return err;
}

ErrorCode ch_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;
    
    return err;
}

ErrorCode th_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;
    
    return err;
}

ErrorCode cth_diff(TreeNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;
    
    return err;
}