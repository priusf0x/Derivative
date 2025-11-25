#ifndef TAKE_DERIVATIVE_H
#define TAKE_DERIVATIVE_H

#include "derivative.h"

enum take_derivative_return_e
{   
    TAKE_DERIVATIVE_RETURN_SUCCESS,
    TAKE_DERIVATIVE_UNDEFINED_FUNCTION,
    TAKE_DERIVATIVE_TREE_ERROR,
    TAKE_DERIVATIVE_INCORRECT_EXPRESSION,
    TAKE_DERIVATIVE_INVALID_VALUE
};


take_derivative_return_e TakeDerivative(derivative_t derivative, ssize_t current_node);

#endif //TAKE_DERIVATIVE_H