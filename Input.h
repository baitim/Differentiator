#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>

#include "Errors.h"

ErrorCode file_to_buf(const char* name_file, char** buf);

#endif // INPUT_H