#ifndef DUMP_H
#define DUMP_H

#include "Errors.h"
#include "Tree.h"

struct OutputInfo {
    int number_graph_dump;
    int number_png_dump;
    int number_tex_dump;
    int number_html_dump;
};

ErrorCode prepare_dump_dir  (Tree* tree);
ErrorCode tree_big_dump     (Tree* tree);
ErrorCode tree_graph_dump   (Tree* tree);
ErrorCode tree_cmd_dump     (Tree* tree);
ErrorCode tree_png_dump     (Tree* tree);
ErrorCode tree_html_dump    (Tree* tree);
ErrorCode tree_tex_dump     (Tree* tree);

#endif // DUMP_H