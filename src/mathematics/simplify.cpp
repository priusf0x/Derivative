#include "simplify.h"

#include "Assert.h"
#include "derivative.h"
#include "tools.h"
#include "expression.h"

// =========================== SIMPLIFICATION =================================

static simplify_return_e 
SimplifyMultiplicationOnNull(derivative_t derivative,
                             ssize_t*     current_node);
static bool
CheckIfOperator(const derivative_t derivative,   
          ssize_t            current_node);
static bool
CheckIfConst(const derivative_t derivative,   
             ssize_t            current_node);
static bool
CheckIfConstNum(const derivative_t derivative,
                ssize_t            current_node,
                double             number);
static simplify_return_e
SimplifySumWithNull(derivative_t derivative,
                    ssize_t      current_node);
static simplify_return_e
SimplifyMultiplicationOnOne(derivative_t derivative,
                            ssize_t      current_node);
static simplify_return_e
SimplifySubstractWithNull(derivative_t derivative,
                          ssize_t       current_node);

simplify_return_e
SimplifyNeutralMultipliers(derivative_t derivative,
                           ssize_t      current_node)
{
    ASSERT(derivative);

    if (current_node == NO_LINK)
    {
        return SIMPLIFY_RETURN_INCORRECT_VALUE;
    }

    #ifndef NDEBUG
        TreeDump(derivative->ariphmetic_tree);
    #endif

    node_s node = derivative->ariphmetic_tree->nodes_array[current_node];

    if (node.right_index != NO_LINK)
    {
        SimplifyNeutralMultipliers(derivative, node.right_index);
    }
    if (node.left_index != NO_LINK)
    {
        SimplifyNeutralMultipliers(derivative, node.left_index);
    }

    simplify_return_e output = SIMPLIFY_RETURN_SUCCESS;

    if (CheckIfOperator(derivative, current_node))
    {
        if (node.node_value.expression.operation == OPERATOR_MUL)
        {
            if ((output = SimplifyMultiplicationOnNull(derivative, &current_node))
                != SIMPLIFY_RETURN_SUCCESS)
            {
                return output;
            }

            if (CheckIfConstNum(derivative, current_node, 0))
            {
                return SIMPLIFY_RETURN_SUCCESS;
            }

            if ((output = SimplifyMultiplicationOnOne(derivative, current_node))
                != SIMPLIFY_RETURN_SUCCESS)
            {
                return output;
            }

            return SIMPLIFY_RETURN_SUCCESS;
        }

        if (node.node_value.expression.operation == OPERATOR_PLUS)
        {
            output = SimplifySumWithNull(derivative, current_node);
                
            return output;
        }

        if (node.node_value.expression.operation == OPERATOR_MINUS)
        {
            output = SimplifySubstractWithNull(derivative, current_node);

            return output;
        }
    }

    return SIMPLIFY_RETURN_SUCCESS;
}

// simplify_return_e
// SimplifyDoubles(derivative_t derivative,
//                 ssize_t      current_node)
// {
//     ASSERT(derivative != NULL);

//     simplify_return_e output = SIMPLIFY_RETURN_SUCCESS;
    
//     node_s node = derivative->ariphmetic_tree->nodes_array[current_node];

//     if (node.right_index != NO_LINK)
//     {
//         SimplifyNeutralMultipliers(derivative, node.right_index);
//     }
//     if (node.left_index != NO_LINK)
//     {
//         SimplifyNeutralMultipliers(derivative, node.left_index);
//     }

//     if (CheckIfOperator(derivative, current_node) 
//         && CheckIfConst(derivative, node.right_index)
//         && CheckIfConst(derivative, node.left_index))
//     {
//         switch(node.node_value.expression_type)
//         {
//             case OPERATOR_PLUS:
//                 break;

//             case OPERATOR_MINUS:
//                 break;
    
//             case OPERATOR_MUL:
//                 break;
//         }
//     }

//     return SIMPLIFY_RETURN_SUCCESS;
// }

// =============================== HELPERS ====================================

