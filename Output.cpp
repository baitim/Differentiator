#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "ANSI_colors.h"
#include "Evaluation.h"
#include "Math.h"
#include "Node.h"
#include "Output.h"
#include "ProcessCmd.h"

static ErrorCode equation_write_points          (Equation* equation, int num_var, EvalPoints* graph, FILE* dump_file);
static ErrorCode equation_cmd_dump_             (EquationNode* node, Variables* vars, int dep);
static ErrorCode equation_svg_dump_make_node    (EquationNode* node, Variables* vars, FILE* dump_file);
static ErrorCode equation_svg_dump_make_edge    (EquationNode* node, FILE* dump_file);
static ErrorCode equation_tex_dump_             (EquationNode* node, Variables* vars, FILE* dump_file);
static ErrorCode write_left_parenthesis         (EquationNode* node, FILE* dump_file, Branch branch,
                                                 EquationDataType par_type, TypeOperator par_op);
static ErrorCode write_right_parenthesis        (EquationNode* node, FILE* dump_file,
                                                 EquationDataType par_type, TypeOperator par_op);

ErrorCode print_help()
{
    for (int i = 0; i < COUNT_OPTIONS; i++) {
        int count_print = 0;
        printf(print_lgreen("%s%n"), OPTIONS[i].name, &count_print);
        for (int j = 0; j <= MAX_CMD_COMMAND_SIZE - count_print; j++)
            printf(" ");
        printf(print_lgreen("%s\n"), OPTIONS[i].description);
    }
    return ERROR_NO;
}

ErrorCode prepare_dump_dir(Equation* equation)
{
    char command[MAX_SIZE_COMMAND] = "";
    snprintf(command, MAX_SIZE_COMMAND, "mkdir %s", equation->name);
    int sys = system(command);

    snprintf(command, MAX_SIZE_COMMAND, "rm -r %s/*", equation->name);
    sys = system(command);

    snprintf(command, MAX_SIZE_COMMAND, "mkdir %s/py ; "
                                        "mkdir %s/graph ; "
                                        "mkdir %s/dot ; "
                                        "mkdir %s/svg ; "
                                        "mkdir %s/html ;"
                                        "mkdir %s/tex ;"
                                        "mkdir %s/pdf ;",
                                        equation->name, equation->name, equation->name, 
                                        equation->name, equation->name, equation->name,
                                        equation->name);
    sys = system(command);
    if (sys) return ERROR_SYSTEM_COMMAND;

    return ERROR_NO;
}

ErrorCode equation_big_dump(Equation* equation, int num_var)
{
    if (!equation) return ERROR_INVALID_TREE;

    ErrorCode err = ERROR_NO;

    err = equation_cmd_dump(equation);
    if (err) return err;

    err = equation_svg_dump(equation);
    if (err) return err;

    err = equation_tex_dump(equation);
    if (err) return err;

    err = equation_graph_dump(equation, num_var);
    if (err) return err;

    err = equation_html_dump(equation);
    if (err) return err;

    return ERROR_NO;
}

ErrorCode equation_graph_dump(Equation* equation, int num_var)
{
    if (!equation) return ERROR_INVALID_TREE;

    ErrorCode err = equation_verify(equation->root);
    if (err) return err;

    char py_path[MAX_SIZE_NAME_DUMP] = "";
    snprintf(py_path, MAX_SIZE_NAME_DUMP, "%s/py/%s%zu", equation->name, equation->name, 
                                           equation->output_info->number_graph_dump);

    char *name_py_file = nullptr;
    err = make_name_file(py_path, ".py", &name_py_file);
    if (err) return err;
    FILE* dump_file = fopen(name_py_file, "w");
    if (!dump_file) {
        fprintf(stderr, "Error open file to dump\n");
        return ERROR_SYSTEM_COMMAND;
    }

    char graph_path[MAX_SIZE_NAME_DUMP] = "";
    snprintf(graph_path, MAX_SIZE_NAME_DUMP, "%s/graph/%s%zu", equation->name, equation->name, 
                                              equation->output_info->number_graph_dump);

    EvalPoints graph = {-5, 20, 20, -5, 0.05f};

    fprintf(dump_file,  "import matplotlib.pyplot as plt\n");

    err = equation_write_points(equation, num_var, &graph, dump_file);
    if (err) return err;

    fprintf(dump_file, "plt.plot(x, y)\n");
    fprintf(dump_file, "plt.xlim(%lf, %lf)\n"
                       "plt.ylim(%lf, %lf)\n",
                        graph.left_border, graph.right_border,
                        graph.min_value,  graph.max_value);

    char *name_graph_file = nullptr;
    err = make_name_file(graph_path, ".svg", &name_graph_file);
    if (err) return err;
    fprintf(dump_file, "plt.savefig('%s', dpi=300)\n", name_graph_file);
    fclose(dump_file);

    char command[MAX_SIZE_COMMAND] = "";
    snprintf(command, MAX_SIZE_COMMAND, "python3 %s", name_py_file);
    int sys = system(command);
    if (sys) return ERROR_SYSTEM_COMMAND;

    free(name_py_file);
    free(name_graph_file);
    equation->output_info->number_graph_dump++;
    return equation_verify(equation->root);
}

