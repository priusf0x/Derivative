#include "derivative.h"

#include "tree.h"
#include "simple_parser.h"
#include "my_string.h"
#include "tools.h"
#include "latex_dump.h"
#include "take_derivative.h"

static const char* formula_file_name = "pletnev.zov";

int
main()
{
    const size_t start_tree_size = 1; 
    derivative_t deritative = NULL;

    int error_number = 0;

    StartLatexDocument(NULL);

    if ((error_number = DerivativeInit(&deritative, start_tree_size, 
                                        formula_file_name)) != 0)
    {   
        printf("huyna %d\n", error_number);

        return error_number;
    }

    LogDeritativeInLatex(deritative, 0, NULL);

    
    TakeDerivative(deritative, deritative->ariphmetic_tree->nodes_array[0].left_index);

    LogDeritativeInLatex(deritative, 0, NULL);

    TreeDump(deritative->ariphmetic_tree);

    DerivativeDestroy(&deritative);

    EndLatexDocument(NULL);

    return 0;
} 


