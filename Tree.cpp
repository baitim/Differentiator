#include <assert.h>
#include <stdlib.h>

#include "Tree.h"
#include "Diff.h"

#define MAX(a, b) ((a) > (b)) ? (a) : (b)

const int POISON_INT = -1;

static ErrorCode tree_verify_(Tree* tree, int *color);

ErrorCode tree_new(Tree** tree)
{
    if (!tree && !*tree) return ERROR_INVALID_TREE;

    return ERROR_NO;
}

ErrorCode tree_delete(Tree* tree)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(tree);
    if (err) return err;

    if (tree->left) return tree_delete(tree->left);
    if (tree->right) return tree_delete(tree->right);
    
    tree->size = POISON_INT;
    tree->dep =  POISON_INT;

    free(tree);
    
    return ERROR_NO;
}

ErrorCode tree_verify(Tree* tree)
{
    assert(tree);

    ErrorCode err = ERROR_NO;

    int* color = (int*)calloc(MAX(tree->dep, 0), sizeof(int));
    if (!color) return ERROR_ALLOC_FAIL;

    err = tree_verify_(tree, color);
    if (err) return err;
    
    return ERROR_NO;
}

static ErrorCode tree_verify_(Tree* tree, int* color)
{
    assert(tree);
    assert(color);

    if (tree->node.type_value == TYPE_NUM
        && (tree->left || tree->right || tree->node.type_value != 1)) return ERROR_NUMBER_IS_OP;

    int next_dep = 0;
    if (tree->left)  next_dep = MAX(tree->left->dep, next_dep);
    if (tree->right) next_dep = MAX(tree->right->dep, next_dep);
    next_dep++;
    if (tree->node.type_value == TYPE_NUM && next_dep != 1) return ERROR_NUMBER_IS_OP;
    if (next_dep != tree->dep) return ERROR_TREE_DEP;

    int next_size = 0;
    if (tree->left)  next_size += tree->left->size + 1;
    if (tree->right) next_size += tree->right->size + 1;
    if (next_size != tree->size) return ERROR_COUNT_VALID;

    if (tree->left)  return tree_verify(tree->left);
    if (tree->right) return tree_verify(tree->right);

    return ERROR_NO;
}