#include "name_space.h"

#include <stdlib.h>

#include "Assert.h"
#include "tools.h"
#include "my_string.h"
#include "string.h"

static const ssize_t NO_LINK = -1; 

static name_table_return_e 
SetNameTableSize(name_table_t name_table, size_t new_size);

// ============================ MEMORY_CONTROL ================================

name_table_return_e 
InitNameTable(name_table_t* name_table, 
              size_t        start_size)
{
    ASSERT(name_table != NULL);

    name_table_return_e output = NAME_TABLE_RETURN_SUCCESS;

    *name_table = (name_table_t) calloc(1, sizeof(name_table_s));
    if (*name_table == NULL)
    {
        return NAME_TABLE_RETURN_ALLOCATION_ERROR;
    }

    if ((output = SetNameTableSize(*name_table, start_size)))
    {
        free(*name_table);
        *name_table = NULL;

        return output;
    }    

    return NAME_TABLE_RETURN_SUCCESS;
}

name_table_return_e
DestroyNameTable(name_table_t* name_table)
{
    if ((name_table != NULL) && (*name_table != NULL))
    {
        free((*name_table)->name_array);
        free(*name_table);
        *name_table = NULL;
    }    

    return NAME_TABLE_RETURN_SUCCESS;
}

static void 
NumerizeElements(name_table_t name_table,
                 size_t       first_free)
{
    for (size_t index = first_free; index < name_table->name_table_capacity;
            index++)
    {
        name_table->name_array[index].next_element = (ssize_t) index + 1;
        name_table->name_array[index].prev_element = NO_LINK;
    }
}


static name_table_return_e
SetNameTableSize(name_table_t name_table,
                 size_t       new_size)
{
    ASSERT(name_table != NULL);    

    if (new_size < name_table->name_table_capacity)
    {
        return NAME_TABLE_RETURN_INCORRECT_VALUE;
    }

    name_s* new_mem_block = (name_s*) recalloc(name_table->name_array,
                                               name_table->name_table_capacity
                                                 * sizeof(name_s),
                                               new_size * sizeof(name_s));

    if (new_mem_block == NULL)
    {
        return NAME_TABLE_RETURN_ALLOCATION_ERROR;        
    }

    name_table->name_array = new_mem_block;
    name_table->name_table_capacity = new_size;
    NumerizeElements(name_table, name_table->name_count);
    name_table->next_free = (ssize_t) name_table->name_count;

    return NAME_TABLE_RETURN_SUCCESS;
}

// ================================ NAME_TABLE_DUMP ===========================

static void  
PrintNameInfo(const name_s* name, size_t current_index)
{
    fprintf(stderr, "│%.2zu", current_index);
    const size_t table_str_width = 17;
    char print_string[table_str_width] = "";
    if (name->name_string.string_source != NULL)
    {
        strncpy(print_string, name->name_string.string_source, 
                    name->name_string.string_size);
    }
    
    fprintf(stderr, "│%17s",  print_string);
    fprintf(stderr, "│%7ld", name->value);
    fprintf(stderr, "│%6ld", name->next_element);
    fprintf(stderr, "│%6ld│\n", name->prev_element);

}

void 
NameTableDump(name_table_t name_table)
{
    ASSERT(name_table);

    size_t current_index = 0;
    name_s current_name = {};

    const char* table_heading = "┏━━┳━━━━━━━━━━━━━━━━━┳━━━━━━━┳━━━━━━┳━━━━━━┓\n"\
                                "┃No┃      NAME       ┃ VALUE ┃ NEXT ┃ PREV ┃\n"\
                                "┡━━┻━━━━━━━━━━━━━━━━━┻━━━━━━━┻━━━━━━┻━━━━━━┩\n";
    const char* table_ending =  "└──┴─────────────────┴───────┴──────┴──────┘\n";

    fprintf(stderr, "%s", table_heading);
    while (current_index < name_table->name_table_capacity)
    {
        current_name = name_table->name_array[current_index];
        PrintNameInfo(&current_name, current_index);
        current_index++;
    }
    fprintf(stderr, "%s", table_ending);
}

// ================================ ADDIND_NAME ===============================

static ssize_t 
FindElementInTable(unsigned int hash,
                   name_table_t name_table)
{
    ASSERT(name_table != NULL);

    ssize_t last_element = NO_LINK;

    for (ssize_t index = 0; index < (ssize_t) name_table->name_table_capacity; index++)
    {
        if (name_table->name_array[index].hash == hash)
        {
            last_element = index;
        }
    }

    return last_element;
}

static void
NewNameInit(string_s*    string,
            name_value_t value,
            name_table_t name_table)
{
    ASSERT(name_table);
    ASSERT(string);

    name_s new_name = {};

    new_name.name_string = *string;
    new_name.hash = MurmurHash2(string->string_source, 
                                 (unsigned int) string->string_size);
    new_name.prev_element = FindElementInTable(new_name.hash, name_table);
    ssize_t new_element_index = name_table->next_free;
    
    name_table->next_free = name_table->name_array[new_element_index].next_element;
    new_name.next_element = NO_LINK;
    new_name.value = value;

    if (new_name.prev_element != NO_LINK)
    {
        name_table->name_array[new_name.prev_element].
                                    next_element = new_element_index;
    }

    name_table->name_array[new_element_index] = new_name;
}

name_table_return_e 
AddNameInTable(string_s*    string,
               name_value_t value,    
               name_table_t name_table)
{
    ASSERT(string);

    name_table_return_e output = NAME_TABLE_RETURN_SUCCESS;

    if (name_table->name_count 
        == name_table->name_table_capacity)
    {
        if ((output = SetNameTableSize(name_table, 2 * name_table->name_count))
                != NAME_TABLE_RETURN_SUCCESS)
        {
            return output;
        }
    }

    NewNameInit(string, value, name_table);

    name_table->name_count++;

    return NAME_TABLE_RETURN_SUCCESS;   
}

name_table_return_e
DeleteElementInTable