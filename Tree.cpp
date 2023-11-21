#include <assert.h>
#include <stdlib.h>

#include "Tree.h"
#include "Diff.h"

enum Branch {
    BRANCH_LEFT =  -1,
    BRANCH_RIGHT =  1,
};

ErrorCode tree_new(Tree** tree, Node* node)
{
    if (!tree && !*tree) return ERROR_INVALID_TREE;

    tree_insert(tree, node);

    return tree_verify((*tree));
}

ErrorCode tree_delete(Tree* tree)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(tree);
    if (err) return err;

    if (tree->left) return tree_delete(tree->left);
    if (tree->right) return tree_delete(tree->right);

    free(tree);
    
    return ERROR_NO;
}

ErrorCode tree_verify(Tree* tree)
{
    assert(tree);

    if (tree->left)  return tree_verify(tree->left);
    if (tree->right) return tree_verify(tree->right);
    
    return ERROR_NO;
}