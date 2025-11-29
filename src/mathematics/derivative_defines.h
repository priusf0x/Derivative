#ifndef DERIVATIVE_DEFINES_H

#include <stdlib.h>

#include "derivative.h"
#include "tree.h"

ssize_t
DerivativeAddNode(derivative_t derivative, ssize_t current_node,
                  expression_s* expr, edge_dir_e dir);
            
ssize_t
DerivativeCopy(derivative_t derivative, ssize_t src);

ssize_t 
DerivativeAddConst(derivative_t derivative, double value);

ssize_t 
DerivativeAddOperation(derivative_t derivative, ssize_t first_node, 
                       ssize_t second_node, operations_e operation);

ssize_t 
DerivativeAddVar(derivative_t derivative, char value); 

// ================================ DEFINE ====================================

#define RIGHT_INDEX(__X) derivative->ariphmetic_tree->nodes_array[__X].right_index
#define LEFT_INDEX(__X)  derivative->ariphmetic_tree->nodes_array[__X].left_index

#define REPLACE(_X_) do {ssize_t output = (_X_);\
                         DeleteSubgraph(derivative->ariphmetic_tree,\
                                        current_node);\
                         return output;\
                        } while (0);

#define D(_X_) TakeDerivative(derivative, (_X_))
#define COPY(_SRC_) DerivativeCopy(derivative, (_SRC_))

#define cR COPY(RIGHT_INDEX(current_node))
#define cL COPY(LEFT_INDEX(current_node))

#define CONST(_CONST_) DerivativeAddConst(derivative, (_CONST_))   
#define VAR(_VAR_)     DerivativeAddVar(derivative, (_VAR_))

#define SUM(_FIRST_, _SECOND_) DerivativeAddOperation(derivative, (_FIRST_),\
                                                     (_SECOND_), OPERATOR_PLUS)\

#define SUB(_FIRST_, _SECOND_) DerivativeAddOperation(derivative, (_FIRST_),\
                                                      (_SECOND_), OPERATOR_MINUS)

#define MUL(_FIRST_, _SECOND_) DerivativeAddOperation(derivative, (_FIRST_),\
                                                      (_SECOND_), OPERATOR_MUL)

#define DIV(_FIRST_, _SECOND_) DerivativeAddOperation(derivative, (_FIRST_),\
                                                      (_SECOND_), OPERATOR_DIV)   

#define POW(_FIRST_, _SECOND_) DerivativeAddOperation(derivative, (_FIRST_),\
                                                      (_SECOND_), OPERATOR_POWER)

#define SIN(_FIRST_)           DerivativeAddOperation(derivative, (_FIRST_),\
                                                      NO_LINK, OPERATOR_SIN)

#define COS(_FIRST_)           DerivativeAddOperation(derivative, (_FIRST_),\
                                                      NO_LINK, OPERATOR_COS)

#define EXP(_FIRST_)           DerivativeAddOperation(derivative, (_FIRST_),\
                                                      NO_LINK, OPERATOR_EXP)
                                                    
#endif // DERIVATIVE_DEFINES