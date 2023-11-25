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
};

const OP OPs[] = {
    {OP_ERR,    "$"},
    {OP_ADD,    "+"},
    {OP_SUB,    "-"},
    {OP_MUL,    "*"},
    {OP_DIV,    "/"},
    {OP_SQRT,   "sqrt"},
    {OP_POW,    "^"},
    {OP_SIN,    "sin"},
    {OP_COS,    "cos"},
};
const int COUNT_OPs = sizeof(OPs) / sizeof(OP);

#endif // OPERATORS_H