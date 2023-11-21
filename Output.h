#ifndef DUMP_H
#define DUMP_H

#include "Errors.h"
#include "Tree.h"

ErrorCode tree_cmd_dump     (Tree* tree);
ErrorCode tree_graph_dump   (Tree* tree, int* number_graph_dump);
ErrorCode tree_html_dump    (int number_graph_dump);
ErrorCode buf_to_file       (const char* name_file, char** buf);

#endif // DUMP_H