static ErrorCode equation_write_points(Equation* equation, int num_var, EvalPoints* graph, FILE* dump_file)
{
    if (!equation) return ERROR_INVALID_TREE;

    ErrorCode err = equation_verify(equation->root);
    if (err) return err;

    if (graph->right_border < graph->left_border) return equation_verify(equation->root);

    size_t size = (size_t)((graph->right_border - graph->left_border) / graph->step_values) + 1;

    double* x = (double*)calloc(size, sizeof(double));
    if (!x) return ERROR_ALLOC_FAIL;
    double* y = (double*)calloc(size, sizeof(double));
    if (!y) return ERROR_ALLOC_FAIL;

    if (equation->variables->count > 0) {
        err = equation_get_points(equation, num_var, graph, x, y);
        if (err) return err;
    }

    fprintf(dump_file, "x = [");
    for (size_t i = 0; i < size - 1; i++)
        if (!is_double_equal(graph->max_value * 2, y[i]))
            fprintf(dump_file, "%lf, ", x[i]);
    if (!is_double_equal(graph->max_value * 2, y[size - 1]))
        fprintf(dump_file, "%lf", x[size - 1]);
    fprintf(dump_file, "]\n");

    fprintf(dump_file, "y = [");
    for (size_t i = 0; i < size - 1; i++)
        if (!is_double_equal(graph->max_value * 2, y[i]))
            fprintf(dump_file, "%lf, ", y[i]);
    if (!is_double_equal(graph->max_value * 2, y[size - 1]))
        fprintf(dump_file, "%lf", y[size - 1]);
    fprintf(dump_file, "]\n");

    free(x);
    free(y);
    return equation_verify(equation->root);
}

ErrorCode equation_cmd_dump(Equation* equation)
{
    if (!equation) return ERROR_INVALID_TREE;

    ErrorCode err = equation_verify(equation->root);
    if (err) return err;

    fprintf(stderr, print_lmagenta("^^^^^^^^^^^^^^^^^^^^\n\n"));
    fprintf(stderr, print_lmagenta("Equation: %s\n"), equation->name);

    equation_cmd_dump_(equation->root, equation->variables, 0);

    fprintf(stderr, print_lmagenta("vvvvvvvvvvvvvvvvvvvv\n\n"));

    return equation_verify(equation->root);
}

static ErrorCode equation_cmd_dump_(EquationNode* node, Variables* vars, int dep)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = equation_verify(node);
    if (err) return err;

    if (node->left) equation_cmd_dump_(node->left, vars, dep + 1);

    for (int i = 0; i < dep; i++) fprintf(stderr, "\t");
    if (node->type_value == TYPE_NUM) {
        fprintf(stderr, print_green("%.2lf\n"), node->value);
    } else if (node->type_value == TYPE_OP) {
        for (int i = 0; i < COUNT_OPs; i++) {
            if (is_double_equal(OPERATORS[i].type_op, node->value))
                fprintf(stderr, print_lyellow("%s\n"), OPERATORS[i].name);
        }
    } else if (node->type_value == TYPE_VAR) {
        if (vars->var[(int)node->value].valid)
            fprintf(stderr, print_lgreen("%s = %.2lf\n"), vars->var[(int)node->value].name, 
                                          vars->var[(int)node->value].value);
        else 
            fprintf(stderr, print_lgreen("%s\n"), vars->var[(int)node->value].name);
    }

    if (node->right) equation_cmd_dump_(node->right, vars, dep + 1);

    return equation_verify(node);
}

