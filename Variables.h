#ifndef VARIABLES_H
#define VARIABLES_H

#include "Errors.h"

const char BANNED_SYMBOLS[] = "!@#$%&;:<>()[]{}";

static const size_t MAX_SIZE_VAR = 100;
struct Variable {
    char* name;
    int valid;
    double value;
};

static const size_t DEFAULT_CAPACITY =     1;
static const size_t MULTIPLIER_CAPACITY =  2;
struct Variables {
    Variable* var;
    size_t count;
    size_t capacity;
};

ErrorCode variables_copy(Variables* vars, Variables** new_vars);

#endif // VARIABLES_H