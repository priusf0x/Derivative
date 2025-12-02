#include "take_derivative.h"

#include "Assert.h"
#include "derivative.h"
#include "derivative_defines.h"
#include "latex_dump.h"
#include "tree.h"
#include "latex_dump.h"
#include "simplify.h"

// ================================= RULES ====================================

static ssize_t 
TakeConstDerivative(derivative_t derivative,
                    ssize_t      current_node)
{ REPLACE(CONST__(0)); }

static ssize_t
TakeVarDerivative(derivative_t derivative,
                  ssize_t      current_node)
{ REPLACE(CONST__(1)); }

static ssize_t 
TakePlusDerivative(derivative_t derivative,
                   ssize_t      current_node)
{ REPLACE(SUM__(D__(c_L), D__(c_R))); }

static ssize_t 
TakeMinusDerivative(derivative_t derivative,
                    ssize_t      current_node)
{ REPLACE(SUB__(D__(c_L), D__(c_R))); }

static ssize_t 
TakeMulDerivative(derivative_t derivative,
                  ssize_t      current_node)
{ REPLACE(SUM__(MUL__(c_L, D__(c_R)), MUL__(D__(c_L), c_R))); }

static ssize_t 
TakeDivDerivative(derivative_t derivative,
                  ssize_t      current_node)
{ REPLACE(DIV__(SUB__( MUL__(c_L, D__(c_R)), MUL__(D__(c_L), c_R)), POW__(c_R, CONST__(2)))); }

static ssize_t 
TakeSinDerivative(derivative_t derivative,
                  ssize_t      current_node)
{ REPLACE(MUL__(D__(c_L), COS__(c_L)));}

static ssize_t 
TakeCosDerivative(derivative_t derivative,
                  ssize_t      current_node)
{ REPLACE(MUL__(D__(c_L), MUL__(CONST__(-1), SIN__(c_L)))); }

static ssize_t 
TakeLnDerivative(derivative_t derivative,
                 ssize_t      current_node)
{ REPLACE(MUL__(D__(c_L), DIV__(CONST__(1), c_L))); }

static ssize_t 
TakeExpDerivative(derivative_t derivative,
                  ssize_t      current_node)
{ REPLACE(MUL__(D__(c_L), EXP__(c_L))); }

struct function_derivative_s
{
    operations_e operation;
    ssize_t (*derivative) (derivative_t, ssize_t);
};

#define _DERIVATOR_
#include "operation_info.h"

// ========================== MAIN_DERIVATIVE ================================

ssize_t  // TODO: make static and add main take derivative function
TakeExpressionDerivative(derivative_t derivative,
                         ssize_t      current_node)
{
    ASSERT(derivative != NULL);
    RETURN_NO_LINK_IF_ERROR;

    #ifndef NDEBUG
        TreeDump(derivative->ariphmetic_tree);
    #endif

    if(current_node == 0)
    {
        current_node = derivative->ariphmetic_tree->
                            nodes_array[current_node].left_index;
    }

    expression_s node_value = derivative->ariphmetic_tree->
                                nodes_array[current_node].node_value;

    if (node_value.expression_type == EXPRESSION_TYPE_CONST)
    {
        return TakeConstDerivative(derivative, current_node);
    }
    else if (node_value.expression_type == EXPRESSION_TYPE_VAR)
    {
        return TakeVarDerivative(derivative, current_node);
    }
    else if (node_value.expression_type == EXPRESSION_TYPE_OPERATOR)
    {
        if (node_value.expression.operation == OPERATOR_UNDEFINED)
        {
            derivative->error = DERIVATIVE_RETURN_UNDEFINED_OPERATION;
            return NO_LINK;
        }

        return OPERATION_INFO[node_value.expression.operation]
                            .op_function(derivative, current_node);
    }
    
    return NO_LINK;
}
