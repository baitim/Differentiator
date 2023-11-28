#ifndef MATH_H
#define MATH_H

#include <math.h>

static const double EPSILON = 1e-9;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

double clamp_double (double x, double min, double max);
double my_pow       (double x, double st);
double powf         (double x, int st);
int is_double_equal (double x, double y);

#endif // MATH_H