#include <assert.h>
#include <stdio.h>

#include "Math.h"
#include "Simplification.h"

static ErrorCode tree_simplify_ (TreeNode* node, int* is_change, Branch branch);
static ErrorCode node_simplify_ (TreeNode* node, int* is_change, Branch branch);
static ErrorCode num_simplify   (TreeNode* node, OperatorType operator_type, int* is_change);

ErrorCode tree_simplify(Tree* tree)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = ERROR_NO;

    int is_change = 0;
    while (1) {
        is_change = 0;
        err = tree_simplify_(tree->root, &is_change, BRANCH_ERR);
        if (err) return err;

        if (is_change == 0) break;
    }

    return ERROR_NO;
}

static ErrorCode tree_simplify_(TreeNode* node, int* is_change, Branch branch)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(node);
    if (err) return err;

    if (node->left)  err = tree_simplify_(node->left,  is_change, BRANCH_LEFT);
    if (err) return err;
    if (node->right) err = tree_simplify_(node->right, is_change, BRANCH_RIGHT);
    if (err) return err;

    err = node_simplify_(node, is_change, branch);
    if (err) return err;

    return ERROR_NO;
}

static ErrorCode node_simplify_(TreeNode* node, int* is_change, Branch branch)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = ERROR_NO;

    if (node->type_value == TYPE_OP) {
        err = num_simplify(node, OPERATORS[(int)node->value].ops_num, is_change);
        if (err) return err;
    }

    if (node->type_value == TYPE_OP) {
        err = (*OPERATORS[(int)node->value].simp)(node, is_change, branch);
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

        for (TreeNode* np = node->parent; np; np = np->parent) {
            size_t depth = 0;
            err = node_get_depth(np, &depth);
            if (err) return err;
            np->depth = depth;
        }

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

        for (TreeNode* np = node->parent; np; np = np->parent) {
            size_t depth = 0;
            err = node_get_depth(np, &depth);
            if (err) return err;
            np->depth = depth;
        }

        err = node_delete(node->left);
        if (err) return err;
        node->left =  nullptr;
    }

    return ERROR_NO;
}

ErrorCode err_simplify(TreeNode* node, int* is_change, Branch /*branch*/)
{
    assert(node);

    printf("Called err_simplify with node = %p and is_change = %d\n", node, *is_change);

    return ERROR_NO;
}

ErrorCode add_simplify(TreeNode* node, int* is_change, Branch /*branch*/)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    if (node->left->type_value == TYPE_NUM && is_double_equal(node->left->value, 0)) {

        (*is_change)++;
        node->depth =       1;
        node->type_value =  TYPE_NUM;
        node->value =       node->right->value;

        for (TreeNode* np = node->parent; np; np = np->parent) {
            size_t depth = 0;
            err = node_get_depth(np, &depth);
            if (err) return err;
            np->depth = depth;
        }

        err = node_delete(node->left);
        if (err) return err;
        node->left = nullptr;
        err = node_delete(node->right);
        if (err) return err;
        node->right = nullptr;

        return ERROR_NO;
    }

    if (node->right->type_value == TYPE_NUM && is_double_equal(node->right->value, 0)) {

        (*is_change)++;
        node->depth =       1;
        node->type_value =  TYPE_NUM;
        node->value =       node->left->value;

        for (TreeNode* np = node->parent; np; np = np->parent) {
            size_t depth = 0;
            err = node_get_depth(np, &depth);
            if (err) return err;
            np->depth = depth;
        }

        err = node_delete(node->left);
        if (err) return err;
        node->left = nullptr;
        err = node_delete(node->right);
        if (err) return err;
        node->right = nullptr;

        return ERROR_NO;
    }

    return ERROR_NO;
}

ErrorCode sub_simplify(TreeNode* node, int* is_change, Branch /*branch*/)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    if (node->left->type_value == TYPE_NUM && is_double_equal(node->left->value, 0)) {

        (*is_change)++;
        node->depth =       1;
        node->type_value =  TYPE_NUM;
        node->value =       node->right->value;

        for (TreeNode* np = node->parent; np; np = np->parent) {
            size_t depth = 0;
            err = node_get_depth(np, &depth);
            if (err) return err;
            np->depth = depth;
        }

        err = node_delete(node->left);
        if (err) return err;
        node->left = nullptr;
        err = node_delete(node->right);
        if (err) return err;
        node->right = nullptr;

        return ERROR_NO;
    }

    if (node->right->type_value == TYPE_NUM && is_double_equal(node->right->value, 0)) {

        (*is_change)++;
        node->depth =       1;
        node->type_value =  TYPE_NUM;
        node->value =       node->left->value;

        for (TreeNode* np = node->parent; np; np = np->parent) {
            size_t depth = 0;
            err = node_get_depth(np, &depth);
            if (err) return err;
            np->depth = depth;
        }

        err = node_delete(node->left);
        if (err) return err;
        node->left = nullptr;
        err = node_delete(node->right);
        if (err) return err;
        node->right = nullptr;

        return ERROR_NO;
    }

    return ERROR_NO;
}

