#include "derivative.h"

#include <sys/stat.h>   
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "Assert.h"
#include "my_string.h"
#include "tree.h"
#include "tools.h"
#include "buffer.h"

// ======================== MEMORY_CONTROLLING ================================

derivative_return_e 
DerivativeInit(derivative_t* derivative,
               size_t        start_tree_size,
               const char*   file_name)
{
    ASSERT(derivative != NULL);
    ASSERT(file_name != NULL);

    *derivative = (derivative_t) calloc(1, sizeof(derivative_s));
    
    if (*derivative == NULL)
    {
        return DERIVATIVE_RETURN_ALLOCATION_ERROR;
    }

    if (TreeInit(&(*derivative)->ariphmetic_tree, start_tree_size) != 0)
    {
        free(*derivative);
        
        return DERIVATIVE_RETURN_TREE_ERROR;
    }

    derivative_return_e output = DERIVATIVE_RETURN_SUCCESS;

    if (BufferInit(&(*derivative)->buffer, file_name) != 0)
    {
        TreeDestroy((*derivative)->ariphmetic_tree);
        free(*derivative);

        return output;
    }

    return DERIVATIVE_RETURN_SUCCESS;
}

derivative_return_e
DerivativeDestroy(derivative_t* derivative)
{
    if ((derivative != NULL) && (*derivative != NULL))
    {
        BufferDestroy(&(*derivative)->buffer);
        
        TreeDestroy((*derivative)->ariphmetic_tree);
        free(*derivative);
    }

    return DERIVATIVE_RETURN_SUCCESS;
}