ErrorCode equation_svg_dump(Equation* equation)
{
    if (!equation) return ERROR_INVALID_TREE;

    ErrorCode err = equation_verify(equation->root);
    if (err) return err;

    char dot_path[MAX_SIZE_NAME_DUMP] = "";
    snprintf(dot_path, MAX_SIZE_NAME_DUMP, "%s/dot/%s%zu", equation->name, equation->name, 
                                            equation->output_info->number_svg_dump);

    char *name_dot_file = nullptr;
    err = make_name_file(dot_path, ".dot", &name_dot_file);
    if (err) return err;
    FILE* dump_file = fopen(name_dot_file, "w");
    if (!dump_file) {
        fprintf(stderr, "Error open file to dump\n");
        return ERROR_SYSTEM_COMMAND;
    }

    fprintf(dump_file, "digraph {\n"
                       "\tgraph[label = \"%s\", labelloc = top, "
                       "labeljust = center, fontsize = 70, fontcolor = \"#e33e19\"];\n"
                       "\tgraph[dpi = 100];\n"
                       "\tbgcolor = \"#2F353B\";\n"
                       "\tedge[minlen = 3.5, arrowsize = 2.5, penwidth = 4];\n"
                       "\tnode[shape = \"rectangle\", style = \"rounded, filled\", height = 3, width = 2, "
                       "fillcolor = \"#ab5b0f\", width = 3, fontsize = 30, penwidth = 3.5, color = \"#941b1b\"]\n",
                        equation->name);

    err = equation_svg_dump_make_node(equation->root, equation->variables, dump_file);
    if (err) return err;

    err = equation_svg_dump_make_edge(equation->root, dump_file);
    if (err) return err;

    fprintf(dump_file, "}\n");
    fclose(dump_file);

    char svg_path[MAX_SIZE_NAME_DUMP] = "";
    snprintf(svg_path, MAX_SIZE_NAME_DUMP, "%s/svg/%s%zu", equation->name, equation->name, 
                                            equation->output_info->number_svg_dump);

    char *name_svg_file = nullptr;
    err = make_name_file(svg_path, ".svg", &name_svg_file);
    if (err) return err;
    char command[MAX_SIZE_COMMAND] = "";
    snprintf(command, MAX_SIZE_COMMAND, "gvpack -u %s | dot -Tsvg -o %s", 
                                         name_dot_file, name_svg_file);
    int sys = system(command);
    if (sys) return ERROR_SYSTEM_COMMAND;

    equation->output_info->number_svg_dump++;
    free(name_dot_file);
    free(name_svg_file);
    return equation_verify(equation->root);
}

static ErrorCode equation_svg_dump_make_node(EquationNode* node, Variables* vars, FILE* dump_file)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = equation_verify(node);
    if (err) return err;

    if (node->left) equation_svg_dump_make_node(node->left, vars, dump_file);

    fprintf(dump_file, "\t{ \n"
                       "\t\tnode[shape = \"Mrecord\"];\n"
                       "\t\tnode%p[label = \"{ ", node);
    if (node->type_value == TYPE_NUM) {
        fprintf(dump_file, "%.2lf", node->value);
        fprintf(dump_file, " | dep = %zu }\", fillcolor = \"#ab5b0f\"];\n", node->depth);
    } else if (node->type_value == TYPE_OP) {
        for (int i = 0; i < COUNT_OPs; i++) {
            if (is_double_equal(OPERATORS[i].type_op, node->value)) {
                fprintf(dump_file, "%s", OPERATORS[i].name);
                fprintf(dump_file, " | dep = %zu }\", fillcolor = \"#e3964d\"];\n", node->depth);
            }
        }
    } else if (node->type_value == TYPE_VAR) {
        if (vars->var[(int)node->value].valid)
            fprintf(dump_file, "%s = %.2lf", vars->var[(int)node->value].name, 
                                vars->var[(int)node->value].value);
        else 
            fprintf(dump_file, "%s", vars->var[(int)node->value].name);
        fprintf(dump_file, " | dep = %zu }\", fillcolor = \"#f79e19\"];\n", node->depth);
    }
    fprintf(dump_file, "\t}\n");

    if (node->right) equation_svg_dump_make_node(node->right, vars, dump_file);

    return equation_verify(node);
}

