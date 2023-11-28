#ifndef DUMP_H
#define DUMP_H

#include "Errors.h"
#include "Tree.h"

struct OutputInfo {
    int number_graph_dump;
    int number_svg_dump;
    int number_tex_dump;
    int number_html_dump;
};

ErrorCode print_help        ();
ErrorCode prepare_dump_dir  (Tree* tree);
ErrorCode tree_big_dump     (Tree* tree, int num_var);
ErrorCode tree_graph_dump   (Tree* tree, int num_var);
ErrorCode tree_cmd_dump     (Tree* tree);
ErrorCode tree_svg_dump     (Tree* tree);
ErrorCode tree_html_dump    (Tree* tree);
ErrorCode tree_tex_dump     (Tree* tree);

#endif // DUMP_H