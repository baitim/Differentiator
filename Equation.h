#ifndef EQUATION_H
#define EQUATION_H

#include "Errors.h"
#include "Node.h"

struct OutputInfo;

struct Equation {
    char* name;
    EquationNode* root;
    Variables* variables;
    OutputInfo* output_info;
};

ErrorCode equation_new      (Equation** equation, const char*  equation_name);
ErrorCode equation_delete   (Equation* equation);
ErrorCode equation_verify   (EquationNode* node);
ErrorCode equation_copy     (Equation* equation, const char* name, Equation** new_equation);

#endif // EQUATION_H