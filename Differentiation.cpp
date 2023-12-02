#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "ANSI_colors.h"
#include "Differentiation.h"
#include "Input.h"
#include "Math.h"
#include "Output.h"

static ErrorCode equation_diff_         (EquationNode* node, int num_var);
static ErrorCode equation_diff_report_  (Equation* equation, int num_var, FILE* dump_file);
static ErrorCode node_diff              (EquationNode* node, int num_var);
static ErrorCode var_diff               (EquationNode* node, int num_var);
static ErrorCode op_diff                (EquationNode* node, int num_var);

ErrorCode equation_diff(Equation* equation, int num_var)
{
    if (!equation) return ERROR_INVALID_TREE;

    ErrorCode err = ERROR_NO;

    err = equation_diff_(equation->root, num_var);
    if (err) return err;

    return ERROR_NO;
}

ErrorCode equation_diff_report(Equation* equation, int num_var)
{
    if (!equation) return ERROR_INVALID_TREE;

    ErrorCode err = equation_verify(equation->root);
    if (err) return err;

    char tex_path[MAX_SIZE_NAME_DUMP] = "";
    snprintf(tex_path, MAX_SIZE_NAME_DUMP, "%s/tex/%s%zu", equation->name, equation->name, 
                                            equation->output_info->number_tex_dump);

    char *name_tex_file = nullptr;
    err = make_name_file(tex_path, ".tex", &name_tex_file);
    if (err) return err;

    FILE* dump_file = fopen(name_tex_file, "w");
    if (!dump_file) {
        fprintf(stderr, "Error open file to dump\n");
        return ERROR_SYSTEM_COMMAND;
    }

    err = equation_verify(equation->root);
    if (err) return err;

    time_t mytime = time(NULL);
    struct tm *now = localtime(&mytime);

    fprintf(dump_file, "\\documentclass[a4paper,3pt]{article}\n"
                       "\\usepackage[T2A]{fontenc}\n\n"
                       "\\usepackage{amsmath}\n"
                       "\\DeclareMathOperator\\arcctan{arcctan}"
                       "\\author{Baidiusenov Timur}\n"
                       "\\title{%s}\n"
                       "\\date{Date: %d.%d.%d, Time %d:%d:%d}\n"
                       "\\begin{document}\n"
                       "\\maketitle\n",
                       equation->name,
                       now->tm_mday, now->tm_mon + 1, now->tm_year + 1900,
                       now->tm_hour, now->tm_min, now->tm_sec);

    err = equation_diff_report_(equation, num_var, dump_file);
    if (err) return err;

    fprintf(dump_file, "\\end{document}\n");
    fclose(dump_file);

    char pdf_path[MAX_SIZE_NAME_DUMP] = "";
    snprintf(pdf_path, MAX_SIZE_NAME_DUMP, "%s/pdf/%s%zu", equation->name, equation->name, 
                                            equation->output_info->number_tex_dump);

    char *name_aux_file = nullptr;
    err = make_name_file(pdf_path, ".aux", &name_aux_file);
    if (err) return err;
    char *name_tex_log_file = nullptr;
    err = make_name_file(pdf_path, ".log", &name_tex_log_file);
    if (err) return err;

    char command[MAX_SIZE_COMMAND] = "";
    snprintf(command, MAX_SIZE_COMMAND, "pdflatex -output-directory=%s/pdf %s ; "
                                        "rm %s ; rm %s", 
                                        equation->name, name_tex_file, 
                                        name_aux_file, name_tex_log_file);
    int sys = system(command);
    if (sys) return ERROR_SYSTEM_COMMAND;

    equation->output_info->number_tex_dump++;
    free(name_tex_file);
    free(name_aux_file);
    free(name_tex_log_file);
    return ERROR_NO;
}

static ErrorCode equation_diff_(EquationNode* node, int num_var)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = equation_verify(node);
    if (err) return err;

    err = node_diff(node, num_var);
    if (err) return err;

    return equation_verify(node);
}

