#include <stdio.h>
#include <stdlib.h>

#include "ANSI_colors.h"
#include "Tree.h"
#include "Output.h"
#include "Input.h"
#include "Evaluation.h"
#include "Differentiation.h"
#include "Simplification.h"
#include "ProcessCmd.h"

int main(int argc, const char *argv[])
{
    printf(print_lblue("# Implementation of Differentiator.\n"
                       "# (c) BAIDUSENOV TIMUR, 2023\n\n"));

    ErrorCode err = ERROR_NO;
    CmdInputData cmd_data = {};
    char* file_buffer = nullptr;
    char* start_file_buffer = nullptr;
    double eval_equation = -1;
    Tree* tree = nullptr;
    Tree* tree_for_eval = nullptr;
    Tree* tree_for_diff = nullptr;
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

    err = tree_new(&tree, "MainTree");
    if (err) goto error;

    err = file_to_buf(cmd_data.name_data_file, &file_buffer);
    if (err) goto error;

    start_file_buffer = file_buffer;
    err = tree_read(tree, &file_buffer);
    if (err) goto error;

    err = prepare_dump_dir(tree);
    if (err) goto error;
    err = tree_get_num_var(tree, &num_dump_var);
    if (err) return err;
    err = tree_get_val_vars(tree);
    if (err) goto error;
    err = tree_big_dump(tree, num_dump_var);
    if (err) goto error;

    err = tree_copy(tree, "TreeEval", &tree_for_eval);
    if (err) return err;
    err = tree_get_val_vars(tree_for_eval);
    if (err) goto error;
    err = tree_eval(tree_for_eval, &eval_equation);
    if (err) goto error;
    printf(print_lcyan("eval_equation = %lf\n"), eval_equation);
    err = tree_delete(tree_for_eval);
    if (err) return err;

    err = tree_copy(tree, "TreeDiff", &tree_for_diff);
    if (err) goto error;
    err = tree_get_num_var(tree_for_diff, &num_diff_var);
    if (err) return err;
    err = tree_diff(tree_for_diff, num_diff_var);
    if (err) goto error;

    err = prepare_dump_dir(tree_for_diff);
    if (err) goto error;
    err = tree_get_num_var(tree_for_diff, &num_dump_var);
    if (err) return err;
    err = tree_get_val_vars(tree_for_diff);
    if (err) goto error;
    err = tree_big_dump(tree_for_diff, num_dump_var);
    if (err) goto error;

    err = tree_simplify(tree_for_diff);
    if (err) return err;
    err = tree_big_dump(tree_for_diff, num_dump_var);
    if (err) goto error;

    printf(print_lblue("\nBye\n"));

    goto finally;

error:
    err_dump(err);

finally:
    free(start_file_buffer);
    tree_delete(tree);
    tree_delete(tree_for_diff);
    cmd_data_delete(&cmd_data);

    return err;
}