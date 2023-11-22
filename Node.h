#ifndef NODE_H
#define NODE_H

#include "Operators.h"
#include "Variables.h"

enum TypeData {
    TYPE_ERR = -1,
    TYPE_NUM =  1,
    TYPE_OP  =  2,
    TYPE_VAR =  3,
};

struct Node {
    TypeData type_value;
    double value;
    int size;
    int dep;
    Node* left;
    Node* right;
};

#include "Tree.h"

#endif // NODE_H