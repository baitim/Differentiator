#include <assert.h>
#include <stdlib.h>

#include "Node.h"

static const int POISON_VALUE = -0xbe;

ErrorCode node_init(Node** node)
{
    assert(node);
    
    *node = (Node*)calloc(1, sizeof(Node));
    if (!*node) return ERROR_ALLOC_FAIL;

    (*node)->type_value =   TYPE_ERR;
    (*node)->value =        POISON_VALUE;
    (*node)->size =         POISON_VALUE;
    (*node)->dep =          POISON_VALUE;

    return ERROR_NO;
}

ErrorCode node_copy(Node* node, Node** new_node)
{
    assert(node);
    
    ErrorCode err = ERROR_NO;

    err = node_init(new_node);
    if (err) return err;

    (*new_node)->type_value =   node->type_value;
    (*new_node)->value =        node->value;
    (*new_node)->size =         node->size;
    (*new_node)->dep =          node->dep;

    if (node->left)  err = node_copy(node->left,  &(*new_node)->left);
    if (err) return err;

    if (node->right) err = node_copy(node->right, &(*new_node)->right);
    if (err) return err;

    return ERROR_NO;
}