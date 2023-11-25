#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "Tree.h"
#include "Output.h"

#define MAX(a, b) ((a) > (b)) ? (a) : (b)

enum TypeNodeColor {
    NODE_COLOR_WHITE = 0,
    NODE_COLOR_BLACK = 1,
};

static const int POISON_INT = -1;

static ErrorCode tree_delete_(Node* node);
static ErrorCode tree_verify_(Node* node, int *color, int num);

ErrorCode tree_new(Tree** tree, const char* tree_name)
{
    if (!tree && !*tree) return ERROR_INVALID_TREE;

    *tree = (Tree*)calloc(1, sizeof(Tree));
    if (!*tree) return ERROR_ALLOC_FAIL;

    (*tree)->name = strdup(tree_name);
    if (!(*tree)->name) return ERROR_ALLOC_FAIL;
    ////////////////////////////////////////////////////////////////////
    (*tree)->variables = (Variables*)calloc(1, sizeof(Variables));
    if (!(*tree)->variables) return ERROR_ALLOC_FAIL;

    (*tree)->variables->var = (Variable*)calloc(1, sizeof(Variable));
    if (!(*tree)->variables->var) return ERROR_ALLOC_FAIL;

    (*tree)->variables->count = 0;
    (*tree)->variables->capacity = DEFAULT_CAPACITY;
    ////////////////////////////////////////////////////////////////////
    (*tree)->output_info = (OutputInfo*)calloc(1, sizeof(OutputInfo));
    if (!(*tree)->output_info) return ERROR_ALLOC_FAIL;

    (*tree)->output_info->number_graph_dump = 1;
    (*tree)->output_info->number_tex_dump =   1;
    (*tree)->output_info->number_html_dump =  1;

    prepare_dump_dir(*tree);

    return ERROR_NO;
}

ErrorCode tree_delete(Tree* tree)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_delete_(tree->root);
    if (err) return err;

    for (int i = 0; i < tree->variables->capacity; i++) {
        free(tree->variables->var[i].name);
    }
    
    tree->variables->count =                POISON_INT;
    tree->variables->capacity =             POISON_INT;
    tree->output_info->number_graph_dump =  POISON_INT;
    tree->output_info->number_tex_dump =    POISON_INT;
    tree->output_info->number_html_dump =   POISON_INT;
    free(tree->variables->var);
    free(tree->variables);
    free(tree->output_info);
    free(tree->name);
    free(tree);
    
    return ERROR_NO;
}

static ErrorCode tree_delete_(Node* node) 
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(node);
    if (err) return err;

    if (node->left)  tree_delete_(node->left);
    if (node->right) tree_delete_(node->right);
    
    node->size = POISON_INT;
    node->dep =  POISON_INT;

    free(node);
    return ERROR_NO;
}

ErrorCode tree_copy(Tree* tree, const char* name, Tree** new_tree)
{
    if (!tree) return ERROR_INVALID_TREE;
    
    ErrorCode err = ERROR_NO;

    err = tree_new(new_tree, name);
    if (err) return err;

    err = node_copy(tree->root, &(*new_tree)->root);
    if (err) return err;

    err = variables_copy(tree->variables, &(*new_tree)->variables);
    if (err) return err;

    return ERROR_NO;
}

ErrorCode tree_verify(Node* node)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    int count_nodes = (1 << node->dep);
    int* color = (int*)calloc(count_nodes, sizeof(int));
    if (!color) return ERROR_ALLOC_FAIL;

    err = tree_verify_(node, color, 1);
    if (err) return err;
    
    free(color);
    return ERROR_NO;
}

static ErrorCode tree_verify_(Node* node, int* color, int num)
{
    assert(node);
    assert(color);

    ErrorCode err = ERROR_NO;

    if (color[num] == NODE_COLOR_BLACK) return ERROR_TREE_LOOP;
    color[num] = NODE_COLOR_BLACK;

    if (node->dep < 1) return ERROR_TREE_DEP;
    if (node->size < 0) return ERROR_COUNT_VALID;

    int next_dep = 0;
    if (node->left)  next_dep = MAX(node->left->dep, next_dep);
    if (node->right) next_dep = MAX(node->right->dep, next_dep);
    next_dep++;
    if ((node->type_value == TYPE_NUM || node->type_value == TYPE_VAR) 
        && next_dep != 1) return ERROR_LEAF;

    if (next_dep != node->dep) return ERROR_TREE_DEP;

    int next_size = 0;
    if (node->left)  next_size += node->left->size + 1;
    if (node->right) next_size += node->right->size + 1;
    if (next_size != node->size) return ERROR_COUNT_VALID;

    if (node->left)  err = tree_verify_(node->left, color, num * 2);
    if (err) return err;
    if (node->right) err = tree_verify_(node->right, color, num * 2 + 1);
    if (err) return err;

    return ERROR_NO;
}