#include <stdio.h>
#include <cstdlib>
#include <math.h>

#include "ANSI_colors.h"
#include "Output.h"
#include "Node.h"

const int MAX_SIZE_NAME_DUMP = 50;
const int MAX_SIZE_COMMAND = 100;
const double EPSILON = 1e-9;

static ErrorCode tree_cmd_dump_             (Node* node, Variables* vars, int dep);
static ErrorCode tree_graph_dump_make_node  (Node* node, Variables* vars, FILE *dump_file, int dep);
static ErrorCode tree_graph_dump_make_edge  (Node* node, FILE *dump_file);
static int is_double_equal                  (double x, double y);

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
    char buffer[MAX_SIZE_NAME_DUMP] = "";
    snprintf(buffer, MAX_SIZE_NAME_DUMP, "dumps/dump%d", *number_graph_dump);

    FILE* dump_file = fopen(buffer, "w");
    if (!dump_file) {
        printf("Error open file to dump\n");
        return ERROR_SYSTEM_COMMAND;
    }

    if (!tree)
        return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(tree->root);
    if (err) return err;

    fprintf(dump_file, "digraph {\n"
                       "\tgraph[label = \"Tree\", labelloc = top, "
                       "labeljust = center, fontsize = 70, fontcolor = \"#e33e19\"];\n"
                       "\tgraph[dpi = 100];\n"
                       "\tbgcolor = \"#2F353B\";\n"
                       "\tedge[minlen = 3, arrowsize = 1.5, penwidth = 3];\n"
                       "\tnode[shape = \"rectangle\", style = \"rounded, filled\", height = 3, width = 2, "
                       "fillcolor = \"#ab5b0f\", fontsize = 30, penwidth = 3.5, color = \"#941b1b\"]\n");

    err = tree_graph_dump_make_node(tree->root, tree->variables, dump_file, 1);
    if (err) return err;

    err = tree_graph_dump_make_edge(tree->root, dump_file);
    if (err) return err;

    fprintf(dump_file, "}\n");

    fclose(dump_file);

    char command[MAX_SIZE_COMMAND] = "";
    snprintf(command, MAX_SIZE_COMMAND, "gvpack -u %s | dot -Tpng -o %s.png", buffer, buffer);
    int sys = system(command);
    if (sys) 
        return ERROR_SYSTEM_COMMAND;

    (*number_graph_dump)++;
    return tree_verify(tree->root);
}

ErrorCode tree_html_dump(int number_graph_dump)
{
    char buffer[MAX_SIZE_NAME_DUMP] = "";
    snprintf(buffer, MAX_SIZE_NAME_DUMP, "dumps/dump.html");

    FILE* html_file = fopen(buffer, "w");
    if (!html_file) {
        printf("Error open file to dump\n");
        return ERROR_OPEN_FILE;
    }

    fprintf(html_file, "<pre>\n");

    for (int i = 1; i <= number_graph_dump; i++) {
        fprintf(html_file, "<img src = \"dump%d.png\">\n", i);
    }

    fprintf(html_file, "</pre>\n");

    fclose(html_file);
    return ERROR_NO;
}

static ErrorCode tree_graph_dump_make_node(Node* node, Variables* vars, FILE* dump_file, int dep)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(node);
    if (err) return err;

    if (node->left) tree_graph_dump_make_node(node->left, vars, dump_file, dep + 1);

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

    if (node->right) tree_graph_dump_make_node(node->right, vars, dump_file, dep + 1);

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

static int is_double_equal(double x, double y)
{
    return (fabs(x - y) <= EPSILON);
}