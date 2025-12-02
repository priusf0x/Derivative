#include "derivative_defines.h"

#include "Assert.h"
#include "derivative.h"
#include "expression.h"
#include "tools.h"

ssize_t
DerivativeCopy(derivative_t derivative,
               ssize_t      src)
{
    ASSERT(derivative != NULL);
    RETURN_NO_LINK_IF_ERROR;

    ssize_t new_index = 0;

    if (CopySubgraph(derivative->ariphmetic_tree, NO_LINK, 
                     src, &new_index, EDGE_DIR_NO_DIRECTION) != 0)
    {
        derivative->error = DERIVATIVE_RETURN_TREE_ERROR;
    }

    return new_index;
}

ssize_t 
DerivativeAddConst(derivative_t derivative,
                   double       value)
{
    ASSERT(derivative != NULL);
    RETURN_NO_LINK_IF_ERROR;

    node_s const_node = {.parent_index  = NO_LINK,
                      .right_index   = NO_LINK,
                      .left_index    = NO_LINK,
                      .node_value    = {.expression = {.constant = value},
                                        .expression_type = EXPRESSION_TYPE_CONST},
                      .index_in_tree = NO_LINK};    

    if (TreeAddNode(derivative->ariphmetic_tree, &const_node) != 0)
    {
        derivative->error = DERIVATIVE_RETURN_TREE_ERROR;
    }

    return const_node.index_in_tree;
}

ssize_t 
DerivativeAddVar(derivative_t derivative,
                 variable_s*  variable)
{ //FIXME -  add work with name space here
    ASSERT(derivative != NULL);
    RETURN_NO_LINK_IF_ERROR;

    node_s var_node = {.parent_index  = NO_LINK,
                      .right_index   = NO_LINK,
                      .left_index    = NO_LINK,
                      .node_value    = {.expression = {.variable = *variable},
                                        .expression_type = EXPRESSION_TYPE_VAR},
                      .index_in_tree = NO_LINK};    

    if (TreeAddNode(derivative->ariphmetic_tree, &var_node) != 0)
    {
        derivative->error = DERIVATIVE_RETURN_TREE_ERROR;
    }

    return var_node.index_in_tree;
}

ssize_t 
DerivativeAddOperation(derivative_t derivative,
                       ssize_t      first_node,
                       ssize_t      second_node,
                       operations_e operation)
{
    ASSERT(derivative != NULL);
    RETURN_NO_LINK_IF_ERROR;

    node_s op_node = {.parent_index  = NO_LINK,
                      .right_index   = second_node,
                      .left_index    = first_node,
                      .node_value    = {.expression = {.operation = operation},
                                        .expression_type = EXPRESSION_TYPE_OPERATOR},
                      .index_in_tree = NO_LINK};    

    if (TreeAddNode(derivative->ariphmetic_tree, &op_node) != 0)
    {
        derivative->error = DERIVATIVE_RETURN_TREE_ERROR;
    }

    return op_node.index_in_tree;
}
