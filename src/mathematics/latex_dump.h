#ifndef LATEX_DUMP_H
#define LATEX_DUMP_H

#include <stdlib.h>
#include "deritative.h"

// ========================= FILE_LATEX_FEATURES ==============================

void SetLogFileName(const char* new_name);
void WriteInLatex(deritative_t deritative, size_t current_node, FILE* output_file);
void StartLatexDocument(FILE* output_file);
void EndLatexDocument(FILE* output_file);

// ============================================================================

#endif // LATEX_DUMP_H