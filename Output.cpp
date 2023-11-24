#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <math.h>
#include <time.h>

#include "ANSI_colors.h"
#include "Output.h"
#include "Node.h"

enum Branch {
    BRANCH_ERR =   0,
    BRANCH_LEFT = -1,
    BRANCH_RIGHT = 1,
};

const char dump_dir[] =  "dumps";
const char dump_name[] = "dump";
const int MAX_SIZE_NAME_DUMP = 100;
const int MAX_SIZE_COMMAND = 500;
const double EPSILON = 1e-9;

static ErrorCode tree_cmd_dump_             (Node* node, Variables* vars, int dep);
static ErrorCode tree_graph_dump_make_node  (Node* node, Variables* vars, FILE* dump_file);
static ErrorCode tree_graph_dump_make_edge  (Node* node, FILE* dump_file);
static ErrorCode tree_tex_dump_             (Node* node, Variables* vars, FILE* dump_file);
static ErrorCode tree_equation_dump         (Node* node, Variables* vars, FILE* dump_file,
                                             Branch branch, TypeData par_type, TypeOP par_op);
static ErrorCode write_left_parenthesis(Node* node, FILE* dump_file, Branch branch,
                                        TypeData par_type, TypeOP par_op);
static ErrorCode write_right_parenthesis(Node* node, FILE* dump_file,
                                         TypeData par_type, TypeOP par_op);
static ErrorCode make_name_file             (char* buffer, const char *type, char** name_dump_file);
static int is_double_equal                  (double x, double y);

ErrorCode prepare_dump_dir()
{
    char command[MAX_SIZE_COMMAND] = "";
    snprintf(command, MAX_SIZE_COMMAND, "rm -r %s/*", dump_dir);
    int sys = system(command);
    if (sys) return ERROR_SYSTEM_COMMAND;

    snprintf(command, MAX_SIZE_COMMAND, "mkdir %s/dot ; "
                                        "mkdir %s/png ; "
                                        "mkdir %s/html ;"
                                        "mkdir %s/tex ;"
                                        "mkdir %s/pdf ;",
                                        dump_dir, dump_dir, dump_dir, dump_dir, dump_dir);
    sys = system(command);
    if (sys) return ERROR_SYSTEM_COMMAND;

    return ERROR_NO;
}

ErrorCode tree_cmd_dump(Tree* tree)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(tree->root);
    if (err) return err;

    printf(print_lmagenta("^^^^^^^^^^^^^^^^^^^^\n\n"));
    printf(print_lmagenta("Tree:\n"));

    tree_cmd_dump_(tree->root, tree->variables, 0);

    printf(print_lmagenta("vvvvvvvvvvvvvvvvvvvv\n\n"));

    return tree_verify(tree->root);
}

static ErrorCode tree_cmd_dump_(Node* node, Variables* vars, int dep)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(node);
    if (err) return err;

    if (node->left) tree_cmd_dump_(node->left, vars, dep + 1);

    for (int i = 0; i < dep; i++) printf("\t");
    if (node->type_value == TYPE_NUM) {
        printf(print_green("%.2lf\n"), node->value);
    } else if (node->type_value == TYPE_OP) {
        for (int i = 0; i < COUNT_OPs; i++) {
            if (is_double_equal(OPs[i].type_op, node->value))
                printf(print_lyellow("%s\n"), OPs[i].name);
        }
    } else if (node->type_value == TYPE_VAR) {
        printf(print_lgreen("%s = %.2lf\n"), vars->names[(int)node->value], vars->value[(int)node->value]);
    }

    if (node->right) tree_cmd_dump_(node->right, vars, dep + 1);

    return tree_verify(node);
}

