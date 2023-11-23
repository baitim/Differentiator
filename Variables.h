#ifndef VARIABLES_H
#define VARIABLES_H

const char banned_symbols[] = "!@#$%&;:<>()[]{}";

static const int MAX_SIZE_VAR = 100;
struct Variable {
    const char *name;
    int index;
};

static const int MAX_COUNT_VARS = 1000;
static const int DEFAULT_COUNT_VARIABLES = 500;
struct Variables {
    char** names;
    int *valid;
    double *value;
    int count;
};

#endif // VARIABLES_H