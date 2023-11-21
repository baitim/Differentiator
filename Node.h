#ifndef NODE_H
#define NODE_H

enum TypeData {
    TYPE_NAN = -1,
    TYPE_NUM =  1,
    TYPE_OP  =  2,
};

struct Node {
    TypeData type_value;
    double value;
};

#include "Tree.h"

#endif // NODE_H