#ifndef LATEX_DUMP_H
#define LATEX_DUMP_H

#include <stdlib.h>
#include "deritative.h"

void SetLogFileName(const char* new_name);
void WriteInLatex(deritative_t deritative, size_t current_node, FILE* output_file);

#endif // LATEX_DUMP_H