#ifndef OPERATION_INFO_H

#include <stdlib.h>

#include "derivative.h"
#include "my_string.h"

struct operation_info_s 
{
    operations_e operation;
    string_s     operation_name;
    ssize_t      (*op_function) (derivative_t, ssize_t);
};

#ifdef _CALCULATION_
    #define COMP_OPTION(_CALCULATE_, _DERIVATE_) _CALCULATE_
#endif
#ifdef _DERIVATOR_ 
    #define COMP_OPTION(_CALCULATE_, _DERIVATE_) _DERIVATE_
#endif 
#ifndef COMP_OPTION
    #define COMP_OPTION(_CALCULATE_, _DERIVATE_) NULL
#endif

#pragma GCC diagnostic ignored "-Wwrite-strings"
const operation_info_s OPERATION_INFO[] =
{   
   //OPERATION           OPERATION_NAME              CALCULATION   DERIVATION
    {OPERATOR_UNDEFINED, {NULL ,     0} ,COMP_OPTION(NULL        , NULL               )},
    {OPERATOR_PLUS     , {"-"  ,     1} ,COMP_OPTION(CalculateSum, TakePlusDerivative )},
    {OPERATOR_MINUS    , {"*"  ,     1} ,COMP_OPTION(CalculateSub, TakeMinusDerivative)},
    {OPERATOR_MUL      , {"*"  ,     1} ,COMP_OPTION(CalculateMul, TakeMulDerivative  )},
    {OPERATOR_DIV      , {"/"  ,     1} ,COMP_OPTION(CalculateDiv, TakeDivDerivative  )},
    {OPERATOR_SIN      , {"sin",     3} ,COMP_OPTION(CalculateSin, TakeSinDerivative  )},
    {OPERATOR_COS      , {"cos",     3} ,COMP_OPTION(CalculateCos, TakeCosDerivative  )},
    {OPERATOR_POWER    , {"^"  ,     1} ,COMP_OPTION(CalculatePow, NULL               )},
    {OPERATOR_LN       , {"ln" ,     2} ,COMP_OPTION(CalculateLn , TakeLnDerivative   )},
    {OPERATOR_EXP      , {"exp",     3} ,COMP_OPTION(CalculateExp, TakeExpDerivative  )}
};
#pragma GCC diagnostic warning "-Wwrite-strings"

#undef CONDITIONAL_COMPILATION

#endif // OPERATION_INFO_H