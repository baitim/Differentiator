#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Input.h"

#define MAX(a, b) ((a) > (b)) ? (a) : (b)

static const int MAX_SIZE_INPUT = 500;
static const int POISON_VALUE = -0xbe;

static ErrorCode tree_read_     (Node** node, Variables* vars, char** buf, int* childs, int *dep);
static ErrorCode vars_read_     (Variables* vars, char** buf);
static ErrorCode fsize          (const char* name_file, int* size_file);
static ErrorCode get_arg        (char** buf, char* str);
static ErrorCode check_type_arg (char* str, TypeData* type_arg);
static ErrorCode write_arg      (Node* node, Variables* vars, char* str, TypeData type_arg);
static ErrorCode is_operator    (char* str, int* is_oper);
static ErrorCode is_number      (char* str, int* is_num);
static ErrorCode is_variable    (char* str, int* is_var);
static char* skip_spaces        (char* s);
static char* skip_word          (char* s);
static ErrorCode vars_increase_cap(Variables* vars);

ErrorCode tree_read(Tree* tree, char** buf)
{
    assert(buf);
    if (!tree) return ERROR_INVALID_TREE;

    int childs = 0;
    int dep = 0;
    ErrorCode err = tree_read_(&tree->root, tree->variables, buf, &childs, &dep);
    if (err) return err;

    err = vars_read_(tree->variables, buf);
    if (err) return err;

    return ERROR_NO;
}

static ErrorCode tree_read_(Node** node, Variables* vars, char** buf, int* childs, int *dep)
{
    if (!node) return ERROR_INVALID_TREE; 
    if (!buf) return ERROR_INVALID_BUF;

    ErrorCode err = ERROR_NO;

    if ((**buf) == '(') {
        err = node_init(node);
        if (err) return err;

        (*buf)++;

        *buf = skip_spaces(*buf);
        int left_childs = 0;
        int left_dep = 0;
        err = tree_read_(&(*node)->left, vars, buf, &left_childs, &left_dep);
        if (err) return err;
        *buf = skip_spaces(*buf);
        (*childs) += left_childs;
        /////////////////////////////////////////////////////////
        char str[MAX_SIZE_INPUT] = "";
        err = get_arg(buf, str);
        if (err) return err;

        TypeData type_arg = TYPE_ERR;
        err = check_type_arg(str, &type_arg);
        if (err || type_arg == TYPE_ERR) return err;

        err = write_arg(*node, vars, str, type_arg);
        if (err) return err;

        (*childs)++;
        /////////////////////////////////////////////////////////
        *buf = skip_spaces(*buf);
        int right_childs = 0;
        int right_dep = 0;
        err = tree_read_(&(*node)->right, vars, buf, &right_childs, &right_dep);
        if (err) return err;
        *buf = skip_spaces(*buf);
        (*childs) += right_childs;

        (*node)->size = (*childs) - 1;
        (*dep) = (*node)->dep = MAX(left_dep, right_dep) + 1;

        return ERROR_NO;
    } else {
        *buf = skip_word(*buf); 
        return ERROR_NO;
    }

    return ERROR_NO;
}

static ErrorCode vars_read_(Variables* vars, char** buf)
{
    assert(buf);
    assert(*buf);

    ErrorCode err = ERROR_NO;

    int count_vars = vars->count;
    while (**buf != '\0') {
        *buf = skip_spaces(*buf);

        char str[MAX_SIZE_INPUT] = "";
        err = get_arg(buf, str);
        if (err) return err;

        int is_var = 0;
        for (int i = 0; i < count_vars; i++) {
            if (strcmp(vars->names[i], str) == 0) {
                if (vars->valid[i] == 1) return ERROR_DUPLICATE_VAR;

                *buf = skip_spaces(*buf);
                if (**buf != '=') return ERROR_INVALID_INPUT;
                (*buf)++;
                *buf = skip_spaces(*buf);

                char value[MAX_SIZE_INPUT] = "";
                err = get_arg(buf, value);
                if (err) return err;

                vars->value[i] = atof(value);
                vars->valid[i] = 1;

                is_var = 1;
                break;
            }
        }

        if (!is_var) {
            *buf = skip_word(*buf);
        }
    }

    vars->capacity = count_vars;
    
    vars->names = (char**)realloc(vars->names, vars->capacity * sizeof(char*));
    if (!vars->names) return ERROR_ALLOC_FAIL;

    vars->valid = (int*)realloc(vars->valid, vars->capacity * sizeof(int));
    if (!vars->valid) return ERROR_ALLOC_FAIL;

    vars->value = (double*)realloc(vars->value, vars->capacity * sizeof(double));
    if (!vars->value) return ERROR_ALLOC_FAIL;

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
    str[i + 1] = '\0';

    *buf = skip_word(*buf);
    return ERROR_NO;
}