static ErrorCode equation_diff_report_(Equation* equation, int num_var, FILE* dump_file)
{
    if (!equation) return ERROR_INVALID_TREE;

    ErrorCode err = ERROR_NO;

    fprintf(dump_file, "\\begin{equation}\n\t(");
    err = equation_node_dump_(equation->root, equation->variables, dump_file, 
                              BRANCH_ERR, TYPE_ERR, OP_ERR);
    if (err) return err;
    fprintf(dump_file, ")` \n\\end{equation}\n");

    err = node_diff(equation->root, num_var);
    if (err) return err;

    fprintf(dump_file, "%s:\n", tex_txt_data[rand() % COUNT_TEX_TXT_DATA]);

    fprintf(dump_file, "\\begin{equation}\n\t");
    err = equation_node_dump_(equation->root, equation->variables, dump_file, 
                              BRANCH_ERR, TYPE_ERR, OP_ERR);
    if (err) return err;
    fprintf(dump_file, "\n\\end{equation}\n");

    return ERROR_NO;
}

static ErrorCode node_diff(EquationNode* node, int num_var)
{
    assert(node);

    if (node->type_value == TYPE_NUM) {
        node->value = 0;
    } else if (node->type_value == TYPE_VAR) {
        var_diff(node, num_var);
    } else if (node->type_value == TYPE_OP) {
        op_diff(node, num_var);
    }

    return ERROR_NO;
}

static ErrorCode var_diff(EquationNode* node, int num_var)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = equation_verify(node);
    if (err) return err;

    if (!is_double_equal((int)node->value, num_var)) {
        node->type_value = TYPE_NUM;
        node->value = 0;
    } else {
        node->type_value = TYPE_NUM;
        node->value = 1;
    }

    return ERROR_NO;
}

static ErrorCode op_diff(EquationNode* node, int num_var)
{
    if (!node) return ERROR_INVALID_TREE;

    ErrorCode err = equation_verify(node);
    if (err) return err;

    assert(node);

    err = (*OPERATORS[(int)node->value].diff)(node, num_var);
    if (err) return err;

    return err;
}

ErrorCode err_diff(EquationNode* node, int num_var)
{
    assert(node);
    printf("Called err_diff with node = %p and num = %d\n", node, num_var);
    return ERROR_DIFFERENTIATION;
}

ErrorCode add_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    err = node_diff(node->left, num_var);
    if (err) return err;
    err = node_diff(node->right, num_var);
    if (err) return err;

    return ERROR_NO;
}

ErrorCode sub_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    err = node_diff(node->left, num_var);
    if (err) return err;
    err = node_diff(node->right, num_var);
    if (err) return err;

    return ERROR_NO;
}

ErrorCode mul_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    EquationNode* node_left_copy = nullptr;
    err = node_copy(node->left, &node_left_copy);
    if (err) return err;

    EquationNode* node_right_copy = nullptr;
    err = node_copy(node->right, &node_right_copy);
    if (err) return err;

    EquationNode* node_left_diff = nullptr;
    err = node_copy(node_left_copy, &node_left_diff);
    if (err) return err;
    err = node_diff(node_left_diff, num_var);
    if (err) return err;

    EquationNode* node_right_diff = nullptr;
    err = node_copy(node_right_copy, &node_right_diff);
    if (err) return err;
    err = node_diff(node_right_diff, num_var);
    if (err) return err;

    node->type_value =  TYPE_OP;
    node->value =       OP_ADD;
    err = node_insert_op(&node->left,  OP_MUL, node_left_diff,  node_right_copy);
    if (err) return err;
    err = node_insert_op(&node->right, OP_MUL, node_left_copy, node_right_diff);
    if (err) return err;
    
    for (EquationNode* np = node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }

    err = node_delete(&node_left_copy);
    if (err) return err;
    err = node_delete(&node_right_copy);
    if (err) return err;
    err = node_delete(&node_left_diff);
    if (err) return err;
    err = node_delete(&node_right_diff);
    if (err) return err;

    return ERROR_NO;
}

