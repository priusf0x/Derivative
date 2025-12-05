#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "my_string.h"
#include "name_space.h"

// ============================ ENUMS AND STRUCTS =============================

enum expression_type_e
{
    EXPRESSION_TYPE_UNDEFINED,
    EXPRESSION_TYPE_CONST,
    EXPRESSION_TYPE_VAR,
    EXPRESSION_TYPE_OPERATOR
};

enum operations_e 
{
    OPERATOR_UNDEFINED = 0,
    OPERATOR_PLUS      = 1,
    OPERATOR_MINUS     = 2,
    OPERATOR_MUL       = 3,
    OPERATOR_DIV       = 4,
    OPERATOR_SIN       = 5,
    OPERATOR_COS       = 6,
    OPERATOR_POWER     = 7,
    OPERATOR_LN        = 8,
    OPERATOR_EXP       = 9,
    OPERATOR_TG        = 10,
    OPERATOR_CTG       = 11
};

struct variable_s
{
    string_s variable_name;  
    size_t   name_table_index;
};

union expression_u
{
    double       constant;
    operations_e operation;
    variable_s   variable;     
};

struct expression_s
{
    expression_u      expression;          
    expression_type_e expression_type;
};

// ============================================================================

#endif // EXPRESSION_H