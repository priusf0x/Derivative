#include "simplify.h"

#include <stdlib.h>

#include "Assert.h"
#include "derivative_defines.h"
#include "derivative.h"
#include "latex_dump.h"
#include "expression.h"
#include "math.h"
#include "tools.h"

// ======================= SIMPLIFICATION_HELPERS =============================

inline static bool
CheckIfType(const derivative_t derivative,   
            ssize_t            current_node,
            expression_type_e  expression)
{;return derivative->ariphmetic_tree->nodes_array[current_node].
                        node_value.expression_type == expression;}
#define CHECK_IF_TYPE(_EXPR_, _NODE_) CheckIfType(derivative, _NODE_, _EXPR_)

inline static bool
CheckIfOp(const derivative_t derivative,   
          ssize_t            current_node,
          operations_e       operation)
{return derivative->ariphmetic_tree->nodes_array[current_node].
                        node_value.expression.operation == operation;}
#define CHECK_OP(_OP_, _NODE_) CheckIfOp(derivative, _NODE_, _OP_)

static bool 
CheckNodeConstAndEq(const derivative_t derivative,
                    ssize_t            node,
                    double             num)
{
    double node_value = derivative->ariphmetic_tree->nodes_array[node].
                                            node_value.expression.constant; 
    return CHECK_IF_TYPE(EXPRESSION_TYPE_CONST, node) && 
                                                CheckIfEqual(node_value, num);
}

#define CHECK_ONE(_NODE_)  (CheckNodeConstAndEq(derivative, _NODE_, (double) 1))
#define CHECK_ZERO(_NODE_) (CheckNodeConstAndEq(derivative, _NODE_, (double) 0))

static ssize_t
SimplifyNeutralMultipliers(derivative_t derivative, ssize_t current_node);
static ssize_t
SimplifyConst(derivative_t derivative, ssize_t current_node);

// =========================== SIMPLIFICATION =================================

derivative_return_e
SimplifyGraph(derivative_t derivative)
{
    ASSERT(derivative != NULL);

    size_t previous_size = 0;
    size_t current_size = 0;

    do
    {
        previous_size = current_size;
        
        LogDeritativeInLatex(derivative, 0, NULL);
        
        SimplifyConst(derivative, 0);
        if (IF_DERIVATIVE_FAILED)
        {
            return derivative->error;
        }
        SimplifyNeutralMultipliers(derivative, 0);
        if (IF_DERIVATIVE_FAILED)
        {
            return derivative->error;
        }

        current_size = derivative->ariphmetic_tree->nodes_count;

    } while (previous_size != current_size);

    
    return DERIVATIVE_RETURN_SUCCESS;
}   

static void  
ChangeChildNode(derivative_t derivative,
                ssize_t      current_node,
                ssize_t (*recursive_function) (derivative_t, ssize_t))
{
    node_s* node = derivative->ariphmetic_tree->nodes_array +current_node;

    ssize_t new_right = recursive_function(derivative, node->right_index);
    node = derivative->ariphmetic_tree->nodes_array + current_node;
    if (new_right != NO_LINK)
    {
        NODE(new_right)->parent_index = current_node;
        NODE(new_right)->parent_connection = EDGE_DIR_RIGHT;
    }

    ssize_t new_left = recursive_function(derivative, node->left_index);
    node = derivative->ariphmetic_tree->nodes_array + current_node;
    if (new_left != NO_LINK)
    {
        NODE(new_left)->parent_index = current_node;
        NODE(new_left)->parent_connection = EDGE_DIR_LEFT;
    }

    node->left_index = new_left;
    node->right_index = new_right;
}

// ======================= CONSTANT_SIMPLIFICATION ============================

inline static ssize_t
SimplifySum(derivative_t derivative, ssize_t current_node)
{ REPLACE(CONST__(GET_CONST_VAL__(L_O) + GET_CONST_VAL__(R_O))); } 

inline static ssize_t
SimplifySub(derivative_t derivative, ssize_t current_node)
{ REPLACE(CONST__(GET_CONST_VAL__(L_O) - GET_CONST_VAL__(R_O))); } 

inline static ssize_t
SimplifyMul(derivative_t derivative, ssize_t current_node)
{ REPLACE(CONST__(GET_CONST_VAL__(L_O) * GET_CONST_VAL__(R_O))); } 

inline static ssize_t
SimplifyDiv(derivative_t derivative, ssize_t current_node)
{ REPLACE(CONST__(GET_CONST_VAL__(L_O) / GET_CONST_VAL__(R_O))); } 

inline static ssize_t
SimplifySin(derivative_t derivative, ssize_t current_node)
{ REPLACE(CONST__(sin(GET_CONST_VAL__(L_O)))); } 

inline static ssize_t
SimplifyCos(derivative_t derivative, ssize_t current_node)
{ REPLACE(CONST__(cos(GET_CONST_VAL__(L_O)))); } 

inline static ssize_t
SimplifyPow(derivative_t derivative, ssize_t current_node)
{ REPLACE(CONST__(pow(GET_CONST_VAL__(L_O), GET_CONST_VAL__(R_O)))); } 

inline static ssize_t
SimplifyLn(derivative_t derivative, ssize_t current_node)
{ REPLACE(CONST__(log(GET_CONST_VAL__(L_O)))); } 

inline static ssize_t
SimplifyExp(derivative_t derivative, ssize_t current_node)
{ REPLACE(CONST__(exp(GET_CONST_VAL__(L_O)))); } 

inline static ssize_t
SimplifyTg(derivative_t derivative, ssize_t current_node)
{ REPLACE(CONST__(tan(GET_CONST_VAL__(L_O)))); } 