ErrorCode div_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    EquationNode* node_right_copy = nullptr;
    err = node_copy(node->right, &node_right_copy);
    if (err) return err;

    EquationNode* node_value_two = nullptr;
    err = node_init_num(&node_value_two, 2);
    if (err) return err;

    EquationNode* numerator = nullptr;
    err = node_copy(node, &numerator);
    if (err) return err;
    numerator->value = OP_MUL;
    err = node_diff(numerator, num_var);
    if (err) return err;
    numerator->value = OP_SUB;

    EquationNode* denominator = nullptr;
    err = node_insert_op(&denominator, OP_POW, node_right_copy, node_value_two);
    if (err) return err;

    err = node_insert_op(&node, OP_DIV, numerator, denominator);
    if (err) return err;
    
    for (EquationNode* np = node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }

    err = node_delete(&node_right_copy);
    if (err) return err;
    err = node_delete(&node_value_two);
    if (err) return err;
    err = node_delete(&numerator);
    if (err) return err;
    err = node_delete(&denominator);
    if (err) return err;
    
    return err;
}

ErrorCode pow_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    EquationNode* node_left_ln = nullptr;
    err = node_insert_op(&node_left_ln, OP_LN, node->left, nullptr);
    if (err) return err;

    EquationNode* node_degree = nullptr;
    err = node_insert_op(&node_degree, OP_MUL, node_left_ln, node->right);
    if (err) return err;

    EquationNode* node_value_exp = nullptr;
    err = node_init_num(&node_value_exp, M_E);
    if (err) return err;

    EquationNode* node_exp_pow = nullptr;
    err = node_insert_op(&node_exp_pow, OP_POW, node_value_exp, node_degree);
    if (err) return err;

    EquationNode* node_degree_diff = nullptr;
    err = node_copy(node_degree, &node_degree_diff);
    if (err) return err;
    err = node_diff(node_degree_diff, num_var);
    if (err) return err;

    err = node_insert_op(&node, OP_MUL, node_exp_pow, node_degree_diff);
    if (err) return err;

    for (EquationNode* np = node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }

    err = node_delete(&node_left_ln);
    if (err) return err;
    err = node_delete(&node_degree);
    if (err) return err;
    err = node_delete(&node_value_exp);
    if (err) return err;
    err = node_delete(&node_exp_pow);
    if (err) return err;
    err = node_delete(&node_degree_diff);
    if (err) return err;
    
    return err;
}

ErrorCode log_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    EquationNode* node_left_ln = nullptr;
    err = node_insert_op(&node_left_ln, OP_LN, node->left, nullptr);
    if (err) return err;

    EquationNode* node_right_ln = nullptr;
    err = node_insert_op(&node_right_ln, OP_LN, node->right, nullptr);
    if (err) return err;

    err = node_insert_op(&node, OP_DIV, node_right_ln, node_left_ln);
    if (err) return err;
    err = node_diff(node, num_var);
    if (err) return err;

    for (EquationNode* np = node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }

    err = node_delete(&node_left_ln);
    if (err) return err;
    err = node_delete(&node_right_ln);
    if (err) return err;
    
    return err;
}

ErrorCode ln_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    EquationNode* numerator = nullptr;
    err = node_copy(node->left, &numerator);
    if (err) return err;
    err = node_diff(numerator, num_var);
    if (err) return err;

    EquationNode* denominator = nullptr;
    err = node_copy(node->left, &denominator);
    if (err) return err;

    err = node_insert_op(&node, OP_DIV, numerator, denominator);
    if (err) return err;

    for (EquationNode* np = node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }

    err = node_delete(&numerator);
    if (err) return err;
    err = node_delete(&denominator);
    if (err) return err;
    
    return err;
}

