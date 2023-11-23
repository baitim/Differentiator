#include <stdio.h>

#include "ANSI_colors.h"
#include "Errors.h"

static void print_error(int error, const char* s);

void err_dump_(int err, const char* file, const char* func, int line)
{
    if (err == ERROR_NO) {
        fprintf(stderr, print_lgreen("dump: OK\n"));
        return;
    }

    fprintf(stderr, print_lred("ERROR: called from FILE = %s, FUNCTION = %s, LINE = %d\n"), 
                               file, func, line);

    int pow = 0;
    for (int i = 0; i < COUNT_ERRORS; i++) {
        if (err & pow)
            print_error(err, ERRORS[i].description);
        if (!pow)   pow++;
        else        pow *= 2;
    }
}

static void print_error(int error, const char* s)
{
    fprintf(stderr, print_lred("ERROR: %d %s\n"), error, s);
}