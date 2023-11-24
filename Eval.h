#ifndef EVAL_H
#define EVAL_H

#include "Errors.h"
#include "Tree.h"

ErrorCode tree_eval(Tree* tree, double* ans_eval);

#endif // EVAL_H