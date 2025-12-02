#ifndef NAME_SPACE_H
#define NAME_SPACE_H

#include "my_string.h"

enum name_table_return_e
{
    NAME_TABLE_RETURN_SUCCESS,
    NAME_TABLE_RETURN_ALLOCATION_ERROR,
    NAME_TABLE_RETURN_INCORRECT_VALUE
};

typedef ssize_t name_value_t;
struct name_s 
{
    string_s     name_string;
    unsigned int hash;
    name_value_t value;
    ssize_t      next_element;
    ssize_t      prev_element;
};

struct name_table_s
{    
    name_s* name_array;
    ssize_t next_free;
    size_t  name_count;
    size_t  name_table_capacity;
};
typedef name_table_s* name_table_t;

name_table_return_e InitNameTable(name_table_t* name_table, size_t start_size);
name_table_return_e DestroyNameTable(name_table_t* name_table);\
void NameTableDump(name_table_t name_table);

name_table_return_e AddNameInTable(string_s* string, name_value_t value,
                                   name_table_t name_table);

#endif // NAME_SPACE_H