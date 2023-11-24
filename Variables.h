#ifndef VARIABLES_H
#define VARIABLES_H

const char BANNED_SYMBOLS[] = "!@#$%&;:<>()[]{}";

static const int MAX_SIZE_VAR = 100;
struct Variable {
    const char *name;
    int index;
};

static const int DEFAULT_CAPACITY =     1;
static const int MULTIPLIER_CAPACITY =  2;
struct Variables {
    char** names;
    int *valid;
    double *value;
    int count;
    int capacity;
};

#endif // VARIABLES_H