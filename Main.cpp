#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ANSI_colors.h"
#include "Differentiation.h"
#include "Evaluation.h"
#include "Input.h"
#include "Output.h"
#include "ProcessCmd.h"
#include "Simplification.h"
#include "Equation.h"

int main(int argc, const char *argv[])
{
    printf(print_lblue("# Implementation of Differentiator.\n"
                       "# (c) BAIDUSENOV TIMUR, 2023\n\n"));

    srand(time(NULL));
    ErrorCode err = ERROR_NO;
    CmdInputData cmd_data = {};
    char* file_buffer = nullptr;
    char* start_file_buffer = nullptr;
    double eval_equation = -1;
    Equation* equation = nullptr;
    Equation* equation_simple = nullptr;
    Equation* equation_for_eval = nullptr;
    Equation* equation_for_diff = nullptr;
    Equation* equation_for_diff_simple = nullptr;
    int num_dump_var = 0;
    int num_diff_var = 0;

    err = input_cmd(argc, argv, &cmd_data);
    if (err) goto error;

    if (cmd_data.is_help) {
        err = print_help();
        if (err) goto error;
    }

    if (!cmd_data.is_data_file) {
        err = ERROR_INVALID_FILE;
        goto error;
    }

    err = equation_new(&equation, "MainEquation");
    if (err) goto error;

    err = file_to_buf(cmd_data.name_data_file, &file_buffer);
    if (err) goto error;

    start_file_buffer = file_buffer;
    err = equation_read(equation, &file_buffer);
    if (err) goto error;

    err = prepare_dump_dir(equation);
    if (err) goto error;
    err = equation_get_num_var(equation, &num_dump_var);
    if (err) goto error;
    err = equation_get_val_vars(equation);
    if (err) goto error;
    err = equation_big_dump(equation, num_dump_var);
    if (err) goto error;

    err = equation_copy(equation, "EquationEval", &equation_for_eval);
    if (err) goto error;
    err = equation_get_val_vars(equation_for_eval);
    if (err) goto error;
    err = equation_eval(equation_for_eval, &eval_equation);
    if (err) goto error;
    printf(print_lcyan("eval_equation = %lf\n"), eval_equation);

    err = equation_copy(equation, "EquationDiff", &equation_for_diff);
    if (err) goto error;
    err = equation_get_num_var(equation_for_diff, &num_diff_var);
    if (err) goto error;
    err = prepare_dump_dir(equation_for_diff);
    if (err) goto error;
    err = equation_diff_report(equation_for_diff, num_diff_var);
    if (err) goto error;

    err = equation_get_num_var(equation_for_diff, &num_dump_var);
    if (err) goto error;
    err = equation_get_val_vars(equation_for_diff);
    if (err) goto error;
    err = equation_big_dump(equation_for_diff, num_dump_var);
    if (err) goto error;

    err = equation_copy(equation, "EquationSimple", &equation_simple);
    if (err) goto error;
    err = equation_simplify(equation_simple);
    if (err) goto error;
    err = equation_get_num_var(equation_simple, &num_dump_var);
    if (err) goto error;
    err = equation_get_val_vars(equation_simple);
    if (err) goto error;
    err = prepare_dump_dir(equation_simple);
    if (err) goto error;
    err = equation_big_dump(equation_simple, num_dump_var);
    if (err) goto error;

    err = equation_copy(equation_for_diff, "EquationDiffSimple", &equation_for_diff_simple);
    if (err) goto error;
    err = equation_simplify(equation_for_diff_simple);
    if (err) goto error;
    err = equation_get_num_var(equation_for_diff_simple, &num_dump_var);
    if (err) goto error;
    err = equation_get_val_vars(equation_for_diff_simple);
    if (err) goto error;
    err = prepare_dump_dir(equation_for_diff_simple);
    if (err) goto error;
    err = equation_big_dump(equation_for_diff_simple, num_dump_var);
    if (err) goto error;

    printf(print_lblue("\nBye\n"));

    goto finally;

error:
    err_dump(err);

finally:
    free(start_file_buffer);
    equation_delete(equation);
    equation_delete(equation_simple);
    equation_delete(equation_for_eval);
    equation_delete(equation_for_diff);
    equation_delete(equation_for_diff_simple);
    cmd_data_delete(&cmd_data);

    return err;
}