static ErrorCode equation_svg_dump_make_edge(EquationNode* node, FILE* dump_file)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = equation_verify(node);
    if (err) return err;

    if (node->left)  equation_svg_dump_make_edge(node->left, dump_file);
    if (node->left)  fprintf(dump_file, "\tnode%p->node%p[color = yellow, labelangle = 45];\n", node, node->left);
    if (node->right) fprintf(dump_file, "\tnode%p->node%p[color = yellow, labelangle = 45];\n", node, node->right);
    if (node->right) equation_svg_dump_make_edge(node->right, dump_file);

    if (node->parent) fprintf(dump_file, "\tnode%p->node%p[color = \"#b6ff1a\", labelangle = 45];\n", node, node->parent);

    return equation_verify(node);
}

ErrorCode equation_html_dump(Equation* equation)
{
    if (!equation) return ERROR_INVALID_TREE;

    ErrorCode err = equation_verify(equation->root);
    if (err) return err;

    char buffer[MAX_SIZE_NAME_DUMP] = "";
    snprintf(buffer, MAX_SIZE_NAME_DUMP, "%s/html/%s%zu.html", equation->name, equation->name, 
                                          equation->output_info->number_html_dump);

    FILE* html_file = fopen(buffer, "w");
    if (!html_file) {
        fprintf(stderr, "Error open file to dump\n");
        return ERROR_OPEN_FILE;
    }

    fprintf(html_file, "<pre>\n");

    for (size_t i = 1; i < equation->output_info->number_svg_dump; i++) {
        fprintf(html_file, "<img src = \"../svg/%s%zu.svg\">\n", equation->name, i);
    }

    fprintf(html_file, "</pre>\n");

    equation->output_info->number_html_dump++;

    fclose(html_file);
    return equation_verify(equation->root);
}

ErrorCode equation_tex_dump(Equation* equation)
{
    if (!equation) return ERROR_INVALID_TREE;

    ErrorCode err = equation_verify(equation->root);
    if (err) return err;

    char tex_path[MAX_SIZE_NAME_DUMP] = "";
    snprintf(tex_path, MAX_SIZE_NAME_DUMP, "%s/tex/%s%zu", equation->name, equation->name, 
                                            equation->output_info->number_tex_dump);

    char *name_tex_file = nullptr;
    err = make_name_file(tex_path, ".tex", &name_tex_file);
    if (err) return err;

    FILE* dump_file = fopen(name_tex_file, "w");
    if (!dump_file) {
        fprintf(stderr, "Error open file to dump\n");
        return ERROR_SYSTEM_COMMAND;
    }

    err = equation_verify(equation->root);
    if (err) return err;

    time_t mytime = time(NULL);
    struct tm *now = localtime(&mytime);

    fprintf(dump_file, "\\documentclass[a4paper,3pt]{article}\n"
                       "\\usepackage[T2A]{fontenc}\n\n"
                       "\\usepackage{amsmath}\n"
                       "\\DeclareMathOperator\\arcctan{arcctan}"
                       "\\author{Baidiusenov Timur}\n"
                       "\\title{%s}\n"
                       "\\date{Date: %d.%d.%d, Time %d:%d:%d}\n"
                       "\\begin{document}\n"
                       "\\maketitle\n",
                       equation->name,
                       now->tm_mday, now->tm_mon + 1, now->tm_year + 1900,
                       now->tm_hour, now->tm_min, now->tm_sec);

    err = equation_tex_dump_(equation->root, equation->variables, dump_file);
    if (err) return err;

    fprintf(dump_file, "\\end{document}\n");
    fclose(dump_file);

    char pdf_path[MAX_SIZE_NAME_DUMP] = "";
    snprintf(pdf_path, MAX_SIZE_NAME_DUMP, "%s/pdf/%s%zu", equation->name, equation->name, 
                                            equation->output_info->number_tex_dump);

    char *name_aux_file = nullptr;
    err = make_name_file(pdf_path, ".aux", &name_aux_file);
    if (err) return err;
    char *name_tex_log_file = nullptr;
    err = make_name_file(pdf_path, ".log", &name_tex_log_file);
    if (err) return err;

    char command[MAX_SIZE_COMMAND] = "";
    snprintf(command, MAX_SIZE_COMMAND, "pdflatex -output-directory=%s/pdf %s ; "
                                        "rm %s ; rm %s", 
                                        equation->name, name_tex_file, 
                                        name_aux_file, name_tex_log_file);
    int sys = system(command);
    if (sys) return ERROR_SYSTEM_COMMAND;

    equation->output_info->number_tex_dump++;
    free(name_tex_file);
    free(name_aux_file);
    free(name_tex_log_file);
    return equation_verify(equation->root);
}

