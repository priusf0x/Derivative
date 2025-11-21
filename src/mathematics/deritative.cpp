#include "deritative.h"

#include <sys/stat.h>   
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "Assert.h"
#include "my_string.h"
#include "tree.h"
#include "tools.h"

static deritative_return_e 
ReadFileData(deritative_t deritative,
             const char*  file_name);

static read_return_e
RecursiveParser(deritative_t deritative,
                size_t*      current_position,
                size_t       root_position);

// ======================== MEMORY_CONTROLLING ================================

deritative_return_e 
DeritativeInit(deritative_t* deritative,
               size_t        start_tree_size,
               const char*   file_name)
{
    ASSERT(deritative != NULL);
    ASSERT(file_name != NULL);

    *deritative = (deritative_t) calloc(1, sizeof(deritative_s));
    
    if (*deritative == NULL)
    {
        return DERITATIVE_RETURN_ALLOCATION_ERROR;
    }

    if (TreeInit(&(*deritative)->ariphmetic_tree, start_tree_size) != 0)
    {
        free(*deritative);

        return DERITATIVE_RETURN_TREE_ERROR;
    }

    deritative_return_e output = DERITATIVE_RETURN_SUCCESS;

    if ((output = ReadFileData(*deritative, file_name)) != 0)
    {
        TreeDestroy((*deritative)->ariphmetic_tree);
        free(*deritative);

        return output;
    }

    size_t current_position = 0;
    if (RecursiveParser(*deritative, &current_position, 0) != 0)
    {
        TreeDestroy((*deritative)->ariphmetic_tree);
        free((*deritative)->buffer);
        free(*deritative);

        return DERITATIVE_RETURN_READ_FILE_ERROR;
    }

    return DERITATIVE_RETURN_SUCCESS;
}

deritative_return_e
DeritativeDestroy(deritative_t* deritative)
{
    if ((deritative != NULL) && (*deritative != NULL))
    {   
        free((*deritative)->buffer);
        TreeDestroy((*deritative)->ariphmetic_tree);
        free(*deritative);
    }

    return DERITATIVE_RETURN_SUCCESS;
}

// ========================= INIT_HELP_FUNCTION ===============================

static deritative_return_e 
ReadFileData(deritative_t deritative,
             const char*  file_name)
{
    ASSERT(deritative != NULL);
    ASSERT(file_name != NULL);

    struct stat file_stat = {};

    if (stat(file_name, &file_stat) != 0)
    {
        return DERITATIVE_RETURN_SYSTEM_CALL_ERROR;
    }

    size_t char_number = (size_t) (file_stat.st_size);
    
    FILE* file_input = fopen(file_name , "r");
    if (file_input == NULL)
    {
        return DERITATIVE_RETURN_OPEN_FILE_ERROR;
    }

    deritative->buffer = (char*) calloc(char_number + 1, sizeof(char));
    if (deritative->buffer == NULL)
    {
        fclose(file_input);
        return DERITATIVE_RETURN_ALLOCATION_ERROR;
    }
    
    size_t read_count = fread(deritative->buffer , sizeof(char), char_number, file_input);
    
    if (fclose(file_input) != 0)
    {
        free(deritative->buffer);
        return DERITATIVE_RETURN_CLOSE_FILE_ERROR;
    }

    if (read_count == 0)
    {
        free(deritative->buffer);
        return DERITATIVE_RETURN_EMPTY_FILE;
    }
    
    deritative->buffer[read_count] = '\0';

    return DERITATIVE_RETURN_SUCCESS;
}

// ========================== READ_HELP_FUNCTIONS =============================

static expression_type_e
ReadMnemonic(string_s* string,
             node_s*   node)
{
    ASSERT(string != NULL);
    ASSERT(node != NULL);

    expression_u expr = {};

    if ((expr.operation = CheckIfOperator(string)) != OPERATOR_UNDEFINED)
    {
        node->node_value = {.expression = expr, 
                            .expression_type = EXPRESSION_TYPE_OPERATOR};

        return EXPRESSION_TYPE_OPERATOR;
    }
    else if ((expr.variable = CheckIfVar(string)) != OPERATOR_UNDEFINED)
    {
        node->node_value = {.expression = expr,
                            .expression_type = EXPRESSION_TYPE_VAR};

        return EXPRESSION_TYPE_VAR;
    }
    else if (CheckIfDouble(string, &(expr.constant)))
    {
        node->node_value = {.expression = expr,
                            .expression_type = EXPRESSION_TYPE_CONST};

        return EXPRESSION_TYPE_CONST;
    } 

    return EXPRESSION_TYPE_UNDEFINED;
}

// =========================== RECURSION_ALGORITHM ============================

static read_return_e 
ReadNode(deritative_t deritative,
         size_t*      current_position,
         size_t       root_position,
         edge_dir_e   node_position);

static read_return_e
RecursiveParser(deritative_t deritative,
                size_t*      current_position,
                size_t       root_position)
{
    ASSERT(deritative != NULL);
    ASSERT(current_position);

    if (!CheckIfSymbol(deritative->buffer, current_position, '('))
    {
        return READ_RETURN_MISSING_SYMBOL_ERROR;
    }

    read_return_e output = READ_RETURN_SUCCESS;

    output = ReadNode(deritative, current_position, root_position, EDGE_DIR_LEFT);

    if (output != READ_RETURN_SUCCESS)
    {
        return output;
    }
    
    output = ReadNode(deritative, current_position, root_position, EDGE_DIR_RIGHT);

    if (output != READ_RETURN_SUCCESS)
    {
        return output;
    }

    if (!CheckIfSymbol(deritative->buffer, current_position, ')'))
    {
        return READ_RETURN_MISSING_SYMBOL_ERROR;
    }

    return READ_RETURN_SUCCESS;
}

static read_return_e
ReadNode(deritative_t  deritative,
         size_t*       current_position,
         size_t        root_position,
         edge_dir_e    node_position)
{
    ASSERT(deritative != NULL);
    ASSERT(current_position != NULL);

    node_s node = {.parent_index = (ssize_t) root_position, 
                   .parent_connection = node_position, .right_index = -1,
                   .left_index = -1};

    string_s read_string = {};

    read_return_e output = ReadName(&read_string,  
                                    deritative->buffer, 
                                    current_position);
    
    if (output == READ_RETURN_SUCCESS)
    {
        if (ReadMnemonic(&read_string, &node) == EXPRESSION_TYPE_UNDEFINED)
        {
            return READ_RETURN_BAD_MNEMONIC;
        }

        if (TreeAddNode(deritative->ariphmetic_tree, &node) != 0)
        {
            return READ_RETURN_TREE_ERROR;
        }

        output = RecursiveParser(deritative, current_position,
                                 node.index_in_tree); 

        if (output != READ_RETURN_SUCCESS)
        {
            return output;
        }
    }
    else if (output != READ_RETURN_NIL_SYMBOL)
    {
        return output;
    }
    
    return READ_RETURN_SUCCESS;
}