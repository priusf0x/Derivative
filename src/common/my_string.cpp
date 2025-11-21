#include "my_string.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "Assert.h"
#include "expression.h"

size_t
SkipSpaces(const char* string,  
           size_t      current_position)
{
    ASSERT(string != NULL);

    char character = *(string + current_position);

    while ((isspace(character)) && (character != '\0'))
    {
        current_position++;
        character = *(string + current_position);
    }

    return current_position;
}

size_t
SkipNotSpaces(const char* string,
              size_t      current_position)
{
    ASSERT(string != NULL);

    char character = *(string + current_position);

    while (!isspace(character) && (character != '\0'))
    {
        current_position++;
        character = *(string + current_position);
    }

    return current_position;
}

static bool 
CheckIfNil(const char* string,  // add current position 
           size_t*     current_position)
{
    ASSERT(string != NULL);
    ASSERT(current_position != NULL);

    const char* stop_word = "nil";

    if (strncmp(string + *current_position, stop_word, strlen(stop_word)) == 0) 
    {
        (*current_position) += strlen(stop_word);
        return true;
    }

    return false;
}

read_return_e
ReadName(string_s* string_name,
         char*     input_buffer,
         size_t*   current_position)
{
    ASSERT(string_name);
    ASSERT(input_buffer);
    ASSERT(current_position);

    *current_position = SkipSpaces(input_buffer, *current_position);

    char* current_pointer = input_buffer + *current_position;

    if ((*current_pointer == '(') || (*current_pointer == ')'))
    {
        return READ_RETURN_NOT_LITERAL_ERROR;
    }
    else if (CheckIfNil(input_buffer, current_position))
    {
        return READ_RETURN_NIL_SYMBOL;
    }
    else if ((*current_pointer != '\"'))
    {
        return READ_RETURN_NOT_LITERAL_ERROR;
    }

    (*current_position)++;
    current_pointer++;

    char* close_position = strchr(input_buffer + *current_position, '\"');

    if (close_position == NULL)
    {
        return READ_RETURN_MISSING_SYMBOL_ERROR;
    }

    string_name->string_size = (size_t) (close_position - current_pointer);
    string_name->string_source = current_pointer;
    *current_position += string_name->string_size + 1;
    *current_position = SkipSpaces(input_buffer, *current_position);

    return READ_RETURN_SUCCESS;
}

bool 
CheckIfSymbol(char*   string,
              size_t* current_position,
              char    symbol)
{
    *current_position = SkipSpaces(string, *current_position);
    if (*(string + *current_position) == symbol)
    {
        *current_position = SkipSpaces(string, *current_position + 1);
        return true;
    }

    return false;
}

void
PrintString(const string_s* string,
            FILE*           file_output)
{
    ASSERT(string != NULL);
    ASSERT(file_output != NULL);

    fprintf(file_output, "\"");
    fwrite(string->string_source,
           sizeof(char),
           string->string_size,
           file_output);
    fprintf(file_output, "\" ");
}

// ========================= CLASSIFICATION_OF_STRING =========================

operations_e
CheckIfOperator(string_s* string)
{
    ASSERT(string != NULL);
     
    for (size_t index = 1; index < OPERATION_COUNT; index++)
    {
        if (string->string_size == strlen(OPERATION_STR_ARRAY[index]))
        {
            if (strncmp(string->string_source, OPERATION_STR_ARRAY[index], 
                        string->string_size) == 0)
            {
                return (operations_e) index;
            }
        }
    }

    return OPERATOR_UNDEFINED;
}

char
CheckIfVar(string_s* string)
{
    if ((string->string_size == 1) && (isalpha(*string->string_source)))
    {
        return *string->string_source;
    }

    return '\0';
}

bool 
CheckIfDouble(string_s* string, double* number)
{
    ASSERT(string != NULL);
    ASSERT(number != NULL);

    char* last_char = NULL;

    *number = strtod(string->string_source, &last_char);

    if ((last_char == NULL) 
        || (size_t) (last_char - string->string_source) == string->string_size)
    {
        return true;
    }

    return false;
}