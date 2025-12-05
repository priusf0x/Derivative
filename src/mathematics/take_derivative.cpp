#include "take_derivative.h"

#include <string.h>

#include "Assert.h"
#include "derivative.h"
#include "derivative_defines.h"
#include "tools.h"
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
{
    if (GetVarNodeHash(current_node, derivative) 
        == derivative->variable_hash)
    { REPLACE(CONST__(1)); }
    else
    { REPLACE(CONST__(0)); }
}


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
TakePowDerivative(derivative_t derivative,
                  ssize_t      current_node)
{ if (NODE(L_O)->node_value.expression_type == EXPRESSION_TYPE_CONST
      && NODE(R_O)->node_value.expression_type == EXPRESSION_TYPE_CONST)
    {REPLACE(CONST__(0));}
  else if (NODE(R_O)->node_value.expression_type == EXPRESSION_TYPE_CONST)
    {REPLACE(MUL__(D__(c_L), MUL__(c_R, POW__(c_L, SUB__(c_R, CONST__(1))))));}
  else 
    {REPLACE(D__(EXP__(MUL__(LN__(c_L), c_R)))); }
}

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

static ssize_t 
TakeTgDerivative(derivative_t derivative,
                  ssize_t      current_node)
{ REPLACE(DIV__(D__(c_L), POW__(COS__(c_L), CONST__(2)))); }

static ssize_t 
TakeCtgDerivative(derivative_t derivative,
                  ssize_t      current_node)
{ REPLACE(DIV__(MUL__(D__(c_L), CONST__(-1)), POW__(SIN__(c_L), CONST__(2)))); }


struct function_derivative_s
{
    operations_e operation;
    ssize_t (*derivative) (derivative_t, ssize_t);
};

#define _DERIVATOR_
#include "operation_info.h"

// ========================== MAIN_DERIVATIVE ================================

ssize_t
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

    LogDeritativeInLatex(derivative, current_node, NULL);
    
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

derivative_return_e 
TakeDerivative(derivative_t derivative,
               char*  variable_name)
{
    ASSERT(derivative != NULL);
    ASSERT(variable_name != NULL);

    derivative->variable_hash = MurmurHash2(variable_name, 
                                            (unsigned int) strlen(variable_name));

    ssize_t output = TakeExpressionDerivative(derivative, 0);
    if (output != NO_LINK)
    {
        derivative->ariphmetic_tree->nodes_array[output].parent_index = 0;
        derivative->ariphmetic_tree->nodes_array[0].left_index = output;  
    }

    return derivative->error;
}

