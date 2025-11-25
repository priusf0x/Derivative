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
                          size_t       current_node);

simplify_return_e
SimplifyNeutralMultipliers(derivative_t derivative,
                           ssize_t      current_node)
{
    ASSERT(derivative);

    if (current_node == NO_LINK)
    {
        return SIMPLIFY_RETURN_INCORRECT_VALUE;
    }

    TreeDump(derivative->ariphmetic_tree);

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
    if (current_node == NO_LINK)
    {
        return SIMPLIFY_RETURN_INCORRECT_VALUE;
    }

    if (node.node_value.expression_type == EXPRESSION_TYPE_OPERATOR)
    {
        if (node.node_value.expression.operation == OPERATOR_MUL)
        {
            if ((output = SimplifyMultiplicationOnNull(derivative, &current_node))
                != SIMPLIFY_RETURN_SUCCESS)
            {
                return output;
            }

            if (CheckIfEqual(derivative->ariphmetic_tree->nodes_array[current_node]
                                .node_value.expression.constant, 0))
            {
                return SIMPLIFY_RETURN_SUCCESS;
            }

            if ((output = SimplifyMultiplicationOnOne(derivative, current_node))
                != SIMPLIFY_RETURN_SUCCESS)
            {
                return output;
            }
        }

        if (node.node_value.expression.operation == OPERATOR_PLUS)
        {
            if ((output = SimplifySumWithNull(derivative, current_node))
                != SIMPLIFY_RETURN_SUCCESS)
            {
                return output;
            }
        }

        if (node.node_value.expression.operation == OPERATOR_MINUS)
        {
            if ((output = SimplifySubstractWithNull(derivative, current_node))
                != SIMPLIFY_RETURN_SUCCESS)
            {
                return output;
            }
        }
    }

    return SIMPLIFY_RETURN_SUCCESS;
}

simplify_return_e
SimplifyDoubles(derivative_t derivative,
                ssize_t      current_node)
{
    ASSERT(derivative != NULL);



    return SIMPLIFY_RETURN_SUCCESS;
}

// =============================== HELPERS ====================================

static simplify_return_e 
SimplifyMultiplicationOnNull(derivative_t derivative,
                             ssize_t*     current_node)
{
    ASSERT(derivative != NULL);

    node_s* node = &(derivative->ariphmetic_tree->nodes_array[*current_node]);

    if (!(CheckIfConstNum(derivative, node->right_index, 0) 
        || CheckIfConstNum(derivative, node->left_index, 0))) 
    {
        return SIMPLIFY_RETURN_SUCCESS;
    }

    node_s zero_node = {};
    zero_node.parent_index = node->parent_index;
    zero_node.parent_connection = node->parent_connection;
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
                          size_t       current_node)
{
    ASSERT(derivative != NULL);

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

    return (node_array[current_node].node_value.expression_type == EXPRESSION_TYPE_CONST 
            && CheckIfEqual(node_array[current_node].node_value.expression.constant, number));
}