ErrorCode tree_graph_dump(Tree* tree, int* number_graph_dump)
{
    if (!tree) return ERROR_INVALID_TREE;

    char dot_path[MAX_SIZE_NAME_DUMP] = "";
    snprintf(dot_path, MAX_SIZE_NAME_DUMP, "%s/dot/%s%d", dump_dir, dump_name, *number_graph_dump);

    char *name_dot_file = nullptr;
    ErrorCode err = make_name_file(dot_path, ".dot", &name_dot_file);
    if (err) return err;
    FILE* dump_file = fopen(name_dot_file, "w");
    if (!dump_file) {
        printf("Error open file to dump\n");
        return ERROR_SYSTEM_COMMAND;
    }

    err = tree_verify(tree->root);
    if (err) return err;

    fprintf(dump_file, "digraph {\n"
                       "\tgraph[label = \"Tree\", labelloc = top, "
                       "labeljust = center, fontsize = 70, fontcolor = \"#e33e19\"];\n"
                       "\tgraph[dpi = 100];\n"
                       "\tbgcolor = \"#2F353B\";\n"
                       "\tedge[minlen = 3, arrowsize = 1.5, penwidth = 3];\n"
                       "\tnode[shape = \"rectangle\", style = \"rounded, filled\", height = 3, width = 2, "
                       "fillcolor = \"#ab5b0f\", fontsize = 30, penwidth = 3.5, color = \"#941b1b\"]\n");

    err = tree_graph_dump_make_node(tree->root, tree->variables, dump_file);
    if (err) return err;

    err = tree_graph_dump_make_edge(tree->root, dump_file);
    if (err) return err;

    fprintf(dump_file, "}\n");
    fclose(dump_file);

    char png_path[MAX_SIZE_NAME_DUMP] = "";
    snprintf(png_path, MAX_SIZE_NAME_DUMP, "%s/png/%s%d", dump_dir, dump_name, *number_graph_dump);

    char *name_png_file = nullptr;
    err = make_name_file(png_path, ".png", &name_png_file);
    if (err) return err;
    char command[MAX_SIZE_COMMAND] = "";
    snprintf(command, MAX_SIZE_COMMAND, "gvpack -u %s | dot -Tpng -o %s", 
                                        name_dot_file, name_png_file);
    int sys = system(command);
    if (sys) return ERROR_SYSTEM_COMMAND;

    (*number_graph_dump)++;
    free(name_dot_file);
    free(name_png_file);
    return tree_verify(tree->root);
}

static ErrorCode tree_graph_dump_make_node(Node* node, Variables* vars, FILE* dump_file)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(node);
    if (err) return err;

    if (node->left) tree_graph_dump_make_node(node->left, vars, dump_file);

    fprintf(dump_file, "\t{ \n"
                       "\t\tnode[shape = \"Mrecord\"];\n"
                       "\t\tnode%p[label = \"{ ", node);
    if (node->type_value == TYPE_NUM) {
        fprintf(dump_file, "%.2lf", node->value);
        fprintf(dump_file, " | { childs = %d | dep = %d } }\", fillcolor = \"#ab5b0f\"];\n",
                           node->size, node->dep);
    } else if (node->type_value == TYPE_OP) {
        for (int i = 0; i < COUNT_OPs; i++) {
            if (is_double_equal(OPs[i].type_op, node->value)) {
                fprintf(dump_file, "%s", OPs[i].name);
                fprintf(dump_file, " | { childs = %d | dep = %d }}\", fillcolor = \"#e3964d\"];\n",
                                   node->size, node->dep);
            }
        }
    } else if (node->type_value == TYPE_VAR) {
        fprintf(dump_file, "%s = %.2lf", vars->names[(int)node->value], vars->value[(int)node->value]);
        fprintf(dump_file, " | { childs = %d | dep = %d } }\", fillcolor = \"#f79e19\"];\n",
                           node->size, node->dep);
    }
    fprintf(dump_file, "\t}\n");

    if (node->right) tree_graph_dump_make_node(node->right, vars, dump_file);

    return tree_verify(node);
}

