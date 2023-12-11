#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "ANSI_colors.h"
#include "Input.h"
#include "Math.h"

static ErrorCode equation_read_     (EquationNode** node, Variables* vars, 
                                     InputData* input_data);
static ErrorCode vars_read_         (Variables* vars, char* buf);
static ErrorCode fsize              (const char* name_file, size_t* size_file);
static ErrorCode read_word           (char** buf, char* str);
static ErrorCode read_number         (char** buf, double* value);
static char* skip_spaces            (char* str);
static void skip_spaces_buf         (InputData* input_data);
static char* skip_word              (char* str);
static ErrorCode vars_increase_cap  (Variables* vars);
static ErrorCode get_number         (InputData* input_data, EquationNode** node);
static ErrorCode get_id             (InputData* input_data, EquationNode** node);
static ErrorCode get_primary        (InputData* input_data, EquationNode** node, Variables* vars);
static ErrorCode get_pow            (InputData* input_data, EquationNode** node, Variables* vars);
static ErrorCode get_mul            (InputData* input_data, EquationNode** node, Variables* vars);
static ErrorCode get_equation       (InputData* input_data, EquationNode** node, Variables* vars);

ErrorCode equation_get_val_vars(Equation* equation)
{
    if (!equation) return ERROR_INVALID_TREE;

    ErrorCode err = equation_verify(equation->root);
    if (err) return err;

    for (size_t i = 0; i < equation->variables->count; i++) {
        if (!equation->variables->var[i].valid) {
            printf(print_lcyan("You should input variables:\n"));
            break;
        }
    }
                
    for (size_t i = 0; i < equation->variables->count; i++)
        input_var(equation->variables, i);

    printf(print_lcyan("All variables have value\n"));

    return equation_verify(equation->root);
}

ErrorCode equation_get_num_var(Equation* equation, int* num_var)
{
    if (!equation) return ERROR_INVALID_TREE;

    ErrorCode err = equation_verify(equation->root);
    if (err) return err;

    if (equation->variables->count == 0) {
        *num_var = 0;
        return ERROR_NO;
    }

    printf(print_lcyan("You can choose this variables:\n"));
    for (size_t i = 0; i < equation->variables->count; i++)
        printf(print_lcyan("\t· %s\n"), equation->variables->var[i].name);

    char name_var[MAX_SIZE_VAR] = "";
    while (1) {
        printf(print_lcyan("Input name of variable: "));
        int count_read = scanf("%s", name_var);

        if (count_read != 1) {
            clean_stdin();
            printf(print_lred("Wrong argument, you should input name of existing variable\n"));
        }

        int exist = 0;
        for (size_t i = 0; i < equation->variables->count; i++) {
            if (strcmp(equation->variables->var[i].name, name_var) == 0) {
                *num_var = (int)i;
                exist = 1;
            }
        }

        if (exist == 1 && count_read == 1) break;

        clean_stdin();
        printf(print_lred("Wrong argument, you should input name of existing variable\n"));
    }

    return equation_verify(equation->root);
}

ErrorCode equation_read(Equation* equation, InputData* input_data)
{
    if (!equation) return ERROR_INVALID_TREE;

    ErrorCode err = equation_read_(&equation->root, equation->variables, input_data);
    if (err) return err;

    err = vars_read_(equation->variables, input_data->buf + input_data->index);
    if (err) return err;

    return ERROR_NO;
}

