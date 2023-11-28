#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "ANSI_colors.h"
#include "Input.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

static ErrorCode tree_read_     (TreeNode** node, Variables* vars, char** buf, int *dep);
static ErrorCode vars_read_     (Variables* vars, char** buf);
static ErrorCode fsize          (const char* name_file, int* size_file);
static ErrorCode get_arg        (char** buf, char* str);
static ErrorCode check_type_arg (char* str, TreeDataType* type_arg);
static ErrorCode write_arg      (TreeNode* node, Variables* vars, char* str, TreeDataType type_arg);
static ErrorCode is_operator    (char* str, int* is_oper);
static ErrorCode is_number      (char* str, int* is_num);
static ErrorCode is_variable    (char* str, int* is_var);
static char* skip_spaces        (char* ass);
static char* skip_word          (char* sus);
static ErrorCode vars_increase_cap(Variables* vars);

ErrorCode tree_get_val_vars(Tree* tree)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(tree->root);
    if (err) return err;

    for (int i = 0; i < tree->variables->count; i++) {
        if (!tree->variables->var[i].valid) {
            printf(print_lcyan("You should input variables:\n"));
            break;
        }
    }
                
    for (int i = 0; i < tree->variables->count; i++)
        get_var(tree->variables, i);

    printf(print_lcyan("All variables have value\n"));

    return tree_verify(tree->root);
}

ErrorCode tree_get_num_var(Tree* tree, int* num_var)
{
    if (!tree) return ERROR_INVALID_TREE;

    ErrorCode err = tree_verify(tree->root);
    if (err) return err;

    printf(print_lcyan("You can choose this variables:\n"));
    for (int i = 0; i < tree->variables->count; i++)
        printf(print_lcyan("\t· %s\n"), tree->variables->var[i].name);

    char name_var[MAX_SIZE_VAR] = "";
    while (1) {
        printf(print_lcyan("Input name of variable: "));
        int count_read = scanf("%s", name_var);

        if (count_read != 1) {
            clean_stdin();
            printf(print_lred("Wrong argument, you should input name of existing variable\n"));
        }

        int exist = 0;
        for (int i = 0; i < tree->variables->count; i++) {
            if (strcmp(tree->variables->var[i].name, name_var) == 0) {
                *num_var = i;
                exist = 1;
            }
        }

        if (exist == 1 && count_read == 1) break;

        clean_stdin();
        printf(print_lred("Wrong argument, you should input name of existing variable\n"));
    }

    return tree_verify(tree->root);
}

ErrorCode tree_read(Tree* tree, char** buf)
{
    assert(buf);
    if (!tree) return ERROR_INVALID_TREE;

    int dep = 0;
    ErrorCode err = tree_read_(&tree->root, tree->variables, buf, &dep);
    if (err) return err;

    err = vars_read_(tree->variables, buf);
    if (err) return err;

    return ERROR_NO;
}

ErrorCode get_var(Variables* vars, int number_var)
{
    if (!vars->var[number_var].valid) {
        printf(print_lcyan("Input value of %s: "), vars->var[number_var].name);
        int count_read = scanf("%lf", &vars->var[number_var].value);
        while (count_read != 1) {
            clean_stdin();
            printf(print_lred("Wrong argument, you should input double, try again: "));
            count_read = scanf("%lf", &vars->var[number_var].value);
        }
        vars->var[number_var].valid = 1;
    }

    return ERROR_NO;
}

ErrorCode clean_stdin()
{
    while (getchar() != '\n')
        ;
    return ERROR_NO;
}