ErrorCode sqrt_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    EquationNode* numerator = nullptr;
    err = node_copy(node->left, &numerator);
    if (err) return err;
    err = node_diff(numerator, num_var);
    if (err) return err;

    EquationNode* node_copy_ = nullptr;
    err = node_copy(node, &node_copy_);
    if (err) return err;

    EquationNode* node_value_two = nullptr;
    err = node_init_num(&node_value_two, 2);
    if (err) return err;

    EquationNode* denominator = nullptr;
    err = node_insert_op(&denominator, OP_MUL, node_value_two, node_copy_);
    if (err) return err;

    err = node_insert_op(&node, OP_DIV, numerator, denominator);
    if (err) return err;
    
    for (EquationNode* np = node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }

    err = node_delete(&numerator);
    if (err) return err;
    err = node_delete(&node_copy_);
    if (err) return err;
    err = node_delete(&node_value_two);
    if (err) return err;
    err = node_delete(&denominator);
    if (err) return err;

    return err;
}

ErrorCode sin_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    EquationNode* node_copy_ = nullptr;
    err = node_copy(node, &node_copy_);
    if (err) return err;
    node_copy_->value = OP_COS;

    EquationNode* node_left_diff = nullptr;
    err = node_copy(node_copy_->left, &node_left_diff);
    if (err) return err;
    err = node_diff(node_left_diff, num_var);
    if (err) return err;

    err = node_insert_op(&node, OP_MUL, node_left_diff, node_copy_);
    if (err) return err;

    for (EquationNode* np = node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }

    err = node_delete(&node_copy_);
    if (err) return err;
    err = node_delete(&node_left_diff);
    if (err) return err;
    
    return err;
}

ErrorCode cos_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    EquationNode* node_value_minus_one = nullptr;
    err = node_init_num(&node_value_minus_one, -1);
    if (err) return err;

    EquationNode* new_node_right = nullptr;
    err = node_copy(node, &new_node_right);
    if (err) return err;
    new_node_right->value = OP_SIN;

    EquationNode* new_node_right_diff = nullptr;
    err = node_copy(node->left, &new_node_right_diff);
    if (err) return err;
    err = node_diff(new_node_right_diff, num_var);
    if (err) return err;

    EquationNode* node_right = nullptr;
    err = node_insert_op(&node_right, OP_MUL, new_node_right_diff, new_node_right);
    if (err) return err;

    err = node_insert_op(&node, OP_MUL, node_value_minus_one, node_right);
    if (err) return err;

    for (EquationNode* np = node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }

    err = node_delete(&node_value_minus_one);
    if (err) return err;
    err = node_delete(&new_node_right);
    if (err) return err;
    err = node_delete(&new_node_right_diff);
    if (err) return err;
    err = node_delete(&node_right);
    if (err) return err;

    return err;
}

ErrorCode tg_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    EquationNode* numerator = nullptr;
    err = node_copy(node->left, &numerator);
    if (err) return err;
    err = node_diff(numerator, num_var);
    if (err) return err;

    EquationNode* node_copy_ = nullptr;
    err = node_copy(node, &node_copy_);
    if (err) return err;
    node_copy_->value = OP_COS;

    EquationNode* node_value_two = nullptr;
    err = node_init_num(&node_value_two, 2);
    if (err) return err;

    EquationNode* denominator = nullptr;
    err = node_insert_op(&denominator, OP_POW, node_copy_, node_value_two);
    if (err) return err;

    err = node_insert_op(&node, OP_DIV, numerator, denominator);
    if (err) return err;
    
    for (EquationNode* np = node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }

    err = node_delete(&numerator);
    if (err) return err;
    err = node_delete(&node_copy_);
    if (err) return err;
    err = node_delete(&node_value_two);
    if (err) return err;
    err = node_delete(&denominator);
    if (err) return err;

    return err;
}

