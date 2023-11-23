#include <assert.h>
#include <stdlib.h>

#include "Tree.h"
#include "Diff.h"

#define MAX(a, b) ((a) > (b)) ? (a) : (b)

enum TypeNodeColor {
    NODE_COLOR_WHITE = 0,
    NODE_COLOR_BLACK = 1,
};

static const int POISON_INT = -1;

static ErrorCode tree_delete_(Node* node);
static ErrorCode tree_verify_(Node* node, int *color, int num);

ErrorCode tree_new(Tree** tree)
{
    if (!tree && !*tree) return ERROR_INVALID_TREE;

    *tree = (Tree*)calloc(1, sizeof(Tree));
    if (!*tree) return ERROR_ALLOC_FAIL;

    (*tree)->variables = (Variables*)calloc(1, sizeof(Variables));
    if (!(*tree)->variables) return ERROR_ALLOC_FAIL;

    (*tree)->variables->names = (char**)calloc(DEFAULT_CAPACITY, sizeof(char*));
    if (!(*tree)->variables->names) return ERROR_ALLOC_FAIL;

    (*tree)->variables->valid = (int*)calloc(DEFAULT_CAPACITY, sizeof(int));
    if (!(*tree)->variables->valid) return ERROR_ALLOC_FAIL;

    (*tree)->variables->value = (double*)calloc(DEFAULT_CAPACITY, sizeof(double));
    if (!(*tree)->variables->value) return ERROR_ALLOC_FAIL;

    (*tree)->variables->count = 0;
    (*tree)->variables->capacity = DEFAULT_CAPACITY;

    return ERROR_NO;
}

ErrorCode tree_delete(Tree* tree)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_delete_(tree->root);
    if (err) return err;

    for (int i = 0; i < tree->variables->count; i++)
        free(tree->variables->names[i]);
    tree->variables->count = POISON_INT;
    free(tree->variables->names);
    free(tree->variables->valid);
    free(tree->variables->value);
    free(tree->variables);
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