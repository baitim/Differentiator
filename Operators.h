#ifndef OPERATORS_H
#define OPERATORS_H

enum TypeOperator {
    OP_ERR =   0,
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

#include "Node.h"

enum OperatorType {
    OP_TYPE_ERR = -1,
    OP_TYPE_UN =   1,
    OP_TYPE_BIN =  2,
};

struct Operator {
    TypeOperator type_op;
    const char* name;
    OperatorType ops_num;
    ErrorCode (*eval)(double* eval_equation, double left_eval, double right_eval);
    ErrorCode (*diff)(EquationNode* node,  int num_var);
    ErrorCode (*simp)(EquationNode** node, int* is_change);
};

ErrorCode err_eval      (double* eval_equation, double left_eval, double right_eval);
ErrorCode add_eval      (double* eval_equation, double left_eval, double right_eval);
ErrorCode sub_eval      (double* eval_equation, double left_eval, double right_eval);
ErrorCode mul_eval      (double* eval_equation, double left_eval, double right_eval);
ErrorCode div_eval      (double* eval_equation, double left_eval, double right_eval);
ErrorCode pow_eval      (double* eval_equation, double left_eval, double right_eval);
ErrorCode log_eval      (double* eval_equation, double left_eval, double right_eval);
ErrorCode ln_eval       (double* eval_equation, double left_eval, double /*right_eval*/);
ErrorCode sqrt_eval     (double* eval_equation, double left_eval, double /*right_eval*/);
ErrorCode sin_eval      (double* eval_equation, double left_eval, double /*right_eval*/);
ErrorCode cos_eval      (double* eval_equation, double left_eval, double /*right_eval*/);
ErrorCode tg_eval       (double* eval_equation, double left_eval, double /*right_eval*/);
ErrorCode ctg_eval      (double* eval_equation, double left_eval, double /*right_eval*/);
ErrorCode asin_eval     (double* eval_equation, double left_eval, double /*right_eval*/);
ErrorCode acos_eval     (double* eval_equation, double left_eval, double /*right_eval*/);
ErrorCode atg_eval      (double* eval_equation, double left_eval, double /*right_eval*/);
ErrorCode actg_eval     (double* eval_equation, double left_eval, double /*right_eval*/);
ErrorCode sh_eval       (double* eval_equation, double left_eval, double /*right_eval*/);
ErrorCode ch_eval       (double* eval_equation, double left_eval, double /*right_eval*/);
ErrorCode th_eval       (double* eval_equation, double left_eval, double /*right_eval*/);
ErrorCode cth_eval      (double* eval_equation, double left_eval, double /*right_eval*/);

ErrorCode err_diff      (EquationNode* node, int num_var);
ErrorCode add_diff      (EquationNode* node, int num_var);
ErrorCode sub_diff      (EquationNode* node, int num_var);
ErrorCode mul_diff      (EquationNode* node, int num_var);
ErrorCode div_diff      (EquationNode* node, int num_var);
ErrorCode pow_diff      (EquationNode* node, int num_var);
ErrorCode log_diff      (EquationNode* node, int num_var);
ErrorCode ln_diff       (EquationNode* node, int num_var);
ErrorCode sqrt_diff     (EquationNode* node, int num_var);
ErrorCode sin_diff      (EquationNode* node, int num_var);
ErrorCode cos_diff      (EquationNode* node, int num_var);
ErrorCode tg_diff       (EquationNode* node, int num_var);
ErrorCode ctg_diff      (EquationNode* node, int num_var);
ErrorCode asin_diff     (EquationNode* node, int num_var);
ErrorCode acos_diff     (EquationNode* node, int num_var);
ErrorCode atg_diff      (EquationNode* node, int num_var);
ErrorCode actg_diff     (EquationNode* node, int num_var);
ErrorCode sh_diff       (EquationNode* node, int num_var);
ErrorCode ch_diff       (EquationNode* node, int num_var);
ErrorCode th_diff       (EquationNode* node, int num_var);
ErrorCode cth_diff      (EquationNode* node, int num_var);

ErrorCode err_simplify  (EquationNode** node, int* is_change);
ErrorCode add_simplify  (EquationNode** node, int* is_change);
ErrorCode sub_simplify  (EquationNode** node, int* is_change);
ErrorCode mul_simplify  (EquationNode** node, int* is_change);
ErrorCode div_simplify  (EquationNode** node, int* is_change);
ErrorCode pow_simplify  (EquationNode** node, int* /*is_change*/);
ErrorCode log_simplify  (EquationNode** node, int* /*is_change*/);
ErrorCode ln_simplify   (EquationNode** node, int* /*is_change*/);
ErrorCode sqrt_simplify (EquationNode** node, int* /*is_change*/);
ErrorCode sin_simplify  (EquationNode** node, int* /*is_change*/);
ErrorCode cos_simplify  (EquationNode** node, int* /*is_change*/);
ErrorCode tg_simplify   (EquationNode** node, int* /*is_change*/);
ErrorCode ctg_simplify  (EquationNode** node, int* /*is_change*/);
ErrorCode asin_simplify (EquationNode** node, int* /*is_change*/);
ErrorCode acos_simplify (EquationNode** node, int* /*is_change*/);
ErrorCode atg_simplify  (EquationNode** node, int* /*is_change*/);
ErrorCode actg_simplify (EquationNode** node, int* /*is_change*/);
ErrorCode sh_simplify   (EquationNode** node, int* /*is_change*/);
ErrorCode ch_simplify   (EquationNode** node, int* /*is_change*/);
ErrorCode th_simplify   (EquationNode** node, int* /*is_change*/);
ErrorCode cth_simplify  (EquationNode** node, int* /*is_change*/);

const Operator OPERATORS[] = {
    {OP_ERR,    "$",        OP_TYPE_ERR,    err_eval,   err_diff,   err_simplify    },
    {OP_ADD,    "+",        OP_TYPE_BIN,    add_eval,   add_diff,   add_simplify    },
    {OP_SUB,    "-",        OP_TYPE_BIN,    sub_eval,   sub_diff,   sub_simplify    },
    {OP_MUL,    "*",        OP_TYPE_BIN,    mul_eval,   mul_diff,   mul_simplify    },
    {OP_DIV,    "/",        OP_TYPE_BIN,    div_eval,   div_diff,   div_simplify    },
    {OP_POW,    "^",        OP_TYPE_BIN,    pow_eval,   pow_diff,   pow_simplify    },
    {OP_LOG,    "log",      OP_TYPE_BIN,    log_eval,   log_diff,   log_simplify    },
    {OP_LN,     "ln",       OP_TYPE_UN,     ln_eval,    ln_diff,    ln_simplify     },
    {OP_SQRT,   "sqrt",     OP_TYPE_UN,     sqrt_eval,  sqrt_diff,  sqrt_simplify   },
    {OP_SIN,    "sin",      OP_TYPE_UN,     sin_eval,   sin_diff,   sin_simplify    },
    {OP_COS,    "cos",      OP_TYPE_UN,     cos_eval,   cos_diff,   cos_simplify    },
    {OP_TG,     "tg",       OP_TYPE_UN,     tg_eval,    tg_diff,    tg_simplify     },
    {OP_CTG,    "ctg",      OP_TYPE_UN,     ctg_eval,   ctg_diff,   ctg_simplify    },
    {OP_ASIN,   "asin",     OP_TYPE_UN,     asin_eval,  asin_diff,  asin_simplify   },
    {OP_ACOS,   "acos",     OP_TYPE_UN,     acos_eval,  acos_diff,  acos_simplify   },
    {OP_ATG,    "atg",      OP_TYPE_UN,     atg_eval,   atg_diff,   atg_simplify    },
    {OP_ACTG,   "actg",     OP_TYPE_UN,     actg_eval,  actg_diff,  actg_simplify   },
    {OP_SH,     "sh",       OP_TYPE_UN,     sh_eval,    sh_diff,    sh_simplify     },
    {OP_CH,     "ch",       OP_TYPE_UN,     ch_eval,    ch_diff,    ch_simplify     },
    {OP_TH,     "th",       OP_TYPE_UN,     th_eval,    th_diff,    th_simplify     },
    {OP_CTH,    "cth",      OP_TYPE_UN,     cth_eval,   cth_diff,   cth_simplify    },
};
const int COUNT_OPs = sizeof(OPERATORS) / sizeof(Operator);

#endif // OPERATORS_H