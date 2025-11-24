// #include "simplify.h"

// #include "derivative.h"
// #include "expression.h"
// #include "Assert.h"

// derivative_return_e 
// SimplifyNeutralMultipliers(derivative_t derivative,
//                            size_t       current_node)
// {
//     ASSERT(derivative);

//     node_s* node = &(derivative->ariphmetic_tree->nodes_array[current_node]);

//     if (node->right_index)
//     {
//         SimplifyNeutralMultipliers(derivative, node->right_index);
//     }
//     if (node->left_index)
//     {
//         SimplifyNeutralMultipliers(derivative, node->left_index);
//     }

//     // if (node->node_value)
//     return DERITATIVE_RETURN_SUCCESS;
// }