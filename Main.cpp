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
    const char name_data_file[] = "equation.txt";
    int number_graph_dump = 1;
    int number_tex_dump =   1;
    int number_html_dump =  1;
    char* buf_file = nullptr;
    char* old_buf = nullptr;
    double ans_eval = -1;
    Tree* tree = nullptr;
    int k = 0;

    err = prepare_dump_dir();
    if (err) goto Final_err;

    err = tree_new(&tree);
    if (err) goto Final_err;

    err = file_to_buf(name_data_file, &buf_file);
    if (err) goto Final_err;

    old_buf = buf_file;
    err = tree_read(tree, &buf_file);
    free(old_buf);
    if (err) goto Final_err;

    err = tree_eval(tree, &ans_eval);
    if (err) goto Final_err;
    printf(print_lcyan("ans_eval = %lf\n"), ans_eval);

    err = tree_diff(tree);
    if (err) goto Final_err;

    k = 5;
    while (k-- > 0) {
        err = tree_cmd_dump(tree);
        if (err) goto Final_err;

        err = tree_graph_dump(tree, &number_graph_dump);
        if (err) goto Final_err;

        err = tree_tex_dump(tree, &number_tex_dump);
        if (err) goto Final_err;

        err = tree_html_dump(number_graph_dump, &number_html_dump);
        if (err) goto Final_err;
    }

    err = tree_delete(tree);
    if (err) goto Final_err;

    goto Final_noerr;
Final_err:

    err_dump(err);
    return err;

Final_noerr:

    printf(print_lblue("\nBye\n"));

    return 0;
}