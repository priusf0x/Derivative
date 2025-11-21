#ifndef MY_STRING_H
#define MY_STRING_H

#include <stdlib.h>
#include <stdio.h>

#include "expression.h"

struct string_s
{
    char*  string_source;
    size_t string_size;
};

enum read_return_e 
{
    READ_RETURN_SUCCESS,
    READ_RETURN_NIL_SYMBOL,
    READ_RETURN_MISSING_SYMBOL_ERROR,
    READ_RETURN_NOT_LITERAL_ERROR,
    READ_RETURN_TREE_ERROR,
    READ_RETURN_BAD_MNEMONIC
};


// =========================== STRING_HANDLER_HELPERS =========================

size_t SkipSpaces(const char* string, size_t current_position);
size_t SkipNotSpaces(const char* string, size_t current_position);
read_return_e ReadName(string_s* string_name, char* input_buffer, size_t* current_position);
void PrintString(const string_s* string, FILE* file_output);
bool CheckIfSymbol(char* string, size_t* current_position, char symbol);
operations_e CheckIfOperator(string_s* string);
char CheckIfVar(string_s* string);
bool CheckIfDouble(string_s* string, double* number);

// ============================================================================

#endif //MY_STRING_H