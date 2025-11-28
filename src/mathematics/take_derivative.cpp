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

ssize_t (*d_operator[]) (derivative_t derivative, ssize_t current_node) =
{
    TakePlusDerivative
};

// =========================== MAIN_DERIVATIVE ================================

ssize_t 
TakeDerivative(derivative_t derivative,
               ssize_t      current_node)
{
    ASSERT(derivative != NULL);

    #ifndef NDEBUG
        TreeDump(derivative->ariphmetic_tree);
    #endif

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
        switch(node_value.expression.operation)
        {
            case OPERATOR_PLUS:
                return TakePlusDerivative(derivative, current_node);

            case OPERATOR_MINUS:
                return TakeMinusDerivative(derivative, current_node);

            case OPERATOR_MUL:
                return TakeMulDerivative(derivative, current_node);
            
            case OPERATOR_DIV:
                return TakeDivDerivative(derivative, current_node);

            case OPERATOR_SIN:
                return TakeSinDerivative(derivative, current_node);

            case OPERATOR_COS:
                return TakeCosDerivative(derivative, current_node);

            case OPERATOR_LN:
                return TakeLnDerivative(derivative, current_node);

            case OPERATOR_EXP:
                return TakeExpDerivative(derivative, current_node);

            case OPERATOR_POWER:
                break;

            case OPERATOR_UNDEFINED:
            default: return NO_LINK;
        }
    }

    return NO_LINK;
}
