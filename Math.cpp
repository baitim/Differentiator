#include "Math.h"

double clamp_double(double x, double min, double max)
{
    return MIN(MAX(x, min), max);
}

double my_pow(double x, double st)
{
    if (is_double_equal(st, (int)st))
        return powf(x, (int)st);
    else
        return pow(x, st);
}

double powf(double x, int st)
{
    if (st == 0) return 1;
    if (st % 2 == 1) return x * powf(x, st - 1);
    double z = powf(x, st / 2);
    return z * z;
}

int is_double_equal(double x, double y)
{
    return (fabs(x - y) <= EPSILON);
}

int is_double_less(double x, double y)
{
    return (x - y <= EPSILON);
}

int is_double_above(double x, double y)
{
    return (y - x <= EPSILON);
}