#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "Math.h"
#include "Output.h"
#include "Equation.h"

enum TypeNodeColor {
    NODE_COLOR_WHITE = 0,
    NODE_COLOR_BLACK = 1,
};

static ErrorCode equation_verify_(EquationNode* node, int *color, int num);

ErrorCode equation_new(Equation** equation, const char* equation_name)
{
    if (!equation && !*equation) return ERROR_INVALID_TREE;

    *equation = (Equation*)calloc(1, sizeof(Equation));
    if (!*equation) return ERROR_ALLOC_FAIL;

    (*equation)->name = strdup(equation_name);
    if (!(*equation)->name) return ERROR_ALLOC_FAIL;
    ////////////////////////////////////////////////////////////////////
    (*equation)->variables = (Variables*)calloc(1, sizeof(Variables));
    if (!(*equation)->variables) return ERROR_ALLOC_FAIL;

    (*equation)->variables->var = (Variable*)calloc(1, sizeof(Variable));
    if (!(*equation)->variables->var) return ERROR_ALLOC_FAIL;

    (*equation)->variables->count = 0;
    (*equation)->variables->capacity = DEFAULT_CAPACITY;
    ////////////////////////////////////////////////////////////////////
    (*equation)->output_info = (OutputInfo*)calloc(1, sizeof(OutputInfo));
    if (!(*equation)->output_info) return ERROR_ALLOC_FAIL;

    (*equation)->output_info->number_graph_dump = 1;
    (*equation)->output_info->number_svg_dump =   1;
    (*equation)->output_info->number_tex_dump =   1;
    (*equation)->output_info->number_html_dump =  1;

    return ERROR_NO;
}

ErrorCode equation_delete(Equation* equation)
{
    if (!equation) return ERROR_INVALID_TREE;

    ErrorCode err = node_delete(&equation->root);
    if (err) return err;

    for (size_t i = 0; i < equation->variables->capacity; i++) {
        free(equation->variables->var[i].name);
    }
    
    equation->variables->count =                POISON_VALUE;
    equation->variables->capacity =             POISON_VALUE;
    equation->output_info->number_graph_dump =  POISON_VALUE;
    equation->output_info->number_svg_dump =    POISON_VALUE;
    equation->output_info->number_tex_dump =    POISON_VALUE;
    equation->output_info->number_html_dump =   POISON_VALUE;
    free(equation->variables->var);
    free(equation->variables);
    free(equation->output_info);
    free(equation->name);
    free(equation);
    
    return ERROR_NO;
}

ErrorCode equation_copy(Equation* equation, const char* name, Equation** new_equation)
{
    if (!equation) return ERROR_INVALID_TREE;
    
    ErrorCode err = ERROR_NO;

    err = equation_new(new_equation, name);
    if (err) return err;

    err = node_copy(equation->root, &(*new_equation)->root);
    if (err) return err;

    err = variables_copy(equation->variables, &(*new_equation)->variables);
    if (err) return err;

    return ERROR_NO;
}

ErrorCode equation_verify(EquationNode* node)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    size_t count_nodes = (1 << node->depth);
    int* color = (int*)calloc(count_nodes, sizeof(int));
    if (!color) return ERROR_ALLOC_FAIL;

    err = equation_verify_(node, color, 1);
    if (err) return err;
    
    free(color);
    return ERROR_NO;
}

static ErrorCode equation_verify_(EquationNode* node, int* color, int num)
{
    assert(node);
    assert(color);

    ErrorCode err = ERROR_NO;

    if (color[num] == NODE_COLOR_BLACK) return ERROR_TREE_LOOP;
    color[num] = NODE_COLOR_BLACK;

    if (node->depth < 1) return ERROR_TREE_DEP;

    size_t next_depth = 0;
    if (node->left)  next_depth = MAX(node->left->depth,  next_depth);
    if (node->right) next_depth = MAX(node->right->depth, next_depth);
    next_depth++;
    if ((node->type_value == TYPE_NUM || node->type_value == TYPE_VAR) 
        && next_depth != 1) return ERROR_LEAF;

    if (next_depth != node->depth) return ERROR_TREE_DEP;

    if (node->left)  err = equation_verify_(node->left, color, num * 2);
    if (err) return err;
    if (node->right) err = equation_verify_(node->right, color, num * 2 + 1);
    if (err) return err;

    return ERROR_NO;
}