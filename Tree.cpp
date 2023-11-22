#include <assert.h>
#include <stdlib.h>

#include "Tree.h"
#include "Diff.h"

const int POISON_SIZE = -1;

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
    
    tree->size = POISON_SIZE;

    free(tree);
    
    return ERROR_NO;
}

ErrorCode tree_verify(Tree* tree)
{
    assert(tree);

    int next_size = 0;
    if (tree->left)  next_size += tree->left->size + 1;
    if (tree->right) next_size += tree->right->size + 1;
    if (next_size != tree->size) return ERROR_COUNT_VALID;

    if (tree->left)  return tree_verify(tree->left);
    if (tree->right) return tree_verify(tree->right);
    
    return ERROR_NO;
}