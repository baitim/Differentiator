#include <assert.h>
#include <stdlib.h>

#include "Node.h"
#include "Math.h"

ErrorCode node_init(TreeNode** node)
{
    assert(node);
    
    *node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if (!*node) return ERROR_ALLOC_FAIL;

    (*node)->type_value =   TYPE_ERR;
    (*node)->value =        POISON_VALUE;
    (*node)->depth =        POISON_VALUE;
    (*node)->parent =       nullptr;
    (*node)->left =         nullptr;
    (*node)->right =        nullptr;

    return ERROR_NO;
}

ErrorCode node_init_num(TreeNode** node, int value)
{
    assert(node);
    
    *node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if (!*node) return ERROR_ALLOC_FAIL;

    (*node)->type_value =   TYPE_NUM;
    (*node)->value =        value;
    (*node)->depth =        1;
    (*node)->parent =       nullptr;
    (*node)->left =         nullptr;
    (*node)->right =        nullptr;

    return ERROR_NO;
}

ErrorCode node_delete(TreeNode* node) 
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(node);
    if (err) return err;

    if (node->left)  err = node_delete(node->left);
    if (err) return err;
    if (node->right) err = node_delete(node->right);
    if (err) return err;
    
    node->depth = POISON_VALUE;

    free(node);
    return ERROR_NO;
}

ErrorCode node_copy(TreeNode* node, TreeNode** new_node)
{
    assert(node);
    
    ErrorCode err = ERROR_NO;

    err = node_init(new_node);
    if (err) return err;

    (*new_node)->type_value =   node->type_value;
    (*new_node)->value =        node->value;
    (*new_node)->depth =        node->depth;

    if (node->left)  err = node_copy(node->left,  &(*new_node)->left);
    if (err) return err;

    if (node->right) err = node_copy(node->right, &(*new_node)->right);
    if (err) return err;

    if ((*new_node)->left)  (*new_node)->left->parent =  *new_node;
    if ((*new_node)->right) (*new_node)->right->parent = *new_node;

    return ERROR_NO;
}

ErrorCode node_insert_op(TreeNode** dest, TypeOperator operator_, 
                         TreeNode* left, TreeNode* right)
{   
    assert(dest);
    ErrorCode err = ERROR_NO;

    if (!(*dest)) err = node_init(dest);
    if (err) return err;

    if ((*dest)->left) err =  node_delete((*dest)->left);
    if (err) return err;
    if ((*dest)->right) err = node_delete((*dest)->right);
    if (err) return err;

    (*dest)->type_value =  TYPE_OP;
    (*dest)->value =       operator_;

    if (left)   err = node_copy(left, &(*dest)->left);
    else        (*dest)->left = nullptr;
    if (err) return err;

    if (right)  err = node_copy(right, &(*dest)->right);
    else        (*dest)->right = nullptr;
    if (err) return err;

    int depth = 0;
    err = node_get_depth((*dest), &depth);
    if (err) return err;
    (*dest)->depth = depth;

    if ((*dest)->left)  (*dest)->left->parent =  (*dest);
    if ((*dest)->right) (*dest)->right->parent = (*dest);

    return ERROR_NO;
}

ErrorCode node_get_depth(TreeNode* node, int* depth)
{
    assert(node);

    int left_depth =  0;
    int right_depth = 0;
    if (node->left)  left_depth =  node->left->depth;
    if (node->right) right_depth = node->right->depth;
    (*depth) = MAX(left_depth, right_depth) + 1;

    return ERROR_NO;
}