ErrorCode input_var(Variables* vars, size_t number_var)
{
    assert(vars);
    ErrorCode err = ERROR_NO;

    if (!vars->var[number_var].valid) {
        printf(print_lcyan("Input value of %s: "), vars->var[number_var].name);
        int count_read = scanf("%lf", &vars->var[number_var].value);
        while (count_read != 1) {
            err = clean_stdin();
            if (err) return err;
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

static ErrorCode equation_read_(EquationNode** node, Variables* vars, 
                                InputData* input_data)
{
    skip_spaces_buf(input_data);

    ErrorCode err = ERROR_NO;

    err = get_equation(input_data, node, vars);
    if (err) return err;
    skip_spaces_buf(input_data);

    return ERROR_NO;
}

static ErrorCode vars_read_(Variables* vars, char* buf)
{
    assert(buf);

    ErrorCode err = ERROR_NO;

    while (*buf != '\0') {
        buf = skip_spaces(buf);

        char str[MAX_SIZE_INPUT] = "";
        err = read_word(&buf, str);
        if (err) return err;

        int is_var = 0;
        for (size_t i = 0; i < vars->count; i++) {
            if (strcmp(vars->var[i].name, str) == 0) {
                if (vars->var[i].valid == 1) return ERROR_DUPLICATE_VAR;

                buf = skip_spaces(buf);
                if (*buf != '=') return ERROR_INVALID_INPUT;
                buf++;
                buf = skip_spaces(buf);

                double value = 0;
                err = read_number(&buf, &value);
                if (err) return err;

                vars->var[i].value = value;
                vars->var[i].valid = 1;

                is_var = 1;
                break;
            }
        }

        if (!is_var) {
            buf = skip_word(buf);
        }
    }

    vars->capacity = vars->count;
    if (vars->capacity > 0) {
        vars->var = (Variable*)realloc(vars->var, vars->capacity * sizeof(Variable));
        if (!vars->var) return ERROR_ALLOC_FAIL;
    }

    return ERROR_NO;
}

ErrorCode input_data_ctor(InputData** input_data, char* name_input_file)
{
    ErrorCode err = ERROR_NO;

    char* file_buffer = nullptr;
    err = file_to_buf(name_input_file, &file_buffer);
    if (err) return err;

    *input_data = (InputData*)calloc(1, sizeof(InputData));
    if(!(*input_data)) return ERROR_ALLOC_FAIL;
    (*input_data)->buf = (char*)calloc(MAX_SIZE_INPUT, sizeof(char));
    if(!(*input_data)->buf) return ERROR_ALLOC_FAIL;
    (*input_data)->buf = (char*)memcpy((*input_data)->buf, file_buffer, strlen(file_buffer));
    if(!(*input_data)->buf) return ERROR_ALLOC_FAIL;
    (*input_data)->index = 0;

    free(file_buffer);
    return ERROR_NO;
}

ErrorCode input_data_dtor(InputData** input_data)
{
    free((*input_data)->buf);
    (*input_data)->index = -POISON_VALUE;
    free(*input_data);
    return ERROR_NO;
}

static ErrorCode read_word(char** buf, char* str)
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

static ErrorCode read_number(char** buf, double* value)
{
    assert(buf);
    assert(*buf);

    char str[MAX_SIZE_INPUT] = "";
    ErrorCode err = read_word(buf, str);
    if (err) return err;

    *value = atof(str);

    return ERROR_NO;
}

static char* skip_spaces(char* str)
{
    assert(str);
    while (isspace(*str))
        str++;
    return str;
}

static void skip_spaces_buf(InputData* input_data)
{
    assert(input_data);
    while (isspace(input_data->buf[input_data->index]))
        input_data->index++;
    return;
}

static char* skip_word(char* str)
{
    while (*str != '\0' && !isspace(*str))
        str++;
    return str;
}

static ErrorCode get_number(InputData* input_data, EquationNode** node)
{
    ErrorCode err = ERROR_NO;
    skip_spaces_buf(input_data);
    double z = 1;
    double val = 0;
    if (input_data->buf[input_data->index] == '-') z = -1;
    while (input_data->buf[input_data->index] >= '0' && input_data->buf[input_data->index] <= '9') {
        val = val * 10 + input_data->buf[input_data->index] - '0';
        input_data->index++;
    }
    double div = 0.1;
    if (input_data->buf[input_data->index] == '.') {
        input_data->index++;
        while (input_data->buf[input_data->index] >= '0' && input_data->buf[input_data->index] <= '9') {
            val = val + (input_data->buf[input_data->index] - '0') * div;
            div /= 10;
            input_data->index++;
        }
    }
    skip_spaces_buf(input_data);
    err = node_init_num(node, val * z);
    return err;
}

static ErrorCode get_id(InputData* input_data, char** id)
{
    assert(input_data);

    ErrorCode err = ERROR_NO;
    skip_spaces_buf(input_data);
    *id = (char*)calloc(MAX_SIZE_INPUT, sizeof(char));
    if(!(*id)) return ERROR_ALLOC_FAIL;

    int ind = 0;
    while (!isspace(input_data->buf[input_data->index])) {
        (*id)[ind] = input_data->buf[input_data->index];
        input_data->index++;
        ind++;
    }

    skip_spaces_buf(input_data);
    return err;
}

static ErrorCode get_primary(InputData* input_data, EquationNode** node, Variables* vars)
{
    ErrorCode err = ERROR_NO;
    skip_spaces_buf(input_data);

    int start_index = input_data->index;
    char *id = nullptr;
    err = get_id(input_data, &id);
    if (err) return err;

    if (input_data->buf[input_data->index] == '(') {
        input_data->index++;
        err = get_equation(input_data, node, vars);
        if (err) return err;
        skip_spaces_buf(input_data);
        input_data->index++;

        for (int i = 0; i < COUNT_OPs; i++) {
            if (strcmp(id, OPERATORS[i].name) == 0) {
                EquationNode* node_copy_ = nullptr;
                err = node_copy(*node, &node_copy_);
                if (err) return err;
                err = node_insert_op(node, OPERATORS[i].type_op, node_copy_, nullptr);
                if (err) return err;
                node_delete(&node_copy_);
                free(id);
                return err;
            }
        }

        for (EquationNode* np = *node; np; np = np->parent) {
            size_t depth = 0;
            err = node_get_depth(np, &depth);
            if (err) return err;
            np->depth = depth;
        }
        free(id);
        return err;
    }

    input_data->index = start_index;

    for (EquationNode* np = *node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }
    err = get_number(input_data, node);

    free(id);
    return err;
}

static ErrorCode get_pow(InputData* input_data, EquationNode** node, Variables* vars)
{
    skip_spaces_buf(input_data);
    ErrorCode err = get_primary(input_data, node, vars);
    if (err) return err;
    skip_spaces_buf(input_data);
    while (input_data->buf[input_data->index] == '^') {
        input_data->index++;
        EquationNode* node_right = nullptr;
        err = get_primary(input_data, &node_right, vars);
        if (err) return err;
        EquationNode* node_left = nullptr;
        err = node_copy(*node, &node_left);
        if (err) return err;
        skip_spaces_buf(input_data);
        err = node_insert_op(node, OP_POW, node_left, node_right);
        if (err) return err;

        node_delete(&node_right);
        node_delete(&node_left);
    }
    for (EquationNode* np = *node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }
    skip_spaces_buf(input_data);
    return ERROR_NO;
}

static ErrorCode get_mul(InputData* input_data, EquationNode** node, Variables* vars)
{
    skip_spaces_buf(input_data);
    ErrorCode err = get_pow(input_data, node, vars);
    if (err) return err;
    skip_spaces_buf(input_data);
    while (input_data->buf[input_data->index] == '*' || input_data->buf[input_data->index] == '/') {
        int op = input_data->index;
        input_data->index++;
        skip_spaces_buf(input_data);
        EquationNode* node_right = nullptr;
        err = get_pow(input_data, &node_right, vars);
        if (err) return err;
        EquationNode* node_left = nullptr;
        err = node_copy(*node, &node_left);
        if (err) return err;
        switch (input_data->buf[op]) {
            case '*':
                err = node_insert_op(node, OP_MUL, node_left, node_right);
                if (err) return err;
                break;
            case '/':
                err = node_insert_op(node, OP_DIV, node_left, node_right);
                if (err) return err;
                break;
            default:
                assert(0);
        }
        node_delete(&node_right);
        node_delete(&node_left);
    }
    for (EquationNode* np = *node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }
    skip_spaces_buf(input_data);
    return ERROR_NO;
}

static ErrorCode get_equation(InputData* input_data, EquationNode** node, Variables* vars)
{
    skip_spaces_buf(input_data);
    ErrorCode err = get_mul(input_data, node, vars);
    if (err) return err;
    skip_spaces_buf(input_data);
    while (input_data->buf[input_data->index] == '+' || input_data->buf[input_data->index] == '-') {
        int op = input_data->index;
        input_data->index++;
        skip_spaces_buf(input_data);
        EquationNode* node_right = nullptr;
        err = get_mul(input_data, &node_right, vars);
        if (err) return err;
        EquationNode* node_left = nullptr;
        err = node_copy(*node, &node_left);
        if (err) return err;
        switch (input_data->buf[op]) {
            case '+':
                err = node_insert_op(node, OP_ADD, node_left, node_right);
                if (err) return err;
                break;
            case '-':
                err = node_insert_op(node, OP_SUB, node_left, node_right);
                if (err) return err;
                break;
            default:
                assert(0);
        }
        node_delete(&node_right);
        node_delete(&node_left);
    }
    for (EquationNode* np = *node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }
    skip_spaces_buf(input_data);
    return ERROR_NO;
}

ErrorCode file_to_buf(const char* name_file, char** buf)
{
    if (!name_file) return ERROR_OPEN_FILE;

    ErrorCode err = ERROR_NO;

    FILE* data_file = fopen(name_file, "r");
    if (!data_file) return ERROR_OPEN_FILE;

    size_t size_file = (size_t)-1;
    err = fsize(name_file, &size_file);
    if (err) return err;

    *buf = (char *)calloc(size_file, sizeof(char));
    if (!(*buf)) return ERROR_ALLOC_FAIL;

    size_t count_read = fread(*buf, sizeof(char), size_file, data_file);
    if (count_read != size_file - 1) return ERROR_READ_FILE;
    (*buf)[size_file - 1] = '\0';

    fclose(data_file);
    return err;
}

static ErrorCode fsize(const char* name_file, size_t* size_file) 
{
    assert(name_file);
    struct stat st = {};

    if (stat(name_file, &st) == 0) {
        (*size_file) = (size_t)st.st_size + 1;
        return ERROR_NO;
    }

    return ERROR_READ_FILE;
}

static ErrorCode vars_increase_cap(Variables* vars)
{
    assert(vars);

    vars->capacity = vars->capacity * MULTIPLIER_CAPACITY;
    vars->var = (Variable*)realloc(vars->var, vars->capacity * sizeof(Variable));
    if (!vars->var) return ERROR_ALLOC_FAIL;

    for (size_t i = vars->count; i < vars->capacity; i++) {
        vars->var[i].valid = 0;
        vars->var[i].value = (int)POISON_VALUE;
    }

    return ERROR_NO;
}