#ifndef DUMP_H
#define DUMP_H

#include <stdio.h>

#include "Errors.h"
#include "Equation.h"

const int MAX_SIZE_NAME_DUMP = 100;
const int MAX_SIZE_COMMAND = 500;
const int MAX_SIZE_TEX_TXT_DATA = 200;

static const char tex_txt_data[][MAX_SIZE_TEX_TXT_DATA] = {
                                "Несложными преобразованиями получаем",
                                "Nobody cares",
                                "Даже советский батон преобразовал бы это",
                                "Договоримся",
                                "Выражение находится в кормушке, а значит",
                                "Преобразования аналогичны, разобранным лекции",
                                "Что вы от меня хотите? Если вы это не понимаете, вы дурак",
                                "Итак",
                                "Значит",
                                "Таким образом",
};
const int COUNT_TEX_TXT_DATA = sizeof(tex_txt_data) / (MAX_SIZE_TEX_TXT_DATA * sizeof(char));

struct OutputInfo {
    size_t number_graph_dump;
    size_t number_svg_dump;
    size_t number_tex_dump;
    size_t number_html_dump;
};

ErrorCode print_help            ();
ErrorCode prepare_dump_dir      (Equation* equation);
ErrorCode equation_big_dump     (Equation* equation, int num_var);
ErrorCode equation_graph_dump   (Equation* equation, int num_var);
ErrorCode equation_cmd_dump     (Equation* equation);
ErrorCode equation_svg_dump     (Equation* equation);
ErrorCode equation_html_dump    (Equation* equation);
ErrorCode equation_tex_dump     (Equation* equation);
ErrorCode make_name_file        (char* buffer, const char *type, char** name_dump_file);
ErrorCode equation_node_dump_   (EquationNode* node, Variables* vars, FILE* dump_file,
                                 Branch branch, EquationDataType par_type, TypeOperator par_op);
#endif // DUMP_H