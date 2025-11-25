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

#define TURN_ON_CAST _Pragma("GCC diagnostic warning \"-Wuseless-cast\"") \
                     _Pragma("GCC diagnostic warning \"-Wconversion\"")\
                     _Pragma("GCC diagnostic warning \"-Wimplicit-fallthrough\"")
#define TURN_OFF_CAST _Pragma("GCC diagnostic ignored \"-Wuseless-cast\"")\
                      _Pragma("GCC diagnostic ignored \"-Wconversion\"")\
                      _Pragma("GCC diagnostic ignored \"-Wimplicit-fallthrough\"")


#define INIT_EXPR(__TYPE, __VALUE)\
    do {switch(__TYPE)\
    {\
        TURN_OFF_CAST\
        case EXPRESSION_TYPE_CONST:\
            expr__.expression.constant = (double) __VALUE;\
            break;\
        case EXPRESSION_TYPE_OPERATOR:\
            expr__.expression.operation = (operations_e) __VALUE;\
            break;\
        case EXPRESSION_TYPE_VAR:\
            expr__.expression.variable = (char) __VALUE;\
        case EXPRESSION_TYPE_UNDEFINED:\
        default: return TAKE_DERIVATIVE_INCORRECT_EXPRESSION;\
        TURN_ON_CAST\
    }} while(0);

#define CHECK_INDEX \
    if (current_node == NO_LINK)\
    {\
        return TAKE_DERIVATIVE_UNDEFINED_FUNCTION;\
    }

#define INSERT_BEFORE(__CHILD, __TYPE, __VALUE, __HOW) \
    do {take_derivative_return_e output = TAKE_DERIVATIVE_RETURN_SUCCESS;\
        expression_s expr__ = {.expression_type = __TYPE};\
        INIT_EXPR(__TYPE, __VALUE);\
        if ((output = InsertNode(derivative, __CHILD, \
        &expr__, __HOW)) != TAKE_DERIVATIVE_RETURN_SUCCESS)\
        {\
            return output;\
        }\
        } while (0)

#define ADD_NODE(__PARENT, __TYPE, __VALUE, __HOW) \
    do {take_derivative_return_e output = TAKE_DERIVATIVE_RETURN_SUCCESS;\
        expression_s expr__ = {.expression_type = __TYPE};\
        INIT_EXPR(__TYPE, __VALUE);\
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

#define CHANGE_OPER(__OP)  derivative->ariphmetic_tree->\
                            nodes_array[current_node].node_value.expression.operation = __OP

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
TakeDivDerivative(derivative_t derivative,
                  ssize_t      current_node);

static take_derivative_return_e
TakeSinDerivative(derivative_t derivative,
                  ssize_t       current_node);
    
static take_derivative_return_e
TakeCosDerivative(derivative_t derivative,
                  ssize_t      current_node);

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
                TakeDivDerivative(derivative, current_node);
                break;

            case OPERATOR_SIN:
                TakeSinDerivative(derivative, current_node);
                break;

            case OPERATOR_COS:
                TakeCosDerivative(derivative, current_node);
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
                  OPERATOR_PLUS, EDGE_DIR_LEFT);

    current_node = PARENT_INDEX(current_node);

    COPY(current_node, LEFT_INDEX(current_node), 
         EDGE_DIR_RIGHT);

    D(RIGHT_INDEX(RIGHT_INDEX(current_node)));
    D(LEFT_INDEX(LEFT_INDEX(current_node)));
    
    return TAKE_DERIVATIVE_RETURN_SUCCESS;
}

static take_derivative_return_e
TakeDivDerivative(derivative_t derivative,
                  ssize_t      current_node)
{
    ASSERT(derivative != NULL);

    CHANGE_OPER(OPERATOR_MUL);

    INSERT_BEFORE(current_node, EXPRESSION_TYPE_OPERATOR,
                  OPERATOR_MINUS, EDGE_DIR_LEFT);

    current_node = PARENT_INDEX(current_node);

    COPY(current_node, LEFT_INDEX(current_node), EDGE_DIR_RIGHT);

    D(LEFT_INDEX(LEFT_INDEX(current_node)));
    D(RIGHT_INDEX(RIGHT_INDEX(current_node)));

    INSERT_BEFORE(current_node, EXPRESSION_TYPE_OPERATOR,
                  OPERATOR_DIV, EDGE_DIR_LEFT);
    
    current_node = PARENT_INDEX(current_node);

    ADD_NODE(current_node, EXPRESSION_TYPE_OPERATOR,
             OPERATOR_POWER, EDGE_DIR_RIGHT);

    COPY(RIGHT_INDEX(current_node), RIGHT_INDEX(LEFT_INDEX(LEFT_INDEX(current_node))), 
         EDGE_DIR_LEFT);

    ADD_NODE(RIGHT_INDEX(current_node), EXPRESSION_TYPE_CONST, 
                                     2, EDGE_DIR_RIGHT);

    return TAKE_DERIVATIVE_RETURN_SUCCESS;
}

static take_derivative_return_e
TakeSinDerivative(derivative_t derivative,
                  ssize_t      current_node)
{
    ASSERT(derivative != 0);

    CHANGE_OPER(OPERATOR_COS);

    INSERT_BEFORE(current_node, EXPRESSION_TYPE_OPERATOR,
                  OPERATOR_MUL, EDGE_DIR_RIGHT);

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

    CHANGE_OPER(OPERATOR_SIN);

    INSERT_BEFORE(current_node, EXPRESSION_TYPE_OPERATOR,
                  OPERATOR_MUL, EDGE_DIR_RIGHT);

    current_node = PARENT_INDEX(current_node);

    COPY(current_node, LEFT_INDEX(RIGHT_INDEX(current_node)),
         EDGE_DIR_LEFT);

    D(LEFT_INDEX(current_node));
    
    INSERT_BEFORE(current_node, EXPRESSION_TYPE_OPERATOR,
                  OPERATOR_MUL, EDGE_DIR_RIGHT);

    current_node = PARENT_INDEX(current_node);
    
    ADD_NODE(current_node, EXPRESSION_TYPE_CONST,
             -1, EDGE_DIR_LEFT);

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
#undef CHANGE_OPER
#undef TURN_ON_CAST
#undef TURN_OFF_CAST 
#undef INIT_EXPR