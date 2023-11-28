#include <stdio.h>
#include <stdlib.h>

#include "ANSI_colors.h"
#include "Tree.h"
#include "Output.h"
#include "Input.h"
#include "Eval.h"
#include "Diff.h"

int main()
{
    printf(print_lblue("# Implementation of Differentiator.\n"
                       "# (c) BAIDUSENOV TIMUR, 2023\n\n"));

    ErrorCode err = ERROR_NO;
    const char name_data_file[] = "equation2.txt";
    char* file_buffer = nullptr;
    char* start_file_buffer = nullptr;
    double eval_equation = -1;
    Tree* tree = nullptr;
    Tree* tree_for_diff = nullptr;

    err = tree_new(&tree, "MainTree");
    if (err) goto error;

    err = file_to_buf(name_data_file, &file_buffer);
    if (err) goto error;

    start_file_buffer = file_buffer;
    err = tree_read(tree, &file_buffer);
    if (err) goto error;

    prepare_dump_dir(tree);
    err = tree_big_dump(tree);
    if (err) goto error;

    err = tree_eval(tree, &eval_equation);
    if (err) goto error;
    printf(print_lcyan("eval_equation = %lf\n"), eval_equation);

    err = tree_copy(tree, "TreeDiff", &tree_for_diff);
    if (err) goto error;

    err = tree_diff(tree_for_diff);
    if (err) goto error;

    prepare_dump_dir(tree_for_diff);
    err = tree_big_dump(tree_for_diff);
    if (err) goto error;

    printf(print_lblue("\nBye\n"));

    goto finally;

error:
    err_dump(err);

finally:
    free(start_file_buffer);
    tree_delete(tree_for_diff);
    tree_delete(tree);
    return err;
}