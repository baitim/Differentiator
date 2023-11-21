#include <stdio.h>

#include "ANSI_colors.h"
#include "Errors.h"

static void print_error(int error, const char* s);
static int powf(int x, int st);

void err_dump_(int err, const char* file, const char* func, int line)
{
    if (err == ERROR_NO) {
        fprintf(stderr, print_lgreen("dump: OK\n"));
        return;
    }

    fprintf(stderr, print_lred("ERROR: called from FILE = %s, FUNCTION = %s, LINE = %d\n"), 
                               file, func, line);

    for (int i = 1; i < COUNT_ERRORS; i++) {
        if (err & powf(2, i - 1))
            print_error(err, ERRORS[i].description);
    }
}

static void print_error(int error, const char* s)
{
    fprintf(stderr, print_lred("ERROR: %d %s\n"), error, s);
}

static int powf(int x, int st)
{
    if (st == 0) return 1;
    if (st % 2 == 1) return x*  powf(x, st - 1);
    int z = powf(x, st / 2);
    return z * z;
}