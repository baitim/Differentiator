#include <assert.h>

#include "Diff.h"

ErrorCode tree_eval(Tree* tree, double* ans_eval)
{
    if (!tree) return ERROR_INVALID_TREE;

    double left_eval =  -1;
    double right_eval = -1;
    if (tree->left)  tree_eval(tree->left,  &left_eval );
    if (tree->right) tree_eval(tree->right, &right_eval);

    if (!tree->left && !tree->right) {
        *ans_eval = tree->node.value;
    } else {
        switch ((int)tree->node.value) {
            case (OP_ADD) :
                *ans_eval = left_eval + right_eval;
                break;
            case (OP_SUB) :
                *ans_eval = left_eval - right_eval;
                break;
            case (OP_MUL) :
                *ans_eval = left_eval * right_eval;
                break;
            case (OP_DIV) :
                *ans_eval = left_eval / right_eval;
                break;
            default :
                assert(0);
        }
    }

    return tree_verify(tree);
}