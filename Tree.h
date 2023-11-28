#ifndef TREE_H
#define TREE_H

#include <stdio.h>

#include "Errors.h"
#include "Node.h"

struct OutputInfo;

struct Tree {
    char* name;
    TreeNode* root;
    Variables* variables;
    OutputInfo* output_info;
};

ErrorCode tree_new      (Tree** tree, const char*  tree_name);
ErrorCode tree_delete   (Tree* tree);
ErrorCode tree_verify   (TreeNode* node);
ErrorCode tree_copy     (Tree* tree, const char* name, Tree** new_tree);

#endif // TREE_H