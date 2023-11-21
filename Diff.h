#ifndef DIFF_H
#define DIFF_H

#include "Errors.h"
#include "Tree.h"

ErrorCode tree_read     (Tree** tree, char** buf);
ErrorCode tree_write    (Tree** tree, char* buf, int dep);
ErrorCode tree_init     (Tree** tree);

#endif // DIFF_H