#ifndef EVAL_H
#define EVAL_H

#include "Errors.h"
#include "Tree.h"

struct EvalPoints {
    const double left_board;
    const double right_board;
    const double max_value;
    const double min_value;
    const double step_values;
};

ErrorCode tree_eval(Tree* tree, double* ans_eval);
ErrorCode tree_get_points(Tree* tree, EvalPoints* graph, double* x, double* y);

#endif // EVAL_H