static ErrorCode tree_read_(TreeNode** node, Variables* vars, char** buf, int *dep)
{
    if (!node) return ERROR_INVALID_TREE; 
    if (!buf) return ERROR_INVALID_BUF;

    ErrorCode err = ERROR_NO;

    if ((**buf) == '(') {
        err = node_init(node);
        if (err) return err;

        (*buf)++;

        *buf = skip_spaces(*buf);
        int left_dep = 0;
        err = tree_read_(&(*node)->left, vars, buf, &left_dep);
        if (err) return err;
        *buf = skip_spaces(*buf);
        /////////////////////////////////////////////////////////
        char str[MAX_SIZE_INPUT] = "";
        err = get_arg(buf, str);
        if (err) return err;

        TreeDataType type_arg = TYPE_ERR;
        err = check_type_arg(str, &type_arg);
        if (err || type_arg == TYPE_ERR) return err;

        err = write_arg(*node, vars, str, type_arg);
        if (err) return err;

        /////////////////////////////////////////////////////////
        int right_dep = 0;
        *buf = skip_spaces(*buf);
        if (!(type_arg == TYPE_OP && OPERATORS[(int)(*node)->value].ops_num == 1)) {
            err = tree_read_(&(*node)->right, vars, buf, &right_dep);
            if (err) return err;
        } else {
            *buf = skip_word(*buf); 
        }

        (*dep) = (*node)->depth = MAX(left_dep, right_dep) + 1;

        if ((*node)->left) (*node)->left->parent = *node;
        if ((*node)->right) (*node)->right->parent = *node;

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

    while (**buf != '\0') {
        *buf = skip_spaces(*buf);

        char str[MAX_SIZE_INPUT] = "";
        err = get_arg(buf, str);
        if (err) return err;

        int is_var = 0;
        for (int i = 0; i < vars->count; i++) {
            if (strcmp(vars->var[i].name, str) == 0) {
                if (vars->var[i].valid == 1) return ERROR_DUPLICATE_VAR;

                *buf = skip_spaces(*buf);
                if (**buf != '=') return ERROR_INVALID_INPUT;
                (*buf)++;
                *buf = skip_spaces(*buf);

                char value[MAX_SIZE_INPUT] = "";
                err = get_arg(buf, value);
                if (err) return err;

                vars->var[i].value = atof(value);
                vars->var[i].valid = 1;

                is_var = 1;
                break;
            }
        }

        if (!is_var) {
            *buf = skip_word(*buf);
        }
    }

    vars->capacity = vars->count;
    vars->var = (Variable*)realloc(vars->var, vars->capacity * sizeof(Variable));
    if (!vars->var) return ERROR_ALLOC_FAIL;

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

static ErrorCode check_type_arg(char* str, TreeDataType* type_arg)
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

static ErrorCode write_arg(TreeNode* node, Variables* vars, char* str, TreeDataType type_arg)
{
    ErrorCode err = ERROR_NO;

    node->type_value = type_arg;
    if (type_arg == TYPE_OP) {
        for (int i = 0; i < COUNT_OPs; i++) {
            if (strcmp(OPERATORS[i].name, str) == 0) {
                node->value = OPERATORS[i].type_op;
                break;
            }
        }
    }

    if (type_arg == TYPE_NUM)
        node->value = atof(str);

    if (type_arg == TYPE_VAR) {
        int was = 0;
        for (int i = 0; i < vars->count; i++) {
            if (strcmp(vars->var[i].name, str) == 0) {
                node->value = i;
                was = 1;
                break;
            }
        }
        if (!was) {
            node->value = vars->count;
            vars->var[vars->count].name = strdup(str);
            if (!vars->var[vars->count].name) return ERROR_STRDUP;
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
        if (strcmp(OPERATORS[i].name, str) == 0) {
            (*is_oper) = 1;
            break;
        }
    }
    
    return ERROR_NO;
}

static ErrorCode is_number(char* str, int* is_num)
{
    int i = 0;
    int count_dot = 0;
    int count_minus = 0;
    while (str[i] != '\0' && !isspace(str[i])) {
        if (str[i] == '-') {
            count_minus = i;
        } else if (str[i] == '.') {
            count_dot++;
        } else if (str[i] < '0' || str[i] > '9') {
            (*is_num) = 0;
            return ERROR_NO;
        }
        if (count_dot > 1 || (count_dot == 1 && i == 0) || count_minus > 0)
            return ERROR_READ_INPUT;
        i++;
    }
    (*is_num) = 1;

    return ERROR_NO;
}

static ErrorCode is_variable(char* str, int* is_var)
{
    int i = 0;
    while (str[i] != '\0' && !isspace(str[i])) {
        for (int j = 0; j < (int)sizeof(BANNED_SYMBOLS); j++) {
            if (str[i] == BANNED_SYMBOLS[j]) {
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
    vars->var = (Variable*)realloc(vars->var, vars->capacity * sizeof(Variable));
    if (!vars->var) return ERROR_ALLOC_FAIL;

    for (int i = vars->count; i < vars->capacity; i++)
        vars->var[i].valid = 0;

    return ERROR_NO;
}