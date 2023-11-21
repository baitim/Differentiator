#ifndef DIFF_H
#define DIFF_H

#include "Errors.h"
#include "Tree.h"

ErrorCode tree_read     (Tree** tree, const char* buf);
ErrorCode tree_write    (Tree** tree, const char* buf, int dep);
ErrorCode tree_insert   (Tree** tree, Node* node);
ErrorCode tree_init     (Tree** tree);

#endif // DIFF_H