#ifndef RECURSIVE_DECENT_H
#define RECURSIVE_DECENT_H

#include "derivative.h"
#include "buffer.h"

enum recursive_decent_return_e
{
    RECURSIVE_DECENT_SUCCESS,
};


recursive_decent_return_e
ConvertToGraph(derivative_t derivative);

#endif // RECURSIVE_DECENT_H