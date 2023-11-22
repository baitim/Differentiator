#include <assert.h>
#include <stdlib.h>

#include "Tree.h"
#include "Diff.h"

#define MAX(a, b) ((a) > (b)) ? (a) : (b)

enum TypeNodeColor {
    NODE_COLOR_WHITE = 0,
    NODE_COLOR_BLACK = 1,
};

const int POISON_INT = -1;

static ErrorCode tree_verify_(Tree* tree, int *color, int num);

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

    int count_nodes = (1 << tree->dep);
    int* color = (int*)calloc(count_nodes, sizeof(int));
    if (!color) return ERROR_ALLOC_FAIL;

    err = tree_verify_(tree, color, 1);
    if (err) return err;
    
    return ERROR_NO;
}

static ErrorCode tree_verify_(Tree* tree, int* color, int num)
{
    assert(tree);
    assert(color);

    ErrorCode err = ERROR_NO;

    if (color[num] == NODE_COLOR_BLACK) return ERROR_TREE_LOOP;
    color[num] = NODE_COLOR_BLACK;

    if (tree->dep < 1) return ERROR_TREE_DEP;
    if (tree->size < 0) return ERROR_COUNT_VALID;

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

    if (tree->left)  err = tree_verify_(tree->left, color, num * 2);
    if (err) return err;
    if (tree->right) err = tree_verify_(tree->right, color, num * 2 + 1);
    if (err) return err;

    return ERROR_NO;
}