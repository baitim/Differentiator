#ifndef OPERATORS_H
#define OPERATORS_H

enum TypeOperator {
    OP_ERR =  -1,
    OP_ADD =   1,
    OP_SUB =   2,
    OP_MUL =   3,
    OP_DIV =   4,
    OP_POW =   5,
    OP_LOG =   6,
    OP_LN =    7,
    OP_SQRT =  8,
    OP_SIN =   9,
    OP_COS =  10,
    OP_TG =   11,
    OP_CTG =  12,
    OP_ASIN = 13,
    OP_ACOS = 14,
    OP_ATG =  15,
    OP_ACTG = 16,
    OP_SH =   17,
    OP_CH =   18,
    OP_TH =   19,
    OP_CTH =  20,
};

struct Operator {
    TypeOperator type_op;
    const char* name;
    int ops_num;
};

enum OperatorType {
    OP_TYPE_ERR = -1,
    OP_TYPE_UN =   1,
    OP_TYPE_BIN =  2,
};

const Operator OPERATORS[] = {
    {OP_ERR,    "$",        OP_TYPE_ERR},
    {OP_ADD,    "+",        OP_TYPE_BIN},
    {OP_SUB,    "-",        OP_TYPE_BIN},
    {OP_MUL,    "*",        OP_TYPE_BIN},
    {OP_DIV,    "/",        OP_TYPE_BIN},
    {OP_POW,    "^",        OP_TYPE_BIN},
    {OP_LOG,    "log",      OP_TYPE_BIN},
    {OP_LN,     "ln",       OP_TYPE_UN},
    {OP_SQRT,   "sqrt",     OP_TYPE_UN},
    {OP_SIN,    "sin",      OP_TYPE_UN},
    {OP_COS,    "cos",      OP_TYPE_UN},
    {OP_TG,     "tg",       OP_TYPE_UN},
    {OP_CTG,    "ctg",      OP_TYPE_UN},
    {OP_ASIN,   "asin",     OP_TYPE_UN},
    {OP_ACOS,   "acos",     OP_TYPE_UN},
    {OP_ATG,    "atg",      OP_TYPE_UN},
    {OP_ACTG,   "actg",     OP_TYPE_UN},
    {OP_SH,     "sh",       OP_TYPE_UN},
    {OP_CH,     "ch",       OP_TYPE_UN},
    {OP_TH,     "th",       OP_TYPE_UN},
    {OP_CTH,    "cth",      OP_TYPE_UN},
};
const int COUNT_OPs = sizeof(OPERATORS) / sizeof(Operator);

#endif // OPERATORS_H