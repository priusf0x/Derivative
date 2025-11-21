#include "tree.h"
#include "simple_parser.h"
#include "deritative.h"
#include "my_string.h"
#include "tools.h"

static const char* formula_file_name = "pletnev.zov";

int
main()
{
    const size_t start_tree_size = 1; 
    deritative_t deritative = NULL;

    int error_number = 0;

    if ((error_number = DeritativeInit(&deritative, start_tree_size, 
                                        formula_file_name)) != 0)
    {   
        printf("huyna %d\n", error_number);

        return error_number;
    }

    TreeDump(deritative->ariphmetic_tree);

    DeritativeDestroy(&deritative);

    return 0;
} 



    // if ((error_number = TreeInit(&expression_tree,
    //                              start_tree_size)) != 0)
    // {
    //    return error_number;
    // }

    // node_s node_1 = {.parent_index = 0, .parent_connection = EDGE_DIR_LEFT,
    //                  .right_index  = NO_LINK, .left_index = NO_LINK,
    //                  .node_value = {.expression = {.constant = 10}, .expression_type = EXPRESSION_TYPE_CONST}};

    // TreeAddNode(expression_tree, &node_1);
        

    // node_s node_2 = {.parent_index = 1, .parent_connection = EDGE_DIR_RIGHT,
    //                  .right_index  = NO_LINK, .left_index = NO_LINK,
    //                  .node_value = {.expression = {.operation = OPERATOR_MINUS}, .expression_type = EXPRESSION_TYPE_OPERATOR}};

    // TreeAddNode(expression_tree, &node_2);

    // node_s node_3 = {.parent_index = 2, .parent_connection = EDGE_DIR_RIGHT,
    //                  .right_index  = NO_LINK, .left_index = NO_LINK,
    //                  .node_value = {.expression = {.variable = 'h'}, .expression_type = EXPRESSION_TYPE_VAR}};
