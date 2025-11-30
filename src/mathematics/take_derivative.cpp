#include "take_derivative.h"

#include "Assert.h"
#include "derivative.h"
#include "derivative_defines.h"
#include "latex_dump.h"
#include "tree.h"
#include "tools.h"
#include "latex_dump.h"
#include "simplify.h"

// ================================= RULES ====================================

#define PARENT_INDEX(__X)  derivative->ariphmetic_tree->nodes_array[__X].parent_index
#define PARENT_CONNECTION(__X)  derivative->ariphmetic_tree->nodes_array[__X].parent_connection

static ssize_t 
TakeConstDerivative(derivative_t derivative,
                    ssize_t      current_node)
{ REPLACE(CONST(0)); }

static ssize_t
TakeVarDerivative(derivative_t derivative,
                  ssize_t      current_node)
{ REPLACE(CONST(1)); }

static ssize_t 
TakePlusDerivative(derivative_t derivative,
                   ssize_t      current_node)
{ REPLACE(SUM(D(cL), D(cR))); }

static ssize_t 
TakeMinusDerivative(derivative_t derivative,
                    ssize_t      current_node)
{ REPLACE(SUB(D(cL), D(cR))); }

static ssize_t 
TakeMulDerivative(derivative_t derivative,
                  ssize_t      current_node)
{ REPLACE(SUM(MUL(cL, D(cR)), MUL(D(cL), cR))); }

static ssize_t 
TakeDivDerivative(derivative_t derivative,
                  ssize_t      current_node)
{ REPLACE(DIV(SUB( MUL(cL, D(cR)), MUL(D(cL), cR)), POW(cR, CONST(2)))) }

static ssize_t 
TakeSinDerivative(derivative_t derivative,
                  ssize_t      current_node)
{ REPLACE(MUL(D(cL), COS(cL))) }

static ssize_t 
TakeCosDerivative(derivative_t derivative,
                  ssize_t      current_node)
{ REPLACE(MUL(D(cL), MUL(CONST(-1), SIN(cL)))) }

static ssize_t 
TakeLnDerivative(derivative_t derivative,
                 ssize_t      current_node)
{ REPLACE(MUL(D(cL), DIV(CONST(1), cL))) }

static ssize_t 
TakeExpDerivative(derivative_t derivative,
                  ssize_t      current_node)
{ REPLACE(MUL(D(cL), EXP(cL))) }

struct function_derivative_s
{
    operations_e operation;
    ssize_t (*derivative) (derivative_t, ssize_t);
};

const function_derivative_s OP_DERIVATIVES[] =
{//  OPERATIONS            DERIVATIVES
    {OPERATOR_UNDEFINED,   NULL               },
    {OPERATOR_PLUS     ,   TakePlusDerivative },
    {OPERATOR_MINUS    ,   TakeMinusDerivative},
    {OPERATOR_MUL      ,   TakeMulDerivative  },
    {OPERATOR_DIV      ,   TakeDivDerivative  },
    {OPERATOR_SIN      ,   TakeSinDerivative  },
    {OPERATOR_COS      ,   TakeCosDerivative  },
    {OPERATOR_POWER    ,   NULL               },
    {OPERATOR_LN       ,   TakeLnDerivative   },
    {OPERATOR_EXP      ,   TakeExpDerivative  }
};
const size_t DERIVATIVES_COUNT = sizeof(OP_DERIVATIVES) / sizeof(OP_DERIVATIVES[0]);

// =========================== MAIN_DERIVATIVE ================================

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

        return OP_DERIVATIVES[node_value.expression.operation]
                                        .derivative(derivative, current_node);
    }
    
    return NO_LINK;
}
