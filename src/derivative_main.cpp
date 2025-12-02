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

// WISH_LIST
// 1 priority Strings_var, name_table, ask user variable
// 2 priotity clean_main, make functions 
// 3 priority Fill function, calculation with vars
// 4 GRRRRRROROOOOOOOQQQQQ 
// 4 


int
main()
{
    const size_t start_tree_size = 1; 
    derivative_t derivative = NULL;

    StartLatexDocument(NULL);

    int error_number = 0;

    if ((error_number = DerivativeInit(&derivative, start_tree_size, 
                                        formula_file_name)) != 0)
    {   
        printf("huyna %d\n", error_number);

        return error_number;
    }

    ConvertToGraph(derivative);    

    LogDeritativeInLatex(derivative, 0, NULL);
 
    ssize_t output = TakeExpressionDerivative(derivative, 0);   
    derivative->ariphmetic_tree->nodes_array[output].parent_index = 0;
    derivative->ariphmetic_tree->nodes_array[0].left_index = output;

    SimplifyNeutralMultipliers(derivative, 0);
  
    SimplifyConst(derivative, 0);

    LogDeritativeInLatex(derivative, 0, NULL);

    DerivativeDestroy(&derivative);

    EndLatexDocument(NULL);

    return 0;
} 


