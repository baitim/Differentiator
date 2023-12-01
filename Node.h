#ifndef NODE_H
#define NODE_H

#include "Errors.h"

static const size_t POISON_VALUE = (size_t)-0xbe;

enum Branch {
    BRANCH_ERR =   0,
    BRANCH_LEFT = -1,
    BRANCH_RIGHT = 1,
};

enum EquationDataType {
    TYPE_ERR = -1,
    TYPE_NUM =  1,
    TYPE_OP  =  2,
    TYPE_VAR =  3,
};

struct EquationNode {
    EquationDataType type_value;
    double value;
    size_t depth;
    EquationNode* left;
    EquationNode* right;
    EquationNode* parent;
};

#include "Operators.h"
#include "Variables.h"

ErrorCode node_init     (EquationNode** node);
ErrorCode node_init_num (EquationNode** node, double value);
ErrorCode node_delete   (EquationNode** node);
ErrorCode node_copy     (EquationNode* node, EquationNode** new_node);
ErrorCode node_insert_op(EquationNode** dest, TypeOperator operator_, 
                         EquationNode* left, EquationNode* right);
ErrorCode node_get_depth(EquationNode* node, size_t* depth);

#include "Equation.h"

#endif // NODE_H