static simplify_return_e 
SimplifyMultiplicationOnNull(derivative_t derivative,
                             ssize_t*     current_node)
{
    ASSERT(derivative != NULL);

    node_s node = derivative->ariphmetic_tree->nodes_array[*current_node];

    if (!(CheckIfConstNum(derivative, node.right_index, 0) 
        || CheckIfConstNum(derivative, node.left_index, 0))) 
    {
        return SIMPLIFY_RETURN_SUCCESS;
    }

    node_s zero_node = {};
    zero_node.parent_index = node.parent_index;
    zero_node.parent_connection = node.parent_connection;
    zero_node.left_index = NO_LINK;
    zero_node.right_index = NO_LINK;
    zero_node.node_value = {.expression = {.constant = 0}, 
                            .expression_type = EXPRESSION_TYPE_CONST};

    if (DeleteSubgraph(derivative->ariphmetic_tree, *current_node) != 0)
    {
        return SIMPLIFY_RETURN_TREE_ERROR;
    }

    if (TreeAddNode(derivative->ariphmetic_tree, &zero_node) != 0)
    {
        return SIMPLIFY_RETURN_TREE_ERROR;
    }

    *current_node = (ssize_t) zero_node.index_in_tree;

    return SIMPLIFY_RETURN_SUCCESS;
}

static simplify_return_e
SimplifySumWithNull(derivative_t derivative,
                    ssize_t      current_node)
{
    ASSERT(derivative != NULL);

    node_s node = (derivative->ariphmetic_tree->nodes_array[current_node]);

    if (CheckIfConstNum(derivative, node.right_index, 0))      
    {
        if (ForceConnect(derivative->ariphmetic_tree, node.left_index, 
                         node.parent_index, node.parent_connection) != 0)
        {
            return SIMPLIFY_RETURN_TREE_ERROR;
        }   
    }
    else if (CheckIfConstNum(derivative, node.left_index, 0))      
    {
        if (ForceConnect(derivative->ariphmetic_tree, node.right_index, 
                         node.parent_index, node.parent_connection) != 0)
        {
            return SIMPLIFY_RETURN_TREE_ERROR;
        }   
    }

    return SIMPLIFY_RETURN_SUCCESS;
}

static simplify_return_e
SimplifyMultiplicationOnOne(derivative_t derivative,
                            ssize_t      current_node)
{
    ASSERT(derivative != NULL);

    node_s node = derivative->ariphmetic_tree->nodes_array[current_node];

    if (CheckIfConstNum(derivative, node.right_index, 1))      
    {
        if (ForceConnect(derivative->ariphmetic_tree, node.left_index, 
                            node.parent_index, node.parent_connection) != 0)
        {
            return SIMPLIFY_RETURN_TREE_ERROR;
        }   
    }
    else if (CheckIfConstNum(derivative, node.left_index, 1))      
    {
        if (ForceConnect(derivative->ariphmetic_tree, node.right_index, 
                        node.parent_index, node.parent_connection) != 0)
        {
            return SIMPLIFY_RETURN_TREE_ERROR;
        }   
    }

    return SIMPLIFY_RETURN_SUCCESS;
}

static simplify_return_e
SimplifySubstractWithNull(derivative_t derivative,
                          ssize_t      current_node)
{
    ASSERT(derivative != NULL);

    if (current_node == NO_LINK)
    {
        return SIMPLIFY_RETURN_INCORRECT_VALUE;
    }

    node_s node = derivative->ariphmetic_tree->nodes_array[current_node];
    node_s* node_array = derivative->ariphmetic_tree->nodes_array;
 
    if (CheckIfConstNum(derivative, node.right_index , 0))      
    {
        if (ForceConnect(derivative->ariphmetic_tree, node.left_index, 
                            node.parent_index, node.parent_connection) != 0)
        {
            return SIMPLIFY_RETURN_TREE_ERROR;
        }
    }
    else if (CheckIfConstNum(derivative, node.left_index, 0))      
    {
        node_array[node.left_index].node_value.expression.constant = -1;
        node_array[current_node].node_value.expression.operation = OPERATOR_MUL;
    }

    return SIMPLIFY_RETURN_SUCCESS;
}

static bool
CheckIfConstNum(const derivative_t derivative,
                ssize_t            current_node,
              double             number)
{
    ASSERT(derivative != NULL);

    node_s* node_array = derivative->ariphmetic_tree->nodes_array;

    return (CheckIfConst(derivative, current_node) 
            && CheckIfEqual(node_array[current_node].node_value.
                                expression.constant, number));  
}

static bool
CheckIfConst(const derivative_t derivative,   
             ssize_t            current_node)
{
    ASSERT(derivative != NULL);

    return derivative->ariphmetic_tree->nodes_array[current_node].
            node_value.expression_type == EXPRESSION_TYPE_CONST;
}

static bool
CheckIfOperator(const derivative_t derivative,   
                ssize_t            current_node)
{
    ASSERT(derivative != NULL);

    return derivative->ariphmetic_tree->nodes_array[current_node].
            node_value.expression_type == EXPRESSION_TYPE_OPERATOR;
}