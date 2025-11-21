#ifndef ARIPHMETIC_OPERATIONS_H
#define ARIPHMETIC_OPERATIONS_H

#include <stdint.h>

#include "tree.h"
#include "expression.h"
#include "my_string.h"

// ======================== ARIPHMETIC_FUNCTION_ENUM ==========================

enum deritative_return_e 
{
    DERITATIVE_RETURN_SUCCESS,
    DERITATIVE_RETURN_SYSTEM_CALL_ERROR,
    DERITATIVE_RETURN_TREE_ERROR,
    DERITATIVE_RETURN_OPEN_FILE_ERROR,
    DERITATIVE_RETURN_READ_FILE_ERROR,
    DERITATIVE_RETURN_CLOSE_FILE_ERROR,
    DERITATIVE_RETURN_EMPTY_FILE,
    DERITATIVE_RETURN_ALLOCATION_ERROR,
    DERITATIVE_RETURN_MISSING_SYMBOL
};

// ========================== STRUCTS_AND_UNIONS ==============================

struct deritative_s
{
    tree_t ariphmetic_tree;
    char*  buffer;
};

typedef deritative_s* deritative_t;

// ========================== ARIPHMETIC_FUNCTIONS ============================

deritative_return_e DeritativeInit(deritative_t* deritative, size_t start_tree_size, const char* file_name);
deritative_return_e DeritativeDestroy(deritative_t* deritative);

// ============================================================================

#endif // ARIPHMETIC_OPERATIONS_H