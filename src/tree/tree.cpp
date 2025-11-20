#include "tree.h"

#include <stdio.h>
#include <string.h>

#include "Assert.h"
#include "stack.h"
#include "my_string.h"

static tree_return_e SetTreeSize(tree_t tree, size_t  new_size);
static tree_return_e NumerizeElements(tree_t tree, size_t start_index);

// =========================== MEMORY_CONTROLLING ===============================

tree_return_e
TreeInit(tree_t* tree,
         size_t  start_tree_size)
{
    ASSERT(tree != NULL);

    tree_return_e output = TREE_RETURN_SUCCESS;

    *tree = (tree_t) calloc(1, sizeof(tree_s));

    if (*tree == NULL)
    {
        return TREE_RETURN_ALLOCATION_ERROR;
    }

    if (SetTreeSize(*tree, start_tree_size + 1) != 0)  // for null element
    {
        return output;
    }

    //zero element creation;
    (*tree)->nodes_array[0] = {.parent_index = 1,        .parent_connection = EDGE_DIR_NO_DIRECTION,
                            .right_index  = NO_LINK,  .left_index        = NO_LINK,
                            .node_value   = 0};
    //end creating zero element;
    return TREE_RETURN_SUCCESS;
}

tree_return_e
TreeDestroy(tree_t tree)
{
    free(tree->nodes_array);
    free(tree);
    tree = NULL;

    return TREE_RETURN_SUCCESS;
}

//=============================== MEMORY_HELPERS ===============================

static void
ClearNode(tree_t tree,
          size_t current_node)
{
    tree->nodes_array[current_node].right_index = -1;
    tree->nodes_array[current_node].left_index = -1;
    tree->nodes_array[current_node].parent_connection = EDGE_DIR_NO_DIRECTION;
    tree->nodes_array[current_node].node_value = {};
}

static tree_return_e
SetTreeSize(tree_t tree,
            size_t new_size)
{
    ASSERT(tree != NULL);

    if (new_size < tree->nodes_count)
    {
        return TREE_RETURN_INCORRECT_VALUE;
    }

    if ((tree->nodes_array = (node_s*) recalloc(tree->nodes_array,
        sizeof(node_s) * (tree->nodes_count + 1),
        sizeof(node_s) * new_size)) == NULL)
    {
        return TREE_RETURN_ALLOCATION_ERROR;
    }

    tree->nodes_capacity = new_size;

    NumerizeElements(tree, tree->nodes_count + 1);

    tree->nodes_array[0].parent_index = (ssize_t) tree->nodes_count + 1;

    return TREE_RETURN_SUCCESS;
}

static tree_return_e
NumerizeElements(tree_t tree,
                 size_t start_index)
{
    ASSERT(tree != NULL);

    for(size_t index = start_index; index < tree->nodes_capacity; index++)
    {
        tree->nodes_array[index].index_in_tree = index;
        tree->nodes_array[index].parent_index = (ssize_t) index + 1;
        ClearNode(tree, index);
    }

    return TREE_RETURN_SUCCESS;
}

// ============================== ADDING NODES ================================

const uint8_t CHILD_RIGHT_USAGE = 0b0000'0001;
const uint8_t CHILD_LEFT_USAGE = 0b0000'0010;
const uint8_t INVALID_NODE =  0b0000'0100;

static uint8_t CheckTreeNode(tree_s* tree, node_s* node);
static tree_return_e ConnectNodes(tree_s* tree, node_s* node, uint8_t children_usage);
static tree_return_e TreeNormilizeSize(tree_s* tree);

tree_return_e
TreeAddNode(tree_t  tree,
            node_s* node)
{
    ASSERT(tree != NULL);
    ASSERT(node != NULL);

    uint8_t children_usage = 0b0000'0000;
    if((children_usage = CheckTreeNode(tree, node)) == INVALID_NODE)
    {
        return TREE_RETURN_INVALID_NODE;
    }

    if (TreeNormilizeSize(tree) != TREE_RETURN_SUCCESS)
    {
        return TREE_RETURN_ALLOCATION_ERROR;
    }

    node->index_in_tree = (size_t) tree->nodes_array[0].parent_index;
    tree->nodes_array[0].parent_index =
        tree->nodes_array[node->index_in_tree].parent_index;

    ConnectNodes(tree, node, children_usage);

    tree->nodes_count++;

    return TREE_RETURN_SUCCESS;
}

// ============================== ADD_NODES_HELPERS ===========================

