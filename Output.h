#ifndef DUMP_H
#define DUMP_H

#include "Errors.h"
#include "Equation.h"

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

#endif // DUMP_H