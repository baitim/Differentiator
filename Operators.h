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

#include "Node.h"

struct Operator {
    TypeOperator type_op;
    const char* name;
    int ops_num;
    ErrorCode (*eval)(double* eval_equation, double left_eval, double right_eval);
    ErrorCode (*diff)(TreeNode* node, int num_var);
    ErrorCode (*simp)(TreeNode* node);
};

enum OperatorType {
    OP_TYPE_ERR = -1,
    OP_TYPE_UN =   1,
    OP_TYPE_BIN =  2,
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

ErrorCode err_diff      (TreeNode* node, int num_var);
ErrorCode add_diff      (TreeNode* node, int num_var);
ErrorCode sub_diff      (TreeNode* node, int num_var);
ErrorCode mul_diff      (TreeNode* node, int num_var);
ErrorCode div_diff      (TreeNode* node, int num_var);
ErrorCode pow_diff      (TreeNode* node, int /*num_var*/);
ErrorCode log_diff      (TreeNode* node, int /*num_var*/);
ErrorCode ln_diff       (TreeNode* node, int num_var);
ErrorCode sqrt_diff     (TreeNode* node, int num_var);
ErrorCode sin_diff      (TreeNode* node, int num_var);
ErrorCode cos_diff      (TreeNode* node, int num_var);
ErrorCode tg_diff       (TreeNode* node, int num_var);
ErrorCode ctg_diff      (TreeNode* node, int num_var);
ErrorCode asin_diff     (TreeNode* node, int /*num_var*/);
ErrorCode acos_diff     (TreeNode* node, int /*num_var*/);
ErrorCode atg_diff      (TreeNode* node, int /*num_var*/);
ErrorCode actg_diff     (TreeNode* node, int /*num_var*/);
ErrorCode sh_diff       (TreeNode* node, int num_var);
ErrorCode ch_diff       (TreeNode* node, int num_var);
ErrorCode th_diff       (TreeNode* node, int /*num_var*/);
ErrorCode cth_diff      (TreeNode* node, int /*num_var*/);

ErrorCode err_simplify  (TreeNode* node);
ErrorCode add_simplify  (TreeNode* node);
ErrorCode sub_simplify  (TreeNode* node);
ErrorCode mul_simplify  (TreeNode* node);
ErrorCode div_simplify  (TreeNode* node);
ErrorCode pow_simplify  (TreeNode* node);
ErrorCode log_simplify  (TreeNode* node);
ErrorCode ln_simplify   (TreeNode* node);
ErrorCode sqrt_simplify (TreeNode* node);
ErrorCode sin_simplify  (TreeNode* node);
ErrorCode cos_simplify  (TreeNode* node);
ErrorCode tg_simplify   (TreeNode* node);
ErrorCode ctg_simplify  (TreeNode* node);
ErrorCode asin_simplify (TreeNode* node);
ErrorCode acos_simplify (TreeNode* node);
ErrorCode atg_simplify  (TreeNode* node);
ErrorCode actg_simplify (TreeNode* node);
ErrorCode sh_simplify   (TreeNode* node);
ErrorCode ch_simplify   (TreeNode* node);
ErrorCode th_simplify   (TreeNode* node);
ErrorCode cth_simplify  (TreeNode* node);

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