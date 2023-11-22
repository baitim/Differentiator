#ifndef TREE_H
#define TREE_H

#include <stdio.h>

#include "Errors.h"
#include "Node.h"

struct Tree {
    Node* root;
};

ErrorCode tree_new      (Tree** tree);
ErrorCode tree_delete   (Tree* tree);
ErrorCode tree_verify   (Node* node);

#endif // TREE_H