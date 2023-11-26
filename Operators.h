#ifndef OPERATORS_H
#define OPERATORS_H

enum TypeOP {
    OP_ERR = -1,
    OP_ADD =  1,
    OP_SUB =  2,
    OP_MUL =  3,
    OP_DIV =  4,
    OP_SQRT = 5,
    OP_POW =  6,
    OP_SIN =  7,
    OP_COS =  8,
};

struct OP {
    TypeOP type_op;
    const char* name;
    int ops_num;
};

const OP OPs[] = {
    {OP_ERR,    "$",   -1},
    {OP_ADD,    "+",    2},
    {OP_SUB,    "-",    2},
    {OP_MUL,    "*",    2},
    {OP_DIV,    "/",    2},
    {OP_SQRT,   "sqrt", 1},
    {OP_POW,    "^",    2},
    {OP_SIN,    "sin",  1},
    {OP_COS,    "cos",  1},
};
const int COUNT_OPs = sizeof(OPs) / sizeof(OP);

#endif // OPERATORS_H