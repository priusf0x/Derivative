#ifndef MY_STRING_H
#define MY_STRING_H

#include <stdlib.h>
#include <stdio.h>

struct string_s
{
    char*  string_source;
    size_t string_size;
};

enum recursion_return_e 
{
    RECURSION_RETURN_SUCCESS,
    RECURSION_RETURN_NIL_SYMBOL,
    RECURSION_RETURN_MISSING_SYMBOL_ERROR,
    RECURSION_RETURN_NOT_LITERAL_ERROR,
    RECURSION_RETURN_TREE_ERROR
};


// =========================== STRING_HANDLER_HELPERS =========================

size_t SkipSpaces(const char* string, size_t current_position);
size_t SkipNotSpaces(const char* string, size_t current_position);
recursion_return_e ReadName(string_s* string_name, char* input_buffer, size_t* current_position);
void PrintString(const string_s* string, FILE* file_output);
bool CheckIfSymbol(char* string, size_t* current_position, char symbol);

// ============================================================================

#endif //MY_STRING_H