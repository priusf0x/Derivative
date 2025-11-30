#include "recursive_decent.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "derivative_defines.h"
#include "derivative.h"
#include "expression.h"
#include "buffer.h"
#include "tree.h"
#include "tools.h"
#include "color.h"

// ============================= HELPERS ======================================

#define CURRENT_STRING (derivative->buffer->buffer \
                        + derivative->buffer->current_position)

static bool
CheckIfVar(derivative_t derivative)
{
    char current_symbol = *CURRENT_STRING;
    if (('a' <= current_symbol && current_symbol >= 'z') 
        || ('A' <= current_symbol && current_symbol >= 'Z')
        || (current_symbol == '_'))
    {
        return true;
    }

    return false;
}

static void 
SkipSpacesInBuffer(buffer_t buffer)
{
    buffer->current_position = SkipSpaces(buffer->buffer, buffer->current_position);
}

static void 
SkipNSymbols(buffer_t buffer,
             size_t   n)
{
    buffer->current_position += n;
}

struct key_words_function_s 
{
    const char*  function_name;
    size_t       name_len;
    operations_e operation;
};

const struct key_words_function_s FUNCTIONS[] =
{// FUNCTION_NAME   NAME_LENGHT        OPERATION
    {       "sin",            3,    OPERATOR_SIN},
    {       "cos",            3,    OPERATOR_COS},
    {        "ln",            2,     OPERATOR_LN},
    {       "exp",            3,    OPERATOR_EXP}
};
const size_t FUNCTIONS_COUNT = sizeof(FUNCTIONS) / sizeof(FUNCTIONS[0]);

// ============================= GET_FUNCTIONS ================================

static ssize_t GetE(derivative_t derivative);
static ssize_t GetT(derivative_t derivative);
static ssize_t GetP(derivative_t derivative);
static ssize_t GetN(derivative_t derivative);

static operations_e 
CheckIfFunction(derivative_t derivative)
{
    for (size_t index = 0; index < FUNCTIONS_COUNT; index++)
    {
        if (ExStrCmp(CURRENT_STRING, FUNCTIONS[index].function_name,
                     FUNCTIONS[index].name_len, " \r\n\t("))
        {
            SkipNSymbols(derivative->buffer, FUNCTIONS[index].name_len);
            SkipSpacesInBuffer(derivative->buffer);

            return FUNCTIONS[index].operation;
        }
    }

    return OPERATOR_UNDEFINED;
}

static ssize_t 
GetN(derivative_t derivative)
{
    #ifndef NDEBUG
        BufferDump(derivative->buffer);
    #endif

    char* current_string = CURRENT_STRING; 
    char* endptr = NULL;

    double value = strtod(current_string, &endptr);

    derivative->buffer->current_position = (size_t) (endptr - 
                                                        derivative->buffer->buffer);
    SkipSpacesInBuffer(derivative->buffer);

    return CONST(value);
}

static ssize_t
GetP(derivative_t derivative)
{
    char current_symbol = *CURRENT_STRING;
    ssize_t return_value = NO_LINK;
    operations_e possible_op = OPERATOR_UNDEFINED;

    #ifndef NDEBUG
        BufferDump(derivative->buffer);
    #endif 

    if (current_symbol == '(')
    {
        //TODO -  придумать обработку какую нибудь хз
        SkipNSymbols(derivative->buffer, 1);
        SkipSpacesInBuffer(derivative->buffer);

        return_value = GetE(derivative);

        if (*CURRENT_STRING != ')')
        {
            return NO_LINK;
        }

        SkipNSymbols(derivative->buffer, 1);
        SkipSpacesInBuffer(derivative->buffer);
        
        return return_value;
    }    
    else if ((possible_op = CheckIfFunction(derivative)) != OPERATOR_UNDEFINED)
    {
        if (*CURRENT_STRING != '(')
        {
            return NO_LINK;
        }

        SkipNSymbols(derivative->buffer, 1);
        SkipSpacesInBuffer(derivative->buffer);

        return_value = GetE(derivative);

        if (*CURRENT_STRING != ')')
        {
            return NO_LINK;
        }

        SkipNSymbols(derivative->buffer, 1);
        SkipSpacesInBuffer(derivative->buffer);
        
        return DerivativeAddOperation(derivative, return_value, NO_LINK, 
                                      possible_op);
    }
    else if (CheckIfVar(derivative)) // FIXME
    {
        char variable = *CURRENT_STRING;
        SkipNSymbols(derivative->buffer, 1);
        SkipSpacesInBuffer(derivative->buffer);

        return VAR(variable);
    }
    else 
    {
        return GetN(derivative);
    }
}

static ssize_t
GetT(derivative_t derivative)
{
    ssize_t last_add = GetP(derivative);

    char last_symbol = *CURRENT_STRING;
    char operation = 0;

    while((last_symbol == '*') || (last_symbol == '/')
          || (last_symbol == '^'))
    {
        #ifndef NDEBUG
            BufferDump(derivative->buffer);
        #endif

        operation = last_symbol;

        SkipNSymbols(derivative->buffer, 1);
        SkipSpacesInBuffer(derivative->buffer);

        switch(operation)
       {
            case '*':
                last_add = MUL(last_add, GetP(derivative));
                break;

            case '/':
                last_add = DIV(last_add, GetP(derivative));
                break;

            case '^':
                last_add = POW(last_add, GetP(derivative));
                break;

            default: return NO_LINK;
        }

        last_symbol = *CURRENT_STRING;
    }

    return last_add; 
} 

static ssize_t
GetE(derivative_t derivative)
{
    ssize_t last_add = GetT(derivative);

    char last_symbol = *CURRENT_STRING;
    char operation = 0;

    while((last_symbol == '+') || (last_symbol == '-'))
    {
        #ifndef NDEBUG
            BufferDump(derivative->buffer);
        #endif

        operation = last_symbol;

        SkipNSymbols(derivative->buffer, 1);
        SkipSpacesInBuffer(derivative->buffer);

        switch(operation)
        {
            case '+':
                last_add = SUM(last_add, GetT(derivative));
                break;

            case '-':
                last_add = SUB(last_add, GetT(derivative));
                break;

            default: return NO_LINK;
        }

        last_symbol = *CURRENT_STRING;
    }

    return last_add;
}

derivative_return_e
ConvertToGraph(derivative_t derivative)
{ 
    ssize_t readen_node = GetE(derivative);

    if ((readen_node == NO_LINK) || IF_DERIVATIVE_FAILED)
    {
        const char* error_read_message = RED "Error was occupied in reading." 
                                             "Buffer dump:\n";

        fprintf(stderr, "%s", error_read_message);
        BufferDump(derivative->buffer);
    }

    if (ForceConnect(derivative->ariphmetic_tree, readen_node, 0,
                                            EDGE_DIR_LEFT) != TREE_RETURN_SUCCESS)
    {
        return DERIVATIVE_RETURN_TREE_ERROR;
    }

    #ifndef NDEBUG
    BufferDump(derivative->buffer);
    #endif 

    return DERIVATIVE_RETURN_SUCCESS; 
}

#undef CURRENT_STRING
