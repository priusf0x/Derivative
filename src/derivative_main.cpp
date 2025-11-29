#include "derivative.h"

#include "tree.h"
#include "simple_parser.h"
#include "my_string.h"
#include "tools.h"
#include "latex_dump.h"
#include "take_derivative.h"
#include "simplify.h"
#include "recursive_decent.h"

static const char* formula_file_name = "pletnev.zov";

int
main()
{
    const size_t start_tree_size = 1; 
    derivative_t derivative = NULL;

    int error_number = 0;

    if ((error_number = DerivativeInit(&derivative, start_tree_size, 
                                        formula_file_name)) != 0)
    {   
        printf("huyna %d\n", error_number);

        return error_number;
    }

    ConvertToGraph(derivative);

    DerivativeDestroy(&derivative);

    return 0;
} 


