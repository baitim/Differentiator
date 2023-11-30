#ifndef NODE_H
#define NODE_H

#include "Errors.h"

static const size_t POISON_VALUE = (size_t)-0xbe;

enum Branch {
    BRANCH_ERR =   0,
    BRANCH_LEFT = -1,
    BRANCH_RIGHT = 1,
};

enum TreeDataType {
    TYPE_ERR = -1,
    TYPE_NUM =  1,
    TYPE_OP  =  2,
    TYPE_VAR =  3,
};

struct TreeNode {
    TreeDataType type_value;
    double value;
    size_t depth;
    TreeNode* left;
    TreeNode* right;
    TreeNode* parent;
};

#include "Operators.h"
#include "Variables.h"

ErrorCode node_init     (TreeNode** node);
ErrorCode node_init_num (TreeNode** node, double value);
ErrorCode node_delete   (TreeNode** node);
ErrorCode node_copy     (TreeNode* node, TreeNode** new_node);
ErrorCode node_insert_op(TreeNode** dest, TypeOperator operator_, 
                         TreeNode* left, TreeNode* right);
ErrorCode node_get_depth(TreeNode* node, size_t* depth);

#include "Tree.h"

#endif // NODE_H