#include "take_derivative.h"

#include "Assert.h"
#include "derivative.h"
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

#define CHECK_INDEX \
    if (current_node == NO_LINK)\
    {\
        return TAKE_DERIVATIVE_UNDEFINED_FUNCTION;\
    }

#define INSERT_BEFORE(__CHILD, __TYPE, __VALUE, __HOW) \
    do {take_derivative_return_e output = TAKE_DERIVATIVE_RETURN_SUCCESS;\
        expression_s expr__ = {__VALUE, __TYPE};\
        if ((output = InsertNode(derivative, __CHILD, \
        &expr__, __HOW)) != TAKE_DERIVATIVE_RETURN_SUCCESS)\
        {\
            return output;\
        }\
        } while (0)

#define ADD_NODE(__PARENT, __TYPE, __VALUE, __HOW) \
    do {take_derivative_return_e output = TAKE_DERIVATIVE_RETURN_SUCCESS;\
        expression_s expr__ = {__VALUE, __TYPE};\
        if ((output = AddNode(derivative, __PARENT, \
        &expr__, __HOW)) != TAKE_DERIVATIVE_RETURN_SUCCESS)\
        {\
            return output;\
        }\
        } while (0)

#define COPY(__DST, __SRC, __HOW) \
    do {if (CopySubgraph(derivative->ariphmetic_tree, (size_t) __DST, \
        (size_t) __SRC, __HOW) != 0)\
        {\
            return TAKE_DERIVATIVE_TREE_ERROR;\
        }\
        } while (0)

#define D(__X) \
    do {take_derivative_return_e output = TAKE_DERIVATIVE_RETURN_SUCCESS; \
        if ((output = TakeDerivative(derivative, ((ssize_t) __X))) != TAKE_DERIVATIVE_RETURN_SUCCESS)\
        {\
            return output;\
        }\
        } while (0)

static take_derivative_return_e
TakePlusDerivative(derivative_t derivative,
                   ssize_t       current_node);

static take_derivative_return_e
TakeMinusDerivative(derivative_t derivative,
                    ssize_t       current_node);

static take_derivative_return_e
TakeMulDerivative(derivative_t derivative,
                  ssize_t       current_node);

static take_derivative_return_e
TakeSinDerivative(derivative_t derivative,
                  ssize_t       current_node);

// =========================== MAIN_DERIVATIVE ================================

