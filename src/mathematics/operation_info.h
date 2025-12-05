#ifndef OPERATION_INFO_H

#include <stdlib.h>

#include "derivative.h"
#include "my_string.h"

struct operation_info_s 
{
    operations_e operation;
    string_s     operation_name;
    ssize_t      (*op_function) (derivative_t, ssize_t);
    size_t       argc;
    const char*  latex_dump_name;
    size_t       priority;
};

#ifdef _CALCULATION_
    #define COMP_OPTION(_CALCULATE_, _DERIVATE_) _CALCULATE_
#endif
#ifdef _DERIVATOR_ 
    #define COMP_OPTION(_CALCULATE_, _DERIVATE_) _DERIVATE_
#endif 
#ifndef _READER_
    #define TURN_OFF(_STRING_SRC_, _STRING_SIZE_) {_STRING_SRC_, _STRING_SIZE_}
#else
    #define TURN_OFF(_STRING_SRC_, _STRING_SIZE_) {NULL,  0}
#endif
#ifndef COMP_OPTION
    #define COMP_OPTION(_CALCULATE_, _DERIVATE_) NULL
#endif

#pragma GCC diagnostic ignored "-Wwrite-strings"
const operation_info_s OPERATION_INFO[] =
{
   //OPERATION           OPERATION_NAME                        CALCULATION   DERIVATION
    {OPERATOR_UNDEFINED, TURN_OFF( NULL ,     0)  ,COMP_OPTION(NULL        , NULL              ), 0, "undefined", 0},
    {OPERATOR_PLUS     , TURN_OFF( "-"  ,     1)  ,COMP_OPTION(SimplifySum, TakePlusDerivative ), 2, "+"        , 1},
    {OPERATOR_MINUS    , TURN_OFF( "*"  ,     1)  ,COMP_OPTION(SimplifySub, TakeMinusDerivative), 2, "-"        , 1},
    {OPERATOR_MUL      , TURN_OFF( "*"  ,     1)  ,COMP_OPTION(SimplifyMul, TakeMulDerivative  ), 2, "*"        , 2},
    {OPERATOR_DIV      , TURN_OFF( "/"  ,     1)  ,COMP_OPTION(SimplifyDiv, TakeDivDerivative  ), 2, "\\over"   , 2},
    {OPERATOR_SIN      ,          {"sin",     3}  ,COMP_OPTION(SimplifySin, TakeSinDerivative  ), 1, "sin"      , 3},
    {OPERATOR_COS      ,          {"cos",     3}  ,COMP_OPTION(SimplifyCos, TakeCosDerivative  ), 1, "cos"      , 3},
    {OPERATOR_POWER    , TURN_OFF( "^"  ,     1)  ,COMP_OPTION(SimplifyPow, TakePowDerivative  ), 2, "^"        , 3},
    {OPERATOR_LN       ,          {"ln" ,     2}  ,COMP_OPTION(SimplifyLn , TakeLnDerivative   ), 1, "ln"       , 3},
    {OPERATOR_EXP      ,          {"exp",     3}  ,COMP_OPTION(SimplifyExp, TakeExpDerivative  ), 1, "exp"      , 3},
    {OPERATOR_TG       ,          {"tg" ,     2}  ,COMP_OPTION(SimplifyTg,  TakeTgDerivative   ), 1, "tg"       , 3},
    {OPERATOR_CTG      ,          {"ctg",     3}  ,COMP_OPTION(SimplifyCtg, TakeCtgDerivative  ), 1, "ctg"      , 3},
};
const size_t OPERATION_COUNT = sizeof(OPERATION_INFO) / sizeof(OPERATION_INFO[0]);

#pragma GCC diagnostic warning "-Wwrite-strings"

#undef CONDITIONAL_COMPILATION

#endif // OPERATION_INFO_H