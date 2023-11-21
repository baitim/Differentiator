#include <stdio.h>
#include <stdlib.h>

#include "ANSI_colors.h"
#include "Tree.h"
#include "Output.h"
#include "Input.h"
#include "Diff.h"

int main()
{
    printf(print_lblue("# Implementation of Tree.\n"
                       "# (c) BAIDUSENOV TIMUR, 2023\n\n"));
    
    ErrorCode err = ERROR_NO;
    int number_graph_dump = 1;
    Tree* tree = nullptr;
    const char name_data_file[] = "equation.txt";

    char* buf_file = nullptr;
    err = file_to_buf(name_data_file, &buf_file);
    if (err) {
        err_dump(err);
        return err;
    }
    char* old_buf = buf_file;
    err = tree_read(&tree, &buf_file);
    if (err) {
        err_dump(err);
        return err;
    }
    free(old_buf);

    double ans_eval = -1;
    err = tree_eval(tree, &ans_eval);
    if (err) {
        err_dump(err);
        return err;
    }
    printf("ans_eval = %lf\n", ans_eval);

    err = tree_cmd_dump(tree);
    if (err) {
        err_dump(err);
        return err;
    }

    err = tree_graph_dump(tree, &number_graph_dump);
    if (err) {
        err_dump(err);
        return err;
    }

    err = tree_delete(tree);
    if (err) {
        err_dump(err);
        return err;
    }

    err = tree_html_dump(number_graph_dump);
    if (err) {
        err_dump(err);
        return err;
    }

    printf(print_lblue("\nBye\n"));

    return 0;
}