ErrorCode ctg_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    EquationNode* node_value_minus_one = nullptr;
    err = node_init_num(&node_value_minus_one, -1);
    if (err) return err;

    EquationNode* numerator = nullptr;
    err = node_copy(node->left, &numerator);
    if (err) return err;
    err = node_diff(numerator, num_var);
    if (err) return err;

    EquationNode* node_copy_ = nullptr;
    err = node_copy(node, &node_copy_);
    if (err) return err;
    node_copy_->value = OP_SIN;

    EquationNode* node_value_two = nullptr;
    err = node_init_num(&node_value_two, 2);
    if (err) return err;

    EquationNode* denominator = nullptr;
    err = node_insert_op(&denominator, OP_POW, node_copy_, node_value_two);
    if (err) return err;

    err = node_insert_op(&node, OP_DIV, numerator, denominator);
    if (err) return err;
    err = node_delete(&node_copy_);
    if (err) return err;
    err = node_copy(node, &node_copy_);
    if (err) return err;

    err = node_insert_op(&node, OP_MUL, node_value_minus_one, node_copy_);
    if (err) return err;
    
    for (EquationNode* np = node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }

    err = node_delete(&node_value_minus_one);
    if (err) return err;
    err = node_delete(&numerator);
    if (err) return err;
    err = node_delete(&node_copy_);
    if (err) return err;
    err = node_delete(&node_value_two);
    if (err) return err;
    err = node_delete(&denominator);
    if (err) return err;

    return err;
}

ErrorCode asin_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    EquationNode* numerator = nullptr;
    err = node_copy(node->left, &numerator);
    if (err) return err;
    err = node_diff(numerator, num_var);
    if (err) return err;

    EquationNode* node_value_one = nullptr;
    err = node_init_num(&node_value_one, 1);
    if (err) return err;

    EquationNode* node_value_two = nullptr;
    err = node_init_num(&node_value_two, 2);
    if (err) return err;

    EquationNode* node_pow_two = nullptr;
    err = node_insert_op(&node_pow_two, OP_POW, node->left, node_value_two);
    if (err) return err;

    EquationNode* denominator = nullptr;
    err = node_insert_op(&denominator, OP_SUB, node_value_one, node_pow_two);
    if (err) return err;
    EquationNode* denominator_copy = nullptr;
    err = node_copy(denominator, &denominator_copy);
    if (err) return err;
    err = node_insert_op(&denominator, OP_SQRT, denominator_copy, nullptr);
    if (err) return err;

    err = node_insert_op(&node, OP_DIV, numerator, denominator);
    if (err) return err;
    
    for (EquationNode* np = node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }

    err = node_delete(&numerator);
    if (err) return err;
    err = node_delete(&node_value_one);
    if (err) return err;
    err = node_delete(&node_value_two);
    if (err) return err;
    err = node_delete(&node_pow_two);
    if (err) return err;
    err = node_delete(&denominator);
    if (err) return err;
    err = node_delete(&denominator_copy);
    if (err) return err;

    return err;
}

ErrorCode acos_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    EquationNode* numerator = nullptr;
    err = node_copy(node->left, &numerator);
    if (err) return err;
    err = node_diff(numerator, num_var);
    if (err) return err;

    EquationNode* node_value_one = nullptr;
    err = node_init_num(&node_value_one, 1);
    if (err) return err;

    EquationNode* node_value_two = nullptr;
    err = node_init_num(&node_value_two, 2);
    if (err) return err;

    EquationNode* node_pow_two = nullptr;
    err = node_insert_op(&node_pow_two, OP_POW, node->left, node_value_two);
    if (err) return err;

    EquationNode* denominator = nullptr;
    err = node_insert_op(&denominator, OP_SUB, node_value_one, node_pow_two);
    if (err) return err;
    EquationNode* denominator_copy = nullptr;
    err = node_copy(denominator, &denominator_copy);
    if (err) return err;
    err = node_insert_op(&denominator, OP_SQRT, denominator_copy, nullptr);
    if (err) return err;

    err = node_insert_op(&node, OP_DIV, numerator, denominator);
    if (err) return err;

    EquationNode* node_value_minus_one = nullptr;
    err = node_init_num(&node_value_minus_one, -1);
    if (err) return err;
    EquationNode* node_copy_ = nullptr;
    err = node_copy(node, &node_copy_);
    if (err) return err;
    err = node_insert_op(&node, OP_MUL, node_value_minus_one, node_copy_);
    if (err) return err;
    
    for (EquationNode* np = node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }

    err = node_delete(&numerator);
    if (err) return err;
    err = node_delete(&node_value_one);
    if (err) return err;
    err = node_delete(&node_value_two);
    if (err) return err;
    err = node_delete(&node_pow_two);
    if (err) return err;
    err = node_delete(&denominator);
    if (err) return err;
    err = node_delete(&denominator_copy);
    if (err) return err;
    err = node_delete(&node_value_minus_one);
    if (err) return err;
    err = node_delete(&node_copy_);
    if (err) return err;

    return err;
}