inline static ssize_t
SimplifyCtg(derivative_t derivative, ssize_t current_node)
{ REPLACE(CONST__(1 / tan(GET_CONST_VAL__(L_O)))); } 

struct op_calculation  
{
    operations_e operation;
    ssize_t (*calc_function) (derivative_t, ssize_t);
};

#define _CALCULATION_
#include "operation_info.h"

static ssize_t
SimplifyConst(derivative_t derivative,
              ssize_t      current_node)
{
    ASSERT(derivative);

    if (current_node == NO_LINK)
    {
        return NO_LINK;
    }

    #ifndef NDEBUG
        TreeDump(derivative->ariphmetic_tree);
    #endif

    ChangeChildNode(derivative, current_node, SimplifyConst);

    RETURN_NO_LINK_IF_ERROR;

    expression_u node_value = derivative->ariphmetic_tree->
                                nodes_array[current_node].node_value.expression;

    if (CHECK_IF_TYPE(EXPRESSION_TYPE_OPERATOR, current_node))
    {
        if (node_value.operation == OPERATOR_UNDEFINED)
        {
            derivative->error = DERIVATIVE_RETURN_UNDEFINED_OPERATION;
            return NO_LINK;
        }

        if (((R_O == NO_LINK) || CHECK_IF_TYPE(EXPRESSION_TYPE_CONST, R_O))
            && ((L_O == NO_LINK || CHECK_IF_TYPE(EXPRESSION_TYPE_CONST, L_O))))
        {
            return OPERATION_INFO[node_value.operation]
                        .op_function(derivative, current_node);
        }
    }

    return current_node;
}

// ========================== NEUTRAL_SIMPLIFICATION ==========================

static ssize_t
SimplifyMultiplicationOnZero(derivative_t derivative,
                             ssize_t      current_node)
{ REPLACE(CONST__(0)); }

static ssize_t
SimplifySumWithZero(derivative_t derivative,
                    ssize_t      current_node)
{
    node_s node = derivative->ariphmetic_tree->nodes_array[current_node];

    if (CHECK_ZERO(node.left_index))
    {
        REPLACE(c_R);
    }
    else if(CHECK_ZERO(node.right_index))
    {
        REPLACE(c_L);
    }

    return NO_LINK;
}

static ssize_t
SimplifySubWithZero(derivative_t derivative,
                    ssize_t      current_node)
{
    node_s node = derivative->ariphmetic_tree->nodes_array[current_node];

    if (CHECK_ZERO(node.left_index))
    {
        REPLACE(MUL__(CONST__(-1), c_R));
    }
    else if(CHECK_ZERO(node.right_index))
    {
        REPLACE(c_L);
    }

    return NO_LINK;
}

static ssize_t
SimplifyMulOnOne(derivative_t derivative,
                 ssize_t      current_node)
{
    node_s node = derivative->ariphmetic_tree->nodes_array[current_node];

    if (CHECK_ONE(node.left_index))
    {
        REPLACE(c_R);
    }
    else if(CHECK_ONE(node.right_index))
    {
        REPLACE(c_L);
    }

    return NO_LINK;
}

static ssize_t
SimplifyPowOne(derivative_t derivative,
               ssize_t      current_node)
{
    REPLACE(c_L);
}

static ssize_t
SimplifyZeroDiv(derivative_t derivative,
                ssize_t      current_node)
{
    REPLACE(CONST__(0));
}

static ssize_t
SimplifyNeutralMultipliers(derivative_t derivative,
                           ssize_t      current_node)
{
    ASSERT(derivative);
 
    RETURN_NO_LINK_IF_ERROR;
    
    if (current_node == NO_LINK)
    {
        return NO_LINK;
    }

    #ifndef NDEBUG
        TreeDump(derivative->ariphmetic_tree);
    #endif

    node_s* node = &(derivative->ariphmetic_tree->nodes_array[current_node]);
    
    ChangeChildNode(derivative, current_node, SimplifyNeutralMultipliers);

    RETURN_NO_LINK_IF_ERROR;

    if (CHECK_IF_TYPE(EXPRESSION_TYPE_OPERATOR, current_node))
    {
        if (CHECK_OP(OPERATOR_MUL, current_node) &&     
            (CHECK_ZERO(node->left_index) || CHECK_ZERO(node->right_index))) 
        {
            return SimplifyMultiplicationOnZero(derivative, current_node);
        }
        else if (CHECK_OP(OPERATOR_PLUS, current_node) &&
                 (CHECK_ZERO(node->left_index) || CHECK_ZERO(node->right_index)))
        {
            return SimplifySumWithZero(derivative, current_node);
        }
        else if (CHECK_OP(OPERATOR_MINUS, current_node) &&
                 (CHECK_ZERO(node->left_index) || CHECK_ZERO(node->right_index)))
        {
            return SimplifySubWithZero(derivative, current_node);
        }   
        else if (CHECK_OP(OPERATOR_MUL, current_node) &&
                 (CHECK_ONE(node->left_index) || CHECK_ONE(node->right_index))) 
        {
            return SimplifyMulOnOne(derivative, current_node);
        }
        else if (CHECK_OP(OPERATOR_DIV, current_node) &&
                 CHECK_ZERO(node->left_index)) 
        {
            return SimplifyZeroDiv(derivative, current_node);
        }
        else if (CHECK_OP(OPERATOR_POWER, current_node) &&
                 CHECK_ONE(node->right_index)) 
        {
            return SimplifyPowOne(derivative, current_node);
        }
    }

    return current_node;
}

// ============================ UNDEFINITION ==================================

#undef CHECK_ONE
#undef CHECK_ZERO