static ErrorCode check_type_arg(char* str, TypeData* type_arg)
{
    assert(str);
    ErrorCode err = ERROR_NO;
    int is_oper = 0;
    err = is_operator(str, &is_oper);
    if (err) return err;
    if (is_oper) {
        (*type_arg) = TYPE_OP;
        return ERROR_NO;
    }
    int is_num =  0;
    err = is_number(str, &is_num);
    if (err) return err;
    if (is_num) {
        (*type_arg) = TYPE_NUM;
        return ERROR_NO;
    }
    int is_var =  0;
    err = is_variable(str, &is_var);
    if (err) return err;
    if (is_var) {
        (*type_arg) = TYPE_VAR;
        return ERROR_NO;
    }

    (*type_arg) = TYPE_ERR;
    return ERROR_INPUT_VARIABLE;
}

static ErrorCode write_arg(Node* node, Variables* vars, char* str, TypeData type_arg)
{
    ErrorCode err = ERROR_NO;

    node->type_value = type_arg;
    if (type_arg == TYPE_OP) {
        for (int i = 0; i < COUNT_OPs; i++) {
            if (strcmp(OPs[i].name, str) == 0) {
                node->value = OPs[i].type_op;
                break;
            }
        }
    }

    if (type_arg == TYPE_NUM)
        node->value = atoi(str);
    
    if (type_arg == TYPE_VAR) {
        int was = 0;
        for (int i = 0; i < vars->count; i++) {
            if (strcmp(vars->names[i], str) == 0) {
                node->value = i;
                was = 1;
                break;
            }
        }
        if (!was) {
            node->value = vars->count;
            vars->names[vars->count] = strdup(str);
            if (!vars->names[vars->count]) return ERROR_STRDUP;
            vars->count++;
            if (vars->count >= vars->capacity - 1) {
                err = vars_increase_cap(vars);
                if (err) return err;
            }
        }
    }

    return ERROR_NO;
}

static ErrorCode is_operator(char* str, int* is_oper)
{
    for (int i = 0; i < COUNT_OPs; i++) {
        if (strcmp(OPs[i].name, str) == 0) {
            (*is_oper) = 1;
            break;
        }
    }
    
    return ERROR_NO;
}

static ErrorCode is_number(char* str, int* is_num)
{
    int i = 0;
    while (str[i] != '\0' && !isspace(str[i])) {
        if (str[i] < '0' || str[i] > '9') {
            (*is_num) = 0;
            return ERROR_NO;
        }
        i++;
    }
    (*is_num) = 1;

    return ERROR_NO;
}

static ErrorCode is_variable(char* str, int* is_var)
{
    int i = 0;
    while (str[i] != '\0' && !isspace(str[i])) {
        for (int j = 0; j < (int)sizeof(banned_symbols); j++) {
            if (str[i] == banned_symbols[j]) {
                (*is_var) = 0;
                return ERROR_INPUT_VARIABLE;
            }
        }
        i++;
    }
    (*is_var) = 1;

    return ERROR_NO;
}

static char* skip_spaces(char* s)
{
    while (*s != '\0' && isspace(*s))
        s++;
    return s;
}

static char* skip_word(char* s)
{
    while (*s != '\0' && !isspace(*s))
        s++;
    return s;
}

ErrorCode node_init(Node** node)
{
    if (!node) return ERROR_INVALID_TREE;
    
    *node = (Node*)calloc(1, sizeof(Node));
    if (!*node) return ERROR_ALLOC_FAIL;

    (*node)->type_value = TYPE_ERR;
    (*node)->value = POISON_VALUE;

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

static ErrorCode vars_increase_cap(Variables* vars)
{
    assert(vars);

    vars->capacity = (int)(vars->capacity * MULTIPLIER_CAPACITY);
    vars->names = (char**)realloc(vars->names, vars->capacity * sizeof(char*));
    if (!vars->names) return ERROR_ALLOC_FAIL;

    vars->valid = (int*)realloc(vars->valid, vars->capacity * sizeof(int));
    if (!vars->valid) return ERROR_ALLOC_FAIL;

    vars->value = (double*)realloc(vars->value, vars->capacity * sizeof(double));
    if (!vars->value) return ERROR_ALLOC_FAIL;

    return ERROR_NO;
}