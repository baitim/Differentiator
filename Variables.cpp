#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "Variables.h"

ErrorCode variables_copy(Variables* vars, Variables** new_vars)
{
    assert(vars);
    assert(new_vars && *new_vars);

    free((*new_vars)->var);
    free(*new_vars);
    *new_vars = (Variables*)calloc(1, sizeof(Variables));
    if (!(*new_vars)) return ERROR_ALLOC_FAIL;

    (*new_vars)->count =    vars->count;
    (*new_vars)->capacity = vars->capacity;

    (*new_vars)->var = (Variable*)calloc(vars->capacity, sizeof(Variable));
    if (!(*new_vars)->var) return ERROR_ALLOC_FAIL;

    for (int i = 0; i < vars->count; i++) {
        (*new_vars)->var[i].name = strdup(vars->var[i].name);
        if (!(*new_vars)->var[i].name) return ERROR_ALLOC_FAIL;

        (*new_vars)->var[i].valid = vars->var[i].valid;
        (*new_vars)->var[i].value = vars->var[i].value;
    }

    return ERROR_NO;
}