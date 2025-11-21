#ifndef TOOLS_H
#define TOOLS_H

#include <stdlib.h>
#include <stdio.h>

void* recalloc(void*  pointer, size_t current_size, size_t new_size);
void  GetTime(char*  current_time, const size_t string_size);

#define MEOW fprintf(stderr, "meow in %d", __LINE__);

#endif // TOOLS_H
