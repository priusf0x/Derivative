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
        printf("R->L %d\n", error_number);

        return error_number;
    }

    ConvertToGraph(derivative);    

    LogDeritativeInLatex(derivative, 0, NULL);
 
    TreeDump(derivative->ariphmetic_tree);

    name_table_t name_table = NULL;

    InitNameTable(&name_table, 3);

    string_s new_name_1 = {"Hello", 5};
    string_s new_name_2 = {"Bye", 3};
    string_s new_name_3 = {"Hello", 5};
    string_s new_name_4 = {"Hello", 5};
    string_s new_name_5 = {"Bye", 3};
    
    AddNameInTable(&new_name_1, 3, name_table);
    AddNameInTable(&new_name_2, 3, name_table);
    AddNameInTable(&new_name_3, 3, name_table);
    AddNameInTable(&new_name_4, 3, name_table);
    AddNameInTable(&new_name_5, 3, name_table);

    NameTableDump(name_table);

    DestroyNameTable(&name_table);

    // ssize_t output = TakeExpressionDerivative(derivative, 0);   
    // derivative->ariphmetic_tree->nodes_array[output].parent_index = 0;
    // derivative->ariphmetic_tree->nodes_array[0].left_index = output;

    // SimplifyNeutralMultipliers(derivative, 0);
  
    // SimplifyConst(derivative, 0);

    // LogDeritativeInLatex(derivative, 0, NULL);

    DerivativeDestroy(&derivative);

    EndLatexDocument(NULL);

    return 0;
} 


