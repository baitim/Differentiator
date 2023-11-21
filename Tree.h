#ifndef TREE_H
#define TREE_H

#include <stdio.h>

#include "Errors.h"
#include "Node.h"

struct Tree {
    Node node;
    Tree* left;
    Tree* right;
};

ErrorCode tree_new      (Tree** tree, Node* node);
ErrorCode tree_delete   (Tree* tree);
ErrorCode tree_verify   (Tree* tree);

#endif // TREE_H