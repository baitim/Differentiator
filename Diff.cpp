#include <stdlib.h>
#include <ctype.h>

#include "Diff.h"

enum Branch {
    BRANCH_LEFT =  -1,
    BRANCH_RIGHT =  1,
};

const int MAX_SIZE_INPUT = 100;
const int POISON_VALUE = -0xbe;

static char* skip_spaces(char* s);
static char* skip_word  (char* s);

ErrorCode tree_read(Tree** tree, char** buf)
{
    if (!tree) return ERROR_INVALID_TREE;
    if (!buf) return ERROR_INVALID_BUF;

    ErrorCode err = ERROR_NO;

    if ((*buf)[0] == '(') {
        err = tree_init(tree);
        if (err) return err;

        (*buf)++;

        *buf = skip_spaces(*buf);
        err = tree_read(&(*tree)->left, buf);
        if (err) return err;
        *buf = skip_spaces(*buf);

        char str[MAX_SIZE_INPUT] = "";
        int i = 0;
        while ((*buf)[i] != '\0') {
            if (isspace((*buf)[i])) break;

            str[i] = (*buf)[i];
            i++;
        }
        i++;
        str[i] = '\0';

        (*tree)->node.value = atoi(str);
        (*buf) += i;

        *buf = skip_spaces(*buf);
        err = tree_read(&(*tree)->right, buf);
        if (err) return err;
        *buf = skip_spaces(*buf);

        return tree_verify(*tree);
    } else {
        printf("str nill = %s\n", *buf);
        *buf = skip_word(*buf); 
        return ERROR_NO;
    }

    return ERROR_NO;
}

static char* skip_spaces(char* s)
{
    int i = 0;
    while (isspace(s[i]))
        i++;
    return &s[i];
}

static char* skip_word(char* s)
{
    int i = 0;
    while (s[i] != '\0' && !isspace(s[i]))
        i++;
    return &s[i];
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