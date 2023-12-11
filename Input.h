#ifndef INPUT_H
#define INPUT_H

#include "Errors.h"
#include "Equation.h"

static const int MAX_SIZE_INPUT = 500;

struct InputData {
    char* buf;
    int index;
};

ErrorCode equation_get_val_vars (Equation* equation);
ErrorCode equation_get_num_var  (Equation* equation, int* num_var);
ErrorCode equation_read         (Equation* equation, InputData* input_data);
ErrorCode file_to_buf           (const char* name_file, char** buf);
ErrorCode get_var               (Variables* vars, size_t number_var);
ErrorCode input_data_ctor       (InputData** input_data, char* name_input_file);
ErrorCode input_data_dtor       (InputData** input_data);
ErrorCode clean_stdin           ();

#endif // INPUT_H