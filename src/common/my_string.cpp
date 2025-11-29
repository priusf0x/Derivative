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

bool 
ExStrCmp(const char* string, 
         const char* example, 
         size_t      lenght, 
         const char* end_symbols)
{
    return (strncmp(string, example, lenght) == 0) 
            && strchr(end_symbols, *(string + lenght));
}