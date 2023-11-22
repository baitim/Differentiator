#ifndef VARIABLES_H
#define VARIABLES_H

const char banned_chars[] = "!@#$%^&**()-+";

const int MAX_SIZE_VAR = 100;
struct Vars {
    const char *name;
    int index;
};
const int MAX_COUNT_VARS = 1000;

#endif // VARIABLES_H