ErrorCode mul_simplify(TreeNode* node, int* is_change, Branch branch)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    if ((node->left->type_value ==  TYPE_NUM && is_double_equal(node->left->value,  0)) ||
        (node->right->type_value == TYPE_NUM && is_double_equal(node->right->value, 0))) {

        (*is_change)++;
        node->depth =       1;
        node->type_value =  TYPE_NUM;
        node->value =       0;

        for (TreeNode* np = node->parent; np; np = np->parent) {
            size_t depth = 0;
            err = node_get_depth(np, &depth);
            if (err) return err;
            np->depth = depth;
        }
        
        err = node_delete(node->left);
        if (err) return err;
        node->left = nullptr;
        err = node_delete(node->right);
        if (err) return err;
        node->right = nullptr;

        return ERROR_NO;
    }

    if (node->left->type_value == TYPE_NUM && is_double_equal(node->left->value, 1)) {

        (*is_change)++;
        TreeNode* node_copy_right = nullptr;
        err = node_copy(node->right, &node_copy_right);
        if (err) return err;

        if (branch == BRANCH_LEFT) {
            err = node_copy(node_copy_right, &node->parent->left);
            if (err) return err;
        }

        if (branch == BRANCH_RIGHT) {
            err = node_copy(node_copy_right, &node->parent->right);
            if (err) return err;
        }

        for (TreeNode* np = node->parent; np; np = np->parent) {
            size_t depth = 0;
            err = node_get_depth(np, &depth);
            if (err) return err;
            np->depth = depth;
        }

        err = node_delete(node_copy_right);
        if (err) return err;
        err = node_delete(node);
        if (err) return err;

        return ERROR_NO;
    }

    if (node->right->type_value == TYPE_NUM && is_double_equal(node->right->value, 1)) {

        (*is_change)++;
        TreeNode* node_copy_left = nullptr;
        err = node_copy(node->left, &node_copy_left);
        if (err) return err;

        if (branch == BRANCH_LEFT) {
            err = node_copy(node_copy_left, &node->parent->left);
            if (err) return err;
        }

        if (branch == BRANCH_RIGHT) {
            err = node_copy(node_copy_left, &node->parent->right);
            if (err) return err;
        }

        for (TreeNode* np = node->parent; np; np = np->parent) {
            size_t depth = 0;
            err = node_get_depth(np, &depth);
            if (err) return err;
            np->depth = depth;
        }

        err = node_delete(node_copy_left);
        if (err) return err;
        err = node_delete(node);
        if (err) return err;

        return ERROR_NO;
    }

    return ERROR_NO;
}

ErrorCode div_simplify(TreeNode* node, int* is_change, Branch /*branch*/)
{
    assert(node);
    printf("Called err_simplify with node = %p and is_change = %d\n", node, *is_change);
    return ERROR_NO;
}

ErrorCode pow_simplify(TreeNode* node, int* /*is_change*/, Branch /*branch*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode log_simplify(TreeNode* node, int* /*is_change*/, Branch /*branch*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode ln_simplify(TreeNode* node, int* /*is_change*/, Branch /*branch*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode sqrt_simplify(TreeNode* node, int* /*is_change*/, Branch /*branch*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode sin_simplify(TreeNode* node, int* /*is_change*/, Branch /*branch*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode cos_simplify(TreeNode* node, int* /*is_change*/, Branch /*branch*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode tg_simplify(TreeNode* node, int* /*is_change*/, Branch /*branch*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode ctg_simplify(TreeNode* node, int* /*is_change*/, Branch /*branch*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode asin_simplify(TreeNode* node, int* /*is_change*/, Branch /*branch*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode acos_simplify(TreeNode* node, int* /*is_change*/, Branch /*branch*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode atg_simplify(TreeNode* node, int* /*is_change*/, Branch /*branch*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode actg_simplify(TreeNode* node, int* /*is_change*/, Branch /*branch*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode sh_simplify(TreeNode* node, int* /*is_change*/, Branch /*branch*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode ch_simplify(TreeNode* node, int* /*is_change*/, Branch /*branch*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode th_simplify(TreeNode* node, int* /*is_change*/, Branch /*branch*/)
{
    assert(node);
    return ERROR_NO;
}

ErrorCode cth_simplify(TreeNode* node, int* /*is_change*/, Branch /*branch*/)
{
    assert(node);
    return ERROR_NO;
}
