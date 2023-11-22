#include <assert.h>

#include "Diff.h"

ErrorCode tree_eval(Node* node, double* ans_eval)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(node);
    if (err) return err;

    double left_eval =  -1;
    double right_eval = -1;
    if (node->left)  tree_eval(node->left,  &left_eval );
    if (node->right) tree_eval(node->right, &right_eval);

    if (!node->left && !node->right) {
        *ans_eval = node->value;
    } else {
        switch ((int)node->value) {
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

    return tree_verify(node);
}