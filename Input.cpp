#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Input.h"

#define MAX(a, b) ((a) > (b)) ? (a) : (b)

const int MAX_SIZE_INPUT = 100;
const int POISON_VALUE = -0xbe;

static ErrorCode fsize  (const char* name_file, int* size_file);
static ErrorCode get_arg(char** buf, char* str);
static char* skip_spaces(char* s);
static char* skip_word  (char* s);

ErrorCode tree_read(Tree** tree, char** buf, int* childs, int *dep)
{
    if (!tree) return ERROR_INVALID_TREE; 
    if (!buf) return ERROR_INVALID_BUF;

    ErrorCode err = ERROR_NO;

    if ((**buf) == '(') {
        err = tree_init(tree);
        if (err) return err;

        (*buf)++;

        *buf = skip_spaces(*buf);
        int left_childs = 0;
        int left_dep = 0;
        err = tree_read(&(*tree)->left, buf, &left_childs, &left_dep);
        if (err) return err;
        *buf = skip_spaces(*buf);
        (*childs) += left_childs;
        /////////////////////////////////////////////////////////
        char str[MAX_SIZE_INPUT] = "";
        err = get_arg(buf, str);
        if (err) return err;
        
        int is_op = 0;
        for (int i = 0; i < COUNT_OPs; i++) {
            if (strcmp(OPs[i].name, str) == 0) {
                is_op = 1;
                (*tree)->node.value = OPs[i].type_op;
                (*tree)->node.type_value = TYPE_OP;
                break;
            }
        }
        if (!is_op) {
            (*tree)->node.value = atoi(str);
            (*tree)->node.type_value = TYPE_NUM;
        }

        (*childs)++;
        /////////////////////////////////////////////////////////
        *buf = skip_spaces(*buf);
        int right_childs = 0;
        int right_dep = 0;
        err = tree_read(&(*tree)->right, buf, &right_childs, &right_dep);
        if (err) return err;
        *buf = skip_spaces(*buf);
        (*childs) += right_childs;

        (*tree)->size = (*childs) - 1;
        (*dep) = (*tree)->dep = MAX(left_dep, right_dep) + 1;

        return ERROR_NO;
    } else {
        *buf = skip_word(*buf); 
        return ERROR_NO;
    }

    return ERROR_NO;
}

static ErrorCode get_arg(char** buf, char* str)
{
    assert(buf);
    assert(*buf);
    assert(str);

    int i = 0;
    while ((*buf)[i] != '\0' && !isspace((*buf)[i])) {
        str[i] = (*buf)[i];
        i++;
    }
    i++;
    str[i] = '\0';

    (*buf) += i;
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

    (*tree)->node.type_value = TYPE_ERR;
    (*tree)->node.value = POISON_VALUE;

    return ERROR_NO;
}

ErrorCode file_to_buf(const char* name_file, char** buf)
{
    if (!name_file) return ERROR_OPEN_FILE;

    ErrorCode err = ERROR_NO;

    FILE* data_file = fopen(name_file, "r");
    if (!data_file) return ERROR_OPEN_FILE;

    int size_file = -1;
    err = fsize(name_file, &size_file);
    if (err) return err;

    *buf = (char *)calloc(size_file, sizeof(char));
    if (!(*buf)) return ERROR_ALLOC_FAIL;

    int count_read = (int)fread(*buf, sizeof(char), size_file, data_file);
    if (count_read != size_file - 1) return ERROR_READ_FILE;
    (*buf)[size_file - 1] = '\0';

    fclose(data_file);
    return err;
}

static ErrorCode fsize(const char* name_file, int* size_file) 
{
    assert(name_file);
    struct stat st = {};

    if (stat(name_file, &st) == 0) {
        (*size_file) = (int)st.st_size + 1;
        return ERROR_NO;
    }

    return ERROR_READ_FILE;
}