static ErrorCode tree_graph_dump_make_edge(Node* node, FILE* dump_file)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(node);
    if (err) return err;

    if (node->left)  tree_graph_dump_make_edge(node->left, dump_file);
    if (node->left)  fprintf(dump_file, "\tnode%p->node%p[color = yellow, labelangle = 45];\n", node, node->left);
    if (node->right) fprintf(dump_file, "\tnode%p->node%p[color = yellow, labelangle = 45];\n", node, node->right);
    if (node->right) tree_graph_dump_make_edge(node->right, dump_file);

    return tree_verify(node);
}

ErrorCode tree_html_dump(int number_graph_dump, int* number_html_dump)
{
    char buffer[MAX_SIZE_NAME_DUMP] = "";
    snprintf(buffer, MAX_SIZE_NAME_DUMP, "%s/html/%s%d.html", dump_dir, dump_name, *number_html_dump);

    FILE* html_file = fopen(buffer, "w");
    if (!html_file) {
        printf("Error open file to dump\n");
        return ERROR_OPEN_FILE;
    }

    fprintf(html_file, "<pre>\n");

    for (int i = 1; i < number_graph_dump; i++) {
        fprintf(html_file, "<img src = \"../png/%s%d.png\">\n", dump_name, i);
    }

    fprintf(html_file, "</pre>\n");

    (*number_html_dump)++;

    fclose(html_file);
    return ERROR_NO;
}

ErrorCode tree_tex_dump(Tree* tree, int* number_tex_dump)
{
    if (!tree) return ERROR_INVALID_TREE;

    char tex_path[MAX_SIZE_NAME_DUMP] = "";
    snprintf(tex_path, MAX_SIZE_NAME_DUMP, "%s/tex/%s%d", dump_dir, dump_name, *number_tex_dump);

    char *name_tex_file = nullptr;
    ErrorCode err = make_name_file(tex_path, ".tex", &name_tex_file);
    if (err) return err;

    FILE* dump_file = fopen(name_tex_file, "w");
    if (!dump_file) {
        printf("Error open file to dump\n");
        return ERROR_SYSTEM_COMMAND;
    }

    err = tree_verify(tree->root);
    if (err) return err;

    time_t mytime = time(NULL);
    struct tm *now = localtime(&mytime);

    fprintf(dump_file, "\\documentclass[a4paper,12pt]{article}\n\n"
                       "\\author{Baidiusenov Timur}\n"
                       "\\title{Differentiator}\n"
                       "\\date{Date: %d.%d.%d, Time %d:%d:%d}\n"
                       "\\begin{document}\n"
                       "\\maketitle\n",
                       now->tm_mday, now->tm_mon + 1, now->tm_year + 1900,
                       now->tm_hour, now->tm_min, now->tm_sec);

    err = tree_tex_dump_(tree->root, tree->variables, dump_file);
    if (err) return err;

    fprintf(dump_file, "\\end{document}\n");
    fclose(dump_file);

    char pdf_path[MAX_SIZE_NAME_DUMP] = "";
    snprintf(pdf_path, MAX_SIZE_NAME_DUMP, "%s/pdf/%s%d", dump_dir, dump_name, *number_tex_dump);

    char *name_aux_file = nullptr;
    err = make_name_file(pdf_path, ".aux", &name_aux_file);
    if (err) return err;
    char *name_tex_log_file = nullptr;
    err = make_name_file(pdf_path, ".log", &name_tex_log_file);
    if (err) return err;

    char command[MAX_SIZE_COMMAND] = "";
    snprintf(command, MAX_SIZE_COMMAND, "pdflatex -output-directory=%s/pdf %s ; "
                                        "rm %s ; rm %s", 
                                        dump_dir, name_tex_file, 
                                        name_aux_file, name_tex_log_file);
    int sys = system(command);
    if (sys) return ERROR_SYSTEM_COMMAND;

    (*number_tex_dump)++;
    free(name_tex_file);
    free(name_aux_file);
    free(name_tex_log_file);
    return tree_verify(tree->root);
}

