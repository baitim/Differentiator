#ifndef OPERATORS_H
#define OPERATORS_H

enum TypeOP {
    OP_ERR = -1,
    OP_ADD =  1,
    OP_SUB =  2,
    OP_MUL =  3,
    OP_DIV =  4,
};

struct OP {
    TypeOP type_op;
    const char* name;
};

const OP OPs[] = {
    {OP_ERR, "$"},
    {OP_ADD, "+"},
    {OP_SUB, "-"},
    {OP_MUL, "*"},
    {OP_DIV, "/"},
};
const int COUNT_OPs = sizeof(OPs) / sizeof(OP);

#endif // OPERATORS_H