ErrorCode atg_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    EquationNode* numerator = nullptr;
    err = node_copy(node->left, &numerator);
    if (err) return err;
    err = node_diff(numerator, num_var);
    if (err) return err;

    EquationNode* node_value_one = nullptr;
    err = node_init_num(&node_value_one, 1);
    if (err) return err;

    EquationNode* node_value_two = nullptr;
    err = node_init_num(&node_value_two, 2);
    if (err) return err;

    EquationNode* node_pow_two = nullptr;
    err = node_insert_op(&node_pow_two, OP_POW, node->left, node_value_two);
    if (err) return err;

    EquationNode* denominator = nullptr;
    err = node_insert_op(&denominator, OP_ADD, node_value_one, node_pow_two);
    if (err) return err;

    err = node_insert_op(&node, OP_DIV, numerator, denominator);
    if (err) return err;
    
    for (EquationNode* np = node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }

    err = node_delete(&numerator);
    if (err) return err;
    err = node_delete(&node_value_one);
    if (err) return err;
    err = node_delete(&node_value_two);
    if (err) return err;
    err = node_delete(&node_pow_two);
    if (err) return err;
    err = node_delete(&denominator);
    if (err) return err;

    return err;
}

ErrorCode actg_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    EquationNode* numerator = nullptr;
    err = node_copy(node->left, &numerator);
    if (err) return err;
    err = node_diff(numerator, num_var);
    if (err) return err;

    EquationNode* node_value_minus_one = nullptr;
    err = node_init_num(&node_value_minus_one, -1);
    if (err) return err;

    EquationNode* node_value_one = nullptr;
    err = node_init_num(&node_value_one, 1);
    if (err) return err;

    EquationNode* node_value_two = nullptr;
    err = node_init_num(&node_value_two, 2);
    if (err) return err;

    EquationNode* node_pow_two = nullptr;
    err = node_insert_op(&node_pow_two, OP_POW, node->left, node_value_two);
    if (err) return err;

    EquationNode* denominator = nullptr;
    err = node_insert_op(&denominator, OP_ADD, node_value_one, node_pow_two);
    if (err) return err;

    err = node_insert_op(&node, OP_DIV, numerator, denominator);
    if (err) return err;
    EquationNode* node_copy_ = nullptr;
    err = node_copy(node, &node_copy_);
    if (err) return err;
    err = node_insert_op(&node, OP_MUL, node_value_minus_one, node_copy_);
    if (err) return err;
    
    for (EquationNode* np = node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }

    err = node_delete(&numerator);
    if (err) return err;
    err = node_delete(&node_value_minus_one);
    if (err) return err;
    err = node_delete(&node_value_one);
    if (err) return err;
    err = node_delete(&node_value_two);
    if (err) return err;
    err = node_delete(&node_pow_two);
    if (err) return err;
    err = node_delete(&denominator);
    if (err) return err;
    err = node_delete(&node_copy_);
    if (err) return err;

    return err;
}