static ErrorCode tree_tex_dump_(Node* node, Variables* vars, FILE* dump_file)
{
    assert(node);
    assert(vars);
    assert(dump_file);

    ErrorCode err = ERROR_NO;

    if (node->left) err =  tree_tex_dump_(node->left,  vars, dump_file);
    if (err) return err;
    if (node->right) err = tree_tex_dump_(node->right, vars, dump_file);
    if (err) return err;

    fprintf(dump_file, "\\begin{equation}\n\t");
    err = tree_equation_dump(node, vars, dump_file, BRANCH_ERR, TYPE_ERR, OP_ERR);
    if (err) return err;
    fprintf(dump_file, "\n\\end{equation}\n");

    return ERROR_NO;
}

static ErrorCode tree_equation_dump(Node* node, Variables* vars, FILE* dump_file,
                                    Branch branch, TypeData par_type, TypeOP par_op)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(node);
    if (err) return err;

    TypeOP node_op = OP_ERR;
    if (node->type_value == TYPE_OP)
        node_op = (TypeOP)(int)node->value;

    err = write_left_parenthesis(node, dump_file, branch, 
                                 par_type, par_op);
    if (err) return err;

    if (node->left) tree_equation_dump(node->left, vars, dump_file, BRANCH_LEFT, 
                                       node->type_value, node_op);

    if (node->type_value == TYPE_NUM) {
        fprintf(dump_file, "%.2lf", node->value);
    } else if (node->type_value == TYPE_OP) {
        switch ((int)node->value) {
            case (OP_ADD) :
                fprintf(dump_file, " + ");
                break;
            case (OP_SUB) :
                fprintf(dump_file, " - ");
                break;
            case (OP_MUL) :
                fprintf(dump_file, " \\cdot ");
                break;
            case (OP_DIV) :
                break;
            default :
                assert(0);
    }
    } else if (node->type_value == TYPE_VAR) {
        fprintf(dump_file, "%s", vars->names[(int)node->value]);
    }

    if (node->right) tree_equation_dump(node->right, vars, dump_file, BRANCH_RIGHT, 
                                        node->type_value, node_op);

    err = write_right_parenthesis(node, dump_file, 
                                  par_type, par_op);
    if (err) return err;

    return tree_verify(node);
}

static ErrorCode write_left_parenthesis(Node* node, FILE* dump_file, Branch branch,
                                        TypeData par_type, TypeOP par_op)
{
    assert(node);
    assert(dump_file);

    if (par_type == TYPE_OP) {
        if (par_op == OP_DIV) {
            if (branch == BRANCH_RIGHT)
                fprintf(dump_file, "{");
            if (branch == BRANCH_LEFT)
                fprintf(dump_file, "\\frac{");
        }
        if (par_op == OP_MUL && node->dep > 1)
            fprintf(dump_file, "(");
    }

    return ERROR_NO;
}

static ErrorCode write_right_parenthesis(Node* node, FILE* dump_file,
                                         TypeData par_type, TypeOP par_op)
{
    assert(node);
    assert(dump_file);
    
    if (par_type == TYPE_OP) {
        if (par_op == OP_DIV)
            fprintf(dump_file, "}");

        if (par_op == OP_MUL && node->dep > 1)
            fprintf(dump_file, ")");
    }

    return ERROR_NO;
}

static ErrorCode make_name_file(char* buffer, const char *type, char** name_dump_file)
{
    int len_buf = (int)strlen(buffer);
    int len_type = (int)strlen(type);
    int size_dump_file = len_buf + len_type + 1;
    *name_dump_file = (char*)calloc(size_dump_file, sizeof(char));
    (*name_dump_file) = (char*)memcpy((*name_dump_file), buffer, len_buf * sizeof(char) + 1);
    if (!(*name_dump_file)) return ERROR_ALLOC_FAIL;
    (*name_dump_file) = strcat((*name_dump_file), type);
    if (!(*name_dump_file)) return ERROR_ALLOC_FAIL;

    return ERROR_NO;
}

static int is_double_equal(double x, double y)
{
    return (fabs(x - y) <= EPSILON);
}