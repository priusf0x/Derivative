#ifndef ARIPHMETIC_OPERATIONS_H
#define ARIPHMETIC_OPERATIONS_H

#include <stdint.h>

#include "tree.h"
#include "expression.h"
#include "my_string.h"
#include "buffer.h"

// ======================== ARIPHMETIC_FUNCTION_ENUM ==========================

enum derivative_return_e 
{
    DERIVATIVE_RETURN_SUCCESS,
    DERIVATIVE_RETURN_READ_ERROR,
    DERIVATIVE_RETURN_TREE_ERROR,
    DERIVATIVE_RETURN_ALLOCATION_ERROR
};

// ========================== STRUCTS_AND_UNIONS ==============================

struct derivative_s
{
    tree_t ariphmetic_tree;
    buffer_t buffer;
};

typedef derivative_s* derivative_t;

// ========================== ARIPHMETIC_FUNCTIONS ============================

derivative_return_e DerivativeInit(derivative_t* deritative, size_t start_tree_size, const char* file_name);
derivative_return_e DerivativeDestroy(derivative_t* deritative);

// ============================================================================

#endif // ARIPHMETIC_OPERATIONS_H