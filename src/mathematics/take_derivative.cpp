#include "take_derivative.h"

#include "Assert.h"
#include "derivative.h"
#include "derivative_defines.h"
#include "latex_dump.h"
#include "tree.h"
#include "tools.h"
#include "latex_dump.h"
#include "simplify.h"

static take_derivative_return_e TakeVarDerivative(derivative_t derivative, 
                                                  ssize_t current_node);

// =============================== DEFINES ====================================

#define RIGHT_INDEX(__X) derivative->ariphmetic_tree->nodes_array[__X].right_index
#define LEFT_INDEX(__X)  derivative->ariphmetic_tree->nodes_array[__X].left_index
#define PARENT_INDEX(__X)  derivative->ariphmetic_tree->nodes_array[__X].parent_index
#define PARENT_CONNECTION(__X)  derivative->ariphmetic_tree->nodes_array[__X].parent_connection

static ssize_t
TakePlusDerivative(derivative_t derivative, ssize_t current_node);

static ssize_t 
TakeMinusDerivative(derivative_t derivative, ssize_t current_node);

static ssize_t 
TakeMulDerivative(derivative_t derivative, ssize_t current_node);

static ssize_t 
TakeDivDerivative(derivative_t derivative, ssize_t current_node);

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
        derivative->ariphmetic_tree->
            nodes_array[current_node].node_value.expression.constant = 0;

        return current_node;
    }
    else if (node_value.expression_type == EXPRESSION_TYPE_VAR)
    {
        TakeVarDerivative(derivative, current_node);

        return current_node;
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
                break;

            case OPERATOR_COS:
                break;

            case OPERATOR_LN:
                break;

            case OPERATOR_EXP:
                break;

            case OPERATOR_POWER:
                break;

            case OPERATOR_UNDEFINED:
            default: return NO_LINK;
        }
    }

    return NO_LINK;
}

// ========================== TAKING_DERIVATIVES ==============================

static take_derivative_return_e
TakeVarDerivative(derivative_t derivative,
                  ssize_t      current_node)
{
    ASSERT(derivative != NULL);

    derivative->ariphmetic_tree->
        nodes_array[current_node].node_value.expression_type = EXPRESSION_TYPE_CONST;

    derivative->ariphmetic_tree->
        nodes_array[current_node].node_value.expression.constant = 1;

    return TAKE_DERIVATIVE_RETURN_SUCCESS;
}

// =========================== OPERATION_DERIVATION ===========================

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
{ REPLACE(DIV(SUB(MUL(cL, D(cR)), MUL(D(cL), cR)), POW(cL, 2))) }