static ErrorCode equation_tex_dump_(EquationNode* node, Variables* vars, FILE* dump_file)
{
    assert(node);
    assert(vars);
    assert(dump_file);

    ErrorCode err = ERROR_NO;

    if (node->left) err =  equation_tex_dump_(node->left,  vars, dump_file);
    if (err) return err;
    if (node->right) err = equation_tex_dump_(node->right, vars, dump_file);
    if (err) return err;

    fprintf(dump_file, "%s:\n", tex_txt_data[rand() % COUNT_TEX_TXT_DATA]);

    fprintf(dump_file, "\\begin{equation}\n\t");
    err = equation_node_dump_(node, vars, dump_file, BRANCH_ERR, TYPE_ERR, OP_ERR);
    if (err) return err;
    fprintf(dump_file, "\n\\end{equation}\n");

    return ERROR_NO;
}

ErrorCode equation_node_dump_(EquationNode* node, Variables* vars, FILE* dump_file,
                              Branch branch, EquationDataType par_type, TypeOperator par_op)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = equation_verify(node);
    if (err) return err;

    TypeOperator node_op = OP_ERR;
    if (node->type_value == TYPE_OP)
        node_op = (TypeOperator)(int)node->value;

    err = write_left_parenthesis(node, dump_file, branch, 
                                 par_type, par_op);
    if (err) return err;

    if (node->left) equation_node_dump_(node->left, vars, dump_file, BRANCH_LEFT, 
                                        node->type_value, node_op);

    if (node->type_value == TYPE_NUM) {
        if (is_double_less(node->value, 0))
            fprintf(dump_file, "(%.2lf)", node->value);
        else
            fprintf(dump_file, "%.2lf", node->value);
    } else if (node->type_value == TYPE_OP) {
        switch ((int)node->value) {
            case (OP_ADD) : fprintf(dump_file, " + ");      break;
            case (OP_SUB) : fprintf(dump_file, " - ");      break;
            case (OP_MUL) : fprintf(dump_file, " \\cdot "); break;
            case (OP_DIV) :                                 break;
            case (OP_POW) : fprintf(dump_file, "^");        break;
            case (OP_LOG) :  case (OP_LN) :  case (OP_SQRT) : case (OP_SIN) :  case (OP_COS) :
            case (OP_TG) :   case (OP_CTG) : case (OP_ASIN) : case (OP_ACOS) : case (OP_ATG) : 
            case (OP_ACTG) : case (OP_SH) :  case (OP_CH) :   case (OP_TH) :   case (OP_CTH) : 
                break;
            default :
                assert(0);
    }
    } else if (node->type_value == TYPE_VAR) {
        fprintf(dump_file, "%s", vars->var[(int)node->value].name);
    }

    if (node->right) equation_node_dump_(node->right, vars, dump_file, BRANCH_RIGHT, 
                                         node->type_value, node_op);

    err = write_right_parenthesis(node, dump_file, 
                                  par_type, par_op);
    if (err) return err;

    return equation_verify(node);
}

