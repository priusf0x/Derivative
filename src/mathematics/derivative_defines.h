#ifndef DERIVATIVE_DEFINES_H

#include <stdlib.h>

#include "derivative.h"

ssize_t
DerivativeAddNode(derivative_t derivative, ssize_t current_node,
                  expression_s* expr, edge_dir_e dir);
            
ssize_t
DerivativeCopy(derivative_t derivative, ssize_t src);

ssize_t 
DerivativeAddOperation(derivative_t derivative, ssize_t first_node, 
                       ssize_t second_node, operations_e operation);

// ================================ DEFINE ====================================

#define REPLACE(_X_) do {ssize_t output = (_X_);\
                         ForceConnect(derivative->ariphmetic_tree, output,\
                                      PARENT_INDEX(current_node),\
                                      PARENT_CONNECTION(current_node));\
                         return output;\
                        } while (0);

#define D(_X_) TakeDerivative(derivative, (_X_))
#define COPY(_SRC_) DerivativeCopy(derivative, (_SRC_))

#define cR COPY(RIGHT_INDEX(current_node))
#define cL COPY(LEFT_INDEX(current_node))

#define SUM(_FIRST_, _SECOND_) DerivativeAddOperation(derivative, (_FIRST_),\
                                                      (_SECOND_), OPERATOR_PLUS)

#define SUB(_FIRST_, _SECOND_) DerivativeAddOperation(derivative, (_FIRST_),\
                                                      (_SECOND_), OPERATOR_MINUS)

#define MUL(_FIRST_, _SECOND_) DerivativeAddOperation(derivative, (_FIRST_),\
                                                      (_SECOND_), OPERATOR_MUL)

#define DIV(_FIRST_, _SECOND_) DerivativeAddOperation(derivative, (_FIRST_),\
                                                      (_SECOND_), OPERATOR_DIV)   

#define POW(_FIRST_, _SECOND_) DerivativeAddOperation(derivative, (_FIRST_),\
                                                      (_SECOND_), OPERATOR_POWER)

#endif // DERIVATIVE_DEFINES