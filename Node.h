#ifndef NODE_H
#define NODE_H

#include "Operators.h"
#include "Variables.h"
#include "Errors.h"

static const int POISON_INT = -1;

enum TreeDataType {
    TYPE_ERR = -1,
    TYPE_NUM =  1,
    TYPE_OP  =  2,
    TYPE_VAR =  3,
};

struct TreeNode {
    TreeDataType type_value;
    double value;
    int depth;
    TreeNode* left;
    TreeNode* right;
    TreeNode* parent;
};

ErrorCode node_init     (TreeNode** node);
ErrorCode node_delete   (TreeNode* node);
ErrorCode node_copy     (TreeNode* node, TreeNode** new_node);
ErrorCode node_insert_op(TreeNode* dest, TypeOperator operator_, 
                         TreeNode* left, TreeNode* right);

#include "Tree.h"

#endif // NODE_H