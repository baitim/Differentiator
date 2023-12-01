#ifndef EVAL_H
#define EVAL_H

#include "Errors.h"
#include "Equation.h"

struct EvalPoints {
    const double left_border;
    const double right_border;
    const double max_value;
    const double min_value;
    const double step_values;
};

ErrorCode equation_eval         (Equation* equation, double* ans_eval);
ErrorCode equation_get_points   (Equation* equation, int num_var, EvalPoints* graph, double* x, double* y);

#endif // EVAL_H