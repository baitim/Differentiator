#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>

#include "Tree.h"
#include "Errors.h"

ErrorCode tree_read     (Tree* tree, char** buf);
ErrorCode file_to_buf   (const char* name_file, char** buf);

#endif // INPUT_H