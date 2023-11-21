#include <stdio.h>
#include <cstdlib>

#include "ANSI_colors.h"
#include "Output.h"
#include "Node.h"

const int MAX_SIZE_NAME_DUMP = 50;
const int MAX_SIZE_COMMAND = 100;

static ErrorCode tree_cmd_dump_(Tree* tree, int dep);
static ErrorCode tree_graph_dump_make_node(Tree* tree, FILE *dump_file, int dep);
static ErrorCode tree_graph_dump_make_edge(Tree* tree, FILE *dump_file);

ErrorCode buf_to_file(const char* name_file, char** buf)
{


    return ERROR_NO;
}

ErrorCode tree_cmd_dump(Tree* tree)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(tree);
    if (err) return err;

    printf(print_magenta("^^^^^^^^^^^^^^^^^^^^\n\n"));
    printf(print_magenta("Tree:\n"));

    tree_cmd_dump_(tree, 0);

    printf(print_magenta("vvvvvvvvvvvvvvvvvvvv\n\n"));

    return tree_verify(tree);
}

static ErrorCode tree_cmd_dump_(Tree* tree, int dep)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(tree);
    if (err) return err;

    if (tree->left) tree_cmd_dump_(tree->left, dep + 1);

    for (int i = 0; i < dep; i++) printf("\t");
    if (!tree->left && !tree->right)    printf(print_lgreen("%lf\n"), tree->node.value);
    else                                printf(print_lyellow("%lf\n"), tree->node.value);

    if (tree->right) tree_cmd_dump_(tree->right, dep + 1);

    return tree_verify(tree);
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

    ErrorCode err = tree_verify(tree);
    if (err) return err;

    fprintf(dump_file, "digraph {\n"
                       "\tgraph[label = \"Tree\", labelloc = top, "
                       "labeljust = center, fontsize = 70, fontcolor = \"#e33e19\"];\n"
                       "\tgraph[dpi = 100];\n"
                       "\tbgcolor = \"#2F353B\";\n"
                       "\tedge[minlen = 3, arrowsize = 1.5, penwidth = 3];\n"
                       "\tnode[shape = \"rectangle\", style = \"rounded, filled\", height = 3, width = 2, "
                       "fillcolor = \"#ab5b0f\", fontsize = 30, penwidth = 3.5, color = \"#941b1b\"]\n");

    err = tree_graph_dump_make_node(tree, dump_file, 1);
    if (err) return err;

    err = tree_graph_dump_make_edge(tree, dump_file);
    if (err) return err;

    fprintf(dump_file, "}\n");

    fclose(dump_file);

    char command[MAX_SIZE_COMMAND] = "";
    snprintf(command, MAX_SIZE_COMMAND, "gvpack -u %s | dot -Tpng -o %s.png", buffer, buffer);
    int sys = system(command);
    if (sys) 
        return ERROR_SYSTEM_COMMAND;

    (*number_graph_dump)++;
    return tree_verify(tree);
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

static ErrorCode tree_graph_dump_make_node(Tree* tree, FILE* dump_file, int dep)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(tree);
    if (err) return err;

    if (tree->left) tree_graph_dump_make_node(tree->left, dump_file, dep + 1);

    fprintf(dump_file, "\t{ \n"
                       "\t\tnode[shape = \"Mrecord\"];\n");

    fprintf(dump_file, "\t\tnode%p[label = \"{ %lf | {", tree, tree->node.value);
    if (tree->left) fprintf(dump_file, "%lf", tree->left->node.value);
    else            fprintf(dump_file, "0");
    fprintf(dump_file, " | ");
    if (tree->right)fprintf(dump_file, "%lf", tree->right->node.value);
    else            fprintf(dump_file, "0");
    fprintf(dump_file, "} }\"];\n");

    fprintf(dump_file, "\t}\n");

    if (tree->right) tree_graph_dump_make_node(tree->right, dump_file, dep + 1);

    return tree_verify(tree);
}

static ErrorCode tree_graph_dump_make_edge(Tree* tree, FILE* dump_file)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(tree);
    if (err) return err;

    if (tree->left)  tree_graph_dump_make_edge(tree->left, dump_file);
    if (tree->left)  fprintf(dump_file, "\tnode%p->node%p[color = yellow, labelangle = 45];\n", tree, tree->left);
    if (tree->right) fprintf(dump_file, "\tnode%p->node%p[color = yellow, labelangle = 45];\n", tree, tree->right);
    if (tree->right) tree_graph_dump_make_edge(tree->right, dump_file);

    return tree_verify(tree);
}