#ifndef DERIVATIVE_DEFINES_H

#include <stdlib.h>

#include "derivative.h"
#include "tree.h"
            
ssize_t
DerivativeCopy(derivative_t derivative, ssize_t src);

ssize_t 
DerivativeAddConst(derivative_t derivative, double value);

ssize_t 
DerivativeAddOperation(derivative_t derivative, ssize_t first_node, 
                       ssize_t second_node, operations_e operation);

ssize_t 
DerivativeAddVar(derivative_t derivative, variable_s* value); 

// ================================ DEFINE ====================================

#define IF_DERIVATIVE_FAILED (derivative->error != DERIVATIVE_RETURN_SUCCESS)  
#define RETURN_NO_LINK_IF_ERROR if (IF_DERIVATIVE_FAILED) return NO_LINK

#define NODE(_X_) (&derivative->ariphmetic_tree->nodes_array[_X_])
#define RIGHT_INDEX(_X_) NODE(_X_)->right_index
#define LEFT_INDEX(_X_)  NODE(_X_)->left_index
#define PARENT_INDEX(_X_) NODE(_X_)->parent_index
#define PARENT_CONNECTION(_X_)  NODE(_X_)->parent_connection

#define REPLACE(_N0DE__) do {ssize_t output = (_N0DE__);\
                        ssize_t parent_index = NO_LINK;\
                        edge_dir_e parent_connection = EDGE_DIR_NO_DIRECTION;\
                         if (current_node != NO_LINK)\
                         {\
                            parent_index = PARENT_INDEX(current_node);\
                            parent_connection = PARENT_CONNECTION(current_node);\
                         }\
                         DeleteSubgraph(derivative->ariphmetic_tree, current_node);\
                         PARENT_CONNECTION(output) = parent_connection;\
                         PARENT_INDEX(output) = parent_index;\
                         return output;\
                        } while (0)

#define D__(_X_) TakeExpressionDerivative(derivative, (_X_))
#define COPY__(_SRC_) DerivativeCopy(derivative, (_SRC_))

#define c_R COPY__(RIGHT_INDEX(current_node))
#define c_L COPY__(LEFT_INDEX(current_node))

#define R_O derivative->ariphmetic_tree->nodes_array[current_node].right_index 
#define L_O derivative->ariphmetic_tree->nodes_array[current_node].left_index
#define GET_CONST_VAL__(_X__)  derivative->ariphmetic_tree->nodes_array[_X__]\
                                    .node_value.expression.constant

#define CONST__(_CONST_) DerivativeAddConst(derivative, (_CONST_))   
#define VAR__(_VAR_)     DerivativeAddVar(derivative, (_VAR_))

#define SUM__(_FIRST_, _SECOND_) DerivativeAddOperation(derivative, (_FIRST_),\
                                                     (_SECOND_),  OPERATOR_PLUS)
#define SUB__(_FIRST_, _SECOND_) DerivativeAddOperation(derivative, (_FIRST_),\
                                                      (_SECOND_), OPERATOR_MINUS)
#define MUL__(_FIRST_, _SECOND_) DerivativeAddOperation(derivative, (_FIRST_),\
                                                      (_SECOND_), OPERATOR_MUL)
#define DIV__(_FIRST_, _SECOND_) DerivativeAddOperation(derivative, (_FIRST_),\
                                                      (_SECOND_), OPERATOR_DIV)   
#define POW__(_FIRST_, _SECOND_) DerivativeAddOperation(derivative, (_FIRST_),\
                                                      (_SECOND_), OPERATOR_POWER)
#define SIN__(_FIRST_)           DerivativeAddOperation(derivative, (_FIRST_),\
                                                      NO_LINK,    OPERATOR_SIN)
#define COS__(_FIRST_)           DerivativeAddOperation(derivative, (_FIRST_),\
                                                      NO_LINK,    OPERATOR_COS)
#define EXP__(_FIRST_)           DerivativeAddOperation(derivative, (_FIRST_),\
                                                      NO_LINK,    OPERATOR_EXP)
                                                    
#endif // DERIVATIVE_DEFINES