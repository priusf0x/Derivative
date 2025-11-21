#include "deritative.h"

#include "tree.h"
#include "simple_parser.h"
#include "my_string.h"
#include "tools.h"
#include "latex_dump.h"


static const char* formula_file_name = "pletnev.zov";

int
main()
{
    const size_t start_tree_size = 1; 
    deritative_t deritative = NULL;

    int error_number = 0;

    StartLatexDocument(NULL);

    if ((error_number = DeritativeInit(&deritative, start_tree_size, 
                                        formula_file_name)) != 0)
    {   
        printf("huyna %d\n", error_number);

        return error_number;
    }

    TreeDump(deritative->ariphmetic_tree);
    
    WriteInLatex(deritative, 0, NULL);

    DeritativeDestroy(&deritative);

    EndLatexDocument(NULL);

    return 0;
} 


