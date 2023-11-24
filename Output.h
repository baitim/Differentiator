#ifndef DUMP_H
#define DUMP_H

#include "Errors.h"
#include "Tree.h"

ErrorCode tree_cmd_dump     (Tree* tree);
ErrorCode tree_graph_dump   (Tree* tree, int* number_graph_dump);
ErrorCode tree_html_dump    (int number_graph_dump, int* number_html_dump);
ErrorCode tree_tex_dump     (Tree* tree, int* number_tex_dump);
ErrorCode prepare_dump_dir  ();

#endif // DUMP_H