take_derivative_return_e
TakeDerivative(derivative_t derivative,
               ssize_t      current_node)
{
    ASSERT(derivative != NULL);

    TreeDump(derivative->ariphmetic_tree);

    CHECK_INDEX;

    expression_s node_value = derivative->ariphmetic_tree->
                                nodes_array[current_node].node_value;

    if (node_value.expression_type == EXPRESSION_TYPE_CONST)
    {
        derivative->ariphmetic_tree->
            nodes_array[current_node].node_value.expression.constant = 0;

        return TAKE_DERIVATIVE_RETURN_SUCCESS;
    }
    else if (node_value.expression_type == EXPRESSION_TYPE_VAR)
    {
        TakeVarDerivative(derivative, current_node);

        return TAKE_DERIVATIVE_RETURN_SUCCESS;
    }
    else if (node_value.expression_type == EXPRESSION_TYPE_OPERATOR)
    {
        switch(node_value.expression.operation)
        {
            case OPERATOR_PLUS:
                TakePlusDerivative(derivative, current_node);
                break;

            case OPERATOR_MINUS:
                TakeMinusDerivative(derivative, current_node);
                break;

            case OPERATOR_MUL:
                TakeMulDerivative(derivative, current_node);
                break;
            
            case OPERATOR_DIV:
                break;

            case OPERATOR_SIN:
                TakeSinDerivative(derivative, current_node);
                break;

            case OPERATOR_COS:
                break;

            case OPERATOR_UNDEFINED:
                return TAKE_DERIVATIVE_UNDEFINED_FUNCTION;

            default: return TAKE_DERIVATIVE_UNDEFINED_FUNCTION;
        }
    }

    return TAKE_DERIVATIVE_RETURN_SUCCESS;
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

// ============================== TREE_HELPERS ================================

static take_derivative_return_e
InsertNode(derivative_t      derivative,
           ssize_t           current_node,
           expression_s*     expr,
           edge_dir_e        child_dir)
{
    ASSERT(derivative != NULL);
    ASSERT(expr != NULL);

    node_s node = derivative->ariphmetic_tree->
                    nodes_array[current_node];

    node_s new_node = {.parent_index      = node.parent_index,
                       .parent_connection = node.parent_connection,
                       .right_index       = NO_LINK,
                       .left_index        = NO_LINK,
                       .node_value        = *expr};

    if (child_dir == EDGE_DIR_LEFT)
    {
        new_node.left_index = current_node;
    }
    else if (child_dir == EDGE_DIR_RIGHT)
    {
        new_node.right_index = current_node;
    }
    else 
    {
        return TAKE_DERIVATIVE_INVALID_VALUE;
    }

    if (TreeAddNode(derivative->ariphmetic_tree, &new_node) != 0)
    {
        return TAKE_DERIVATIVE_TREE_ERROR;
    }
    
    return TAKE_DERIVATIVE_RETURN_SUCCESS;
}

static take_derivative_return_e
AddNode(derivative_t      derivative,
        ssize_t           current_node,
        expression_s*     expr,
        edge_dir_e        dir)
{
    ASSERT(derivative != NULL);
    ASSERT(expr != NULL);

    node_s new_node = {.parent_index      = current_node,
                       .parent_connection = dir,
                       .right_index       = NO_LINK,
                       .left_index        = NO_LINK,
                       .node_value        = *expr};

    if (TreeAddNode(derivative->ariphmetic_tree, &new_node) != 0)
    {
        return TAKE_DERIVATIVE_TREE_ERROR;
    }

    return TAKE_DERIVATIVE_RETURN_SUCCESS;
}

// ============================= FUNCTION_DERIVATIVES =========================

static take_derivative_return_e
TakePlusDerivative(derivative_t derivative,
                   ssize_t       current_node)
{
    ASSERT(derivative != NULL);

    CHECK_INDEX;

    D(RIGHT_INDEX(current_node));
    D(LEFT_INDEX(current_node));

    return TAKE_DERIVATIVE_RETURN_SUCCESS;
}

static take_derivative_return_e
TakeMinusDerivative(derivative_t derivative,
                    ssize_t       current_node)
{
    ASSERT(derivative != NULL);

    CHECK_INDEX;

    D(RIGHT_INDEX(current_node));
    D(LEFT_INDEX(current_node));

    return TAKE_DERIVATIVE_RETURN_SUCCESS;
}

static take_derivative_return_e
TakeMulDerivative(derivative_t derivative,
                  ssize_t      current_node)
{
    ASSERT(derivative != NULL);

    CHECK_INDEX;

    INSERT_BEFORE(current_node, EXPRESSION_TYPE_OPERATOR,
                  {.operation = OPERATOR_PLUS}, EDGE_DIR_LEFT);

    current_node = PARENT_INDEX(current_node);

    COPY(current_node, LEFT_INDEX(current_node), 
         EDGE_DIR_RIGHT);

    D(RIGHT_INDEX(RIGHT_INDEX(current_node)));
    D(LEFT_INDEX(LEFT_INDEX(current_node)));
    
    return TAKE_DERIVATIVE_RETURN_SUCCESS;
}

static take_derivative_return_e
TakeSinDerivative(derivative_t derivative,
                  ssize_t      current_node)
{
    ASSERT(derivative != 0);

    derivative->ariphmetic_tree->
        nodes_array[current_node].node_value.expression.operation = OPERATOR_COS;

    INSERT_BEFORE(current_node, EXPRESSION_TYPE_OPERATOR,
                  {.operation = OPERATOR_MUL}, EDGE_DIR_RIGHT);

    current_node = PARENT_INDEX(current_node);

    COPY(current_node, LEFT_INDEX(RIGHT_INDEX(current_node)),
         EDGE_DIR_LEFT);

    D(LEFT_INDEX(current_node));

    return TAKE_DERIVATIVE_RETURN_SUCCESS;
}

static take_derivative_return_e
TakeCosDerivative(derivative_t derivative,
                  ssize_t      current_node)
{
    ASSERT(derivative != 0);

    derivative->ariphmetic_tree->
        nodes_array[current_node].node_value.expression.operation = OPERATOR_COS;

    INSERT_BEFORE(current_node, EXPRESSION_TYPE_OPERATOR,
                  {.operation = OPERATOR_MUL}, EDGE_DIR_RIGHT);

    current_node = PARENT_INDEX(current_node);

    COPY(current_node, LEFT_INDEX(RIGHT_INDEX(current_node)),
         EDGE_DIR_LEFT);

    D(LEFT_INDEX(current_node));
    
    INSERT_BEFORE(current_node, EXPRESSION_TYPE_OPERATOR,
                  {.operation = OPERATOR_MUL}, EDGE_DIR_RIGHT);

    current_node = PARENT_INDEX(current_node);
    
    ADD_NODE(current_node, EXPRESSION_TYPE_CONST,
             {.constant = -1} EDGE_DIR_LEFT);

    return TAKE_DERIVATIVE_RETURN_SUCCESS;
}

// ============================== UNDEFINITION ================================

#undef RIGHT_INDEX
#undef LEFT_INDEX  
#undef PARENT_INDEX  
#undef PARENT_CONNECTION  
#undef CHECK_INDEX 
#undef INSERT_BEFORE 
#undef COPY 
#undef D 
#undef ADD_NODE