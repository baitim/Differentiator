#include <stdlib.h>

#include "Diff.h"

const int POISON_VALUE = -0xbe;

ErrorCode tree_read(Tree** tree, const char* buf)
{
    if (!tree) return ERROR_INVALID_TREE;
    if (!buf) return ERROR_INVALID_BUF;



    return ERROR_NO;
}

ErrorCode tree_write(Tree** tree, const char* buf, int dep)
{
    if (!tree) return ERROR_INVALID_TREE;
    if (!buf) return ERROR_INVALID_BUF;



    return ERROR_NO;
}

ErrorCode tree_insert(Tree** tree, Node* node)
{
    if (!tree) return ERROR_INVALID_TREE;



    return ERROR_NO;
}

ErrorCode tree_init(Tree** tree)
{
    if (!tree) return ERROR_INVALID_TREE;
    
    *tree = (Tree*)calloc(1, sizeof(Tree));
    if (!*tree) return ERROR_ALLOC_FAIL;

    (*tree)->node.type_value = TYPE_NAN;
    (*tree)->node.value = POISON_VALUE;

    return ERROR_NO;
}