static uint8_t
CheckTreeNode(tree_t  tree,
              node_s* node)
{
    ASSERT(tree != NULL);
    ASSERT(node != NULL);

    ssize_t child_left = node->left_index;
    ssize_t child_right = node->right_index;
    ssize_t parent = node->parent_index;

    uint8_t output = 0b0000'0000;

    if ((parent >= (ssize_t) tree->nodes_capacity)     // check user's values
        || (child_left >= (ssize_t) tree->nodes_capacity)
        || (child_right > (ssize_t) tree->nodes_capacity))
    {
        return INVALID_NODE;
    }
    else if (node->parent_connection == EDGE_DIR_NO_DIRECTION)
    {
        return INVALID_NODE;
    }
    else if (parent == NO_LINK)    // check parent if he is null
    {
        return INVALID_NODE;
    }
    else if ((child_left != NO_LINK) && (output |= CHILD_LEFT_USAGE)
              && (child_left == (ssize_t) node->index_in_tree
                  || child_left == 0))
    {
        return INVALID_NODE;
    }
    else if ((child_right != NO_LINK) && (output |= CHILD_RIGHT_USAGE)
              && (child_right == (ssize_t) node->index_in_tree
                  || child_right == 0))
    {
        return INVALID_NODE;
    }
    else if (output == 0b0000'0000)
    {
        if ((node->parent_connection == EDGE_DIR_RIGHT) &&
            (tree->nodes_array[parent].right_index != NO_LINK))
        {
            return INVALID_NODE;
        }
        else if ((node->parent_connection == EDGE_DIR_LEFT)
                 && (tree->nodes_array[parent].left_index != NO_LINK))
        {
            return INVALID_NODE;
        }
    }

    return output;
}

static tree_return_e
TreeNormilizeSize(tree_t tree)
{
    ASSERT(tree != NULL);

    if (tree->nodes_capacity - 1 == tree->nodes_count)
    {
        if (SetTreeSize(tree, tree->nodes_capacity * 2) != TREE_RETURN_SUCCESS)
        {
            return TREE_RETURN_ALLOCATION_ERROR;
        }
    }

    return TREE_RETURN_SUCCESS;
}

static tree_return_e
ConnectNodes(tree_t  tree,
             node_s* node,
             uint8_t children_usage)
{
    ASSERT(tree != NULL);
    ASSERT(node != NULL);

    node_s* right_element = tree->nodes_array + node->right_index;
    node_s* left_element = tree->nodes_array + node->left_index;

    if (children_usage & CHILD_RIGHT_USAGE)
    {
        if (right_element->parent_connection == EDGE_DIR_RIGHT)
        {
            tree->nodes_array[right_element->parent_index].right_index = NO_LINK;
        }
        else if (right_element->parent_connection == EDGE_DIR_LEFT)
        {
            tree->nodes_array[right_element->parent_index].left_index = NO_LINK;
        }

        right_element->parent_index = (ssize_t) node->index_in_tree;
        right_element->parent_connection = EDGE_DIR_RIGHT;
    }
    if (children_usage & CHILD_LEFT_USAGE)
    {
        if (left_element->parent_connection == EDGE_DIR_RIGHT)
        {
            tree->nodes_array[left_element->parent_index].right_index = NO_LINK;
        }
        else if (left_element->parent_connection == EDGE_DIR_LEFT)
        {
            tree->nodes_array[left_element->parent_index].left_index = NO_LINK;
        }

        left_element->parent_index = (ssize_t) node->index_in_tree;
        left_element->parent_connection = EDGE_DIR_LEFT;
    }

    if (node->parent_connection == EDGE_DIR_RIGHT)
    {
        tree->nodes_array[node->parent_index].right_index = (ssize_t) node->index_in_tree;
    }
    else if (node->parent_connection == EDGE_DIR_LEFT)
    {
        tree->nodes_array[node->parent_index].left_index = (ssize_t) node->index_in_tree;
    }

    memcpy(tree->nodes_array + node->index_in_tree, node, sizeof(node_s));

    return TREE_RETURN_SUCCESS;
}

// ============================ ELEMENTS_ACTIONS ==============================

tree_return_e
DeleteSubgraph(tree_t tree,
               size_t node_index)
{
    ASSERT(tree != NULL);

    swag_t bypass_stack = NULL;

    const size_t stack_start_size = 10;

    if (StackInit(&bypass_stack, stack_start_size, "InDepthBypass") != 0)
    {
        return TREE_RETURN_STACK_ERROR;
    }

    if (tree->nodes_array[node_index].parent_connection == EDGE_DIR_NO_DIRECTION)
    {
        return TREE_RETURN_INVALID_NODE;
    }
    else if (tree->nodes_array[node_index].parent_connection == EDGE_DIR_RIGHT)
    {
        tree->nodes_array[tree->nodes_array[node_index].parent_index].right_index = NO_LINK;
    }
    else if (tree->nodes_array[node_index].parent_connection == EDGE_DIR_LEFT)
    {
        tree->nodes_array[tree->nodes_array[node_index].parent_index].left_index = NO_LINK;
    }

    if (StackPush(bypass_stack, node_index) != 0)
    {
        return TREE_RETURN_STACK_ERROR;
    }

    size_t current_element = 0;

    while (GetStackSize(bypass_stack) != 0)
    {
        if (StackPop(bypass_stack, &current_element))
        {
            return TREE_RETURN_STACK_ERROR;
        }

        if (tree->nodes_array[current_element].right_index != NO_LINK)
        {
            if (StackPush(bypass_stack,
                (size_t) tree->nodes_array[current_element].right_index))
            {
                return TREE_RETURN_STACK_ERROR;
            }
        }

        if (tree->nodes_array[current_element].left_index != NO_LINK)
        {
            if (StackPush(bypass_stack,
                (size_t) tree->nodes_array[current_element].left_index))
            {
                return TREE_RETURN_STACK_ERROR;
            }
        }

        ClearNode(tree, current_element);
        
        tree->nodes_array[current_element].parent_index =
            (ssize_t) tree->nodes_array[0].parent_index;
        tree->nodes_array[0].parent_index = (ssize_t) current_element;

        tree->nodes_count--;
    }

    StackDestroy(bypass_stack);

    return TREE_RETURN_SUCCESS;
}


