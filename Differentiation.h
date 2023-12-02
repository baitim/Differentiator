#ifndef DIFF_H
#define DIFF_H

#include "Errors.h"
#include "Equation.h"

ErrorCode equation_diff         (Equation* equation, int num_var);
ErrorCode equation_diff_report  (Equation* equation, int num_var);

#endif // DIFF_H