ErrorCode sh_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    EquationNode* node_copy_ = nullptr;
    err = node_copy(node, &node_copy_);
    if (err) return err;
    node_copy_->value = OP_CH;

    EquationNode* node_left_diff = nullptr;
    err = node_copy(node_copy_->left, &node_left_diff);
    if (err) return err;
    err = node_diff(node_left_diff, num_var);
    if (err) return err;

    err = node_insert_op(&node, OP_MUL, node_left_diff, node_copy_);
    if (err) return err;

    for (EquationNode* np = node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }

    err = node_delete(&node_copy_);
    if (err) return err;
    err = node_delete(&node_left_diff);
    if (err) return err;
    
    return err;
}

ErrorCode ch_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    EquationNode* node_copy_ = nullptr;
    err = node_copy(node, &node_copy_);
    if (err) return err;
    node_copy_->value = OP_SH;

    EquationNode* node_left_diff = nullptr;
    err = node_copy(node_copy_->left, &node_left_diff);
    if (err) return err;
    err = node_diff(node_left_diff, num_var);
    if (err) return err;

    err = node_insert_op(&node, OP_MUL, node_left_diff, node_copy_);
    if (err) return err;

    for (EquationNode* np = node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }

    err = node_delete(&node_copy_);
    if (err) return err;
    err = node_delete(&node_left_diff);
    if (err) return err;

    return err;
}

ErrorCode th_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    EquationNode* numerator = nullptr;
    err = node_copy(node->left, &numerator);
    if (err) return err;
    err = node_diff(numerator, num_var);
    if (err) return err;

    EquationNode* node_copy_ = nullptr;
    err = node_copy(node, &node_copy_);
    if (err) return err;
    node_copy_->value = OP_CH;

    EquationNode* node_value_two = nullptr;
    err = node_init_num(&node_value_two, 2);
    if (err) return err;

    EquationNode* denominator = nullptr;
    err = node_insert_op(&denominator, OP_POW, node_copy_, node_value_two);
    if (err) return err;

    err = node_insert_op(&node, OP_DIV, numerator, denominator);
    if (err) return err;
    
    for (EquationNode* np = node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }

    err = node_delete(&numerator);
    if (err) return err;
    err = node_delete(&node_copy_);
    if (err) return err;
    err = node_delete(&node_value_two);
    if (err) return err;
    err = node_delete(&denominator);
    if (err) return err;

    return err;
}

ErrorCode cth_diff(EquationNode* node, int num_var)
{
    assert(node);

    ErrorCode err = ERROR_NO;

    EquationNode* node_value_minus_one = nullptr;
    err = node_init_num(&node_value_minus_one, -1);
    if (err) return err;

    EquationNode* numerator = nullptr;
    err = node_copy(node->left, &numerator);
    if (err) return err;
    err = node_diff(numerator, num_var);
    if (err) return err;

    EquationNode* node_copy_ = nullptr;
    err = node_copy(node, &node_copy_);
    if (err) return err;
    node_copy_->value = OP_SH;

    EquationNode* node_value_two = nullptr;
    err = node_init_num(&node_value_two, 2);
    if (err) return err;

    EquationNode* denominator = nullptr;
    err = node_insert_op(&denominator, OP_POW, node_copy_, node_value_two);
    if (err) return err;

    err = node_insert_op(&node, OP_DIV, numerator, denominator);
    if (err) return err;
    err = node_delete(&node_copy_);
    if (err) return err;
    err = node_copy(node, &node_copy_);
    if (err) return err;

    err = node_insert_op(&node, OP_MUL, node_value_minus_one, node_copy_);
    if (err) return err;
    
    for (EquationNode* np = node; np; np = np->parent) {
        size_t depth = 0;
        err = node_get_depth(np, &depth);
        if (err) return err;
        np->depth = depth;
    }

    err = node_delete(&node_value_minus_one);
    if (err) return err;
    err = node_delete(&numerator);
    if (err) return err;
    err = node_delete(&node_copy_);
    if (err) return err;
    err = node_delete(&node_value_two);
    if (err) return err;
    err = node_delete(&denominator);
    if (err) return err;

    return err;
}