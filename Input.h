#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>

#include "Tree.h"
#include "Errors.h"

static const int MAX_SIZE_INPUT = 500;

ErrorCode tree_read     (Tree* tree, char** buf);
ErrorCode file_to_buf   (const char* name_file, char** buf);
ErrorCode get_var       (Variables* vars, int number_var);
ErrorCode clean_stdin   ();

#endif // INPUT_H