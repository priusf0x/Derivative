#include "derivative_defines.h"

#include "Assert.h"
#include "derivative.h"
#include "expression.h"

#define PARENT_INDEX(__X)  derivative->ariphmetic_tree->nodes_array[__X].parent_index
#define PARENT_CONNECTION(__X)  derivative->ariphmetic_tree->nodes_array[__X].parent_connection

ssize_t
DerivativeAddNode(derivative_t      derivative,
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
                       .node_value        = *expr,
                       .index_in_tree     = 0};

    TreeAddNode(derivative->ariphmetic_tree, &new_node);

    return NO_LINK;
}

ssize_t
DerivativeCopy(derivative_t derivative,
               ssize_t      src)
{
    ASSERT(derivative != NULL);

    ssize_t new_index = 0;

    CopySubgraph(derivative->ariphmetic_tree, NO_LINK, 
                 src, &new_index, EDGE_DIR_NO_DIRECTION);

    return new_index;
}

ssize_t 
DerivativeAddOperation(derivative_t derivative,
                       ssize_t      first_node,
                       ssize_t      second_node,
                       operations_e operation)
{
    ASSERT(derivative != NULL);

    node_s op_node = {.parent_index  = NO_LINK,
                      .right_index   = second_node,
                      .left_index    = first_node,
                      .node_value    = {.expression = {.operation = operation},
                                        .expression_type      = EXPRESSION_TYPE_OPERATOR},
                      .index_in_tree = 0};

    TreeAddNode(derivative->ariphmetic_tree, &op_node);

    //NOTE -  add verificator

    return (ssize_t) op_node.index_in_tree;
}