static ErrorCode write_left_parenthesis(EquationNode* node, FILE* dump_file, Branch branch,
                                        EquationDataType par_type, TypeOperator par_op)
{
    assert(node);
    assert(dump_file);

    if (par_type == TYPE_OP) {
        if (par_op == OP_DIV && branch == BRANCH_LEFT)  fprintf(dump_file, "\\frac{");
        if (par_op == OP_DIV && branch == BRANCH_RIGHT) fprintf(dump_file, "{");
        if (par_op == OP_POW) fprintf(dump_file, "{");
        if (par_op == OP_LOG && branch == BRANCH_LEFT)  fprintf(dump_file, "\\log_{");
        if (par_op == OP_LOG && branch == BRANCH_RIGHT) fprintf(dump_file, "{");
        if (par_op == OP_LN)   fprintf(dump_file, "\\ln{");
        if (par_op == OP_SQRT) fprintf(dump_file, "\\sqrt{");
        if (par_op == OP_SIN)  fprintf(dump_file, "\\sin{");
        if (par_op == OP_COS)  fprintf(dump_file, "\\cos{");
        if (par_op == OP_TG)   fprintf(dump_file, "\\tan{");
        if (par_op == OP_CTG)  fprintf(dump_file, "\\cot{");
        if (par_op == OP_ASIN) fprintf(dump_file, "\\arcsin{");
        if (par_op == OP_ACOS) fprintf(dump_file, "\\arccos{");
        if (par_op == OP_ATG)  fprintf(dump_file, "\\arctan{");
        if (par_op == OP_ACTG) fprintf(dump_file, "\\arcctan{");
        if (par_op == OP_SH)   fprintf(dump_file, "\\sinh{");
        if (par_op == OP_CH)   fprintf(dump_file, "\\cosh{");
        if (par_op == OP_TH)   fprintf(dump_file, "\\tanh{");
        if (par_op == OP_CTH)  fprintf(dump_file, "\\coth{");

        if ((par_op == OP_MUL || par_op == OP_POW  || par_op == OP_LOG || 
            par_op == OP_LN   || par_op == OP_SQRT || par_op == OP_SIN || 
            par_op == OP_COS  || par_op == OP_TG   || par_op == OP_CTG ||
            par_op == OP_ASIN || par_op == OP_ACOS || par_op == OP_ATG ||
            par_op == OP_ACTG || par_op == OP_SH   || par_op == OP_CH  ||
            par_op == OP_TH   || par_op == OP_CTH  || par_op == OP_SUB)
            && node->depth > 1)
            fprintf(dump_file, "(");
    }

    return ERROR_NO;
}

static ErrorCode write_right_parenthesis(EquationNode* node, FILE* dump_file,
                                         EquationDataType par_type, TypeOperator par_op)
{
    assert(node);
    assert(dump_file);
    
    if (par_type == TYPE_OP) {
        if ((par_op == OP_MUL || par_op == OP_POW  || par_op == OP_LOG || 
            par_op == OP_LN   || par_op == OP_SQRT || par_op == OP_SIN || 
            par_op == OP_COS  || par_op == OP_TG   || par_op == OP_CTG ||
            par_op == OP_ASIN || par_op == OP_ACOS || par_op == OP_ATG ||
            par_op == OP_ACTG || par_op == OP_SH   || par_op == OP_CH  ||
            par_op == OP_TH   || par_op == OP_CTH  || par_op == OP_SUB)
            && node->depth > 1)
            fprintf(dump_file, ")");

        if (par_op == OP_DIV  || par_op == OP_POW  || par_op == OP_LOG || 
            par_op == OP_LN   || par_op == OP_SQRT || par_op == OP_SIN || 
            par_op == OP_COS  || par_op == OP_TG   || par_op == OP_CTG ||
            par_op == OP_ASIN || par_op == OP_ACOS || par_op == OP_ATG ||
            par_op == OP_ACTG || par_op == OP_SH   || par_op == OP_CH  ||
            par_op == OP_TH   || par_op == OP_CTH)
            fprintf(dump_file, "}");
    }

    return ERROR_NO;
}

ErrorCode make_name_file(char* buffer, const char *type, char** name_dump_file)
{
    size_t len_buf = strlen(buffer);
    size_t len_type = strlen(type);
    size_t size_dump_file = len_buf + len_type + 1;
    *name_dump_file = (char*)calloc(size_dump_file, sizeof(char));
    (*name_dump_file) = (char*)memcpy((*name_dump_file), buffer, len_buf * sizeof(char) + 1);
    if (!(*name_dump_file)) return ERROR_ALLOC_FAIL;
    (*name_dump_file) = strcat((*name_dump_file), type);
    if (!(*name_dump_file)) return ERROR_ALLOC_FAIL;

    return ERROR_NO;
}