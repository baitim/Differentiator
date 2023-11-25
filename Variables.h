#ifndef VARIABLES_H
#define VARIABLES_H

#include "Errors.h"

const char BANNED_SYMBOLS[] = "!@#$%&;:<>()[]{}";

static const int MAX_SIZE_VAR = 100;
struct Variable {
    char* name;
    int valid;
    double value;
};

static const int DEFAULT_CAPACITY =     1;
static const int MULTIPLIER_CAPACITY =  2;
struct Variables {
    Variable* var;
    int count;
    int capacity;
};

ErrorCode variables_copy(Variables* vars, Variables** new_vars);

#endif // VARIABLES_H