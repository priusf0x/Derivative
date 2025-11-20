#include "tree.h"

#include <cstddef>
#include <stdio.h>

#include "Assert.h"
#include "tools.h"
#include "stack.h"
#include "my_string.h"

static tree_return_e TreeDot(const tree_t tree, const char* current_time);
static void DrawNode(const node_s* node, FILE* dot_file);

// ================================ SINGLE_TONE ===============================

const char* LOG_FILE_NAME = "logs/tree_log_file.htm";

FILE*
GetLogFile()
{
    static FILE* log_file = fopen(LOG_FILE_NAME, "w+");
    return log_file;
}

// ============================= BASE_DUMP_FUNCTION ===========================


tree_return_e
TreeBaseDump(const tree_t tree, 
             FILE*        file_output)
{
    ASSERT(tree != NULL);
    ASSERT(file_output != NULL);

    swag_t bypass_stack = NULL;

    const size_t stack_start_size = 10;

    if (StackInit(&bypass_stack, stack_start_size, "InDepthBypass") != 0)
    {
        return TREE_RETURN_STACK_ERROR;
    }

    if (tree->nodes_count == 0)
    {
        StackDestroy(bypass_stack);
        return TREE_RETURN_EMPTY_TREE;
    }

    ssize_t current_element = tree->nodes_array[0].left_index;
    size_t last_direction = (size_t) EDGE_DIR_NO_DIRECTION;

    fprintf(file_output, "( ");

    do
    {
        PrintString(&(tree->nodes_array[current_element].node_value), file_output);

        fprintf(file_output, "( ");

        if (tree->nodes_array[current_element].left_index != NO_LINK)
        {
            if (StackPush(bypass_stack, (size_t) EDGE_DIR_LEFT) != 0)
            {
                return TREE_RETURN_STACK_ERROR;
            }
            
            current_element = tree->nodes_array[current_element].left_index;
        }
        else if (tree->nodes_array[current_element].right_index != NO_LINK)
        {
            fprintf(file_output, "nil ");

            if (StackPush(bypass_stack, (size_t) EDGE_DIR_RIGHT) != 0)
            {
                return TREE_RETURN_STACK_ERROR;
            }
        
            current_element = tree->nodes_array[current_element].right_index;
        }
        else
        {
            fprintf(file_output, "nil nil " );

            do
            {
                if (GetStackSize(bypass_stack) == 0)
                {
                    break; 
                }

                if (StackPop(bypass_stack, &last_direction) != 0)
                {
                    return TREE_RETURN_STACK_ERROR;
                }
                current_element = tree->nodes_array[current_element].parent_index;
                
                fprintf(file_output, ") ");

            } while ((last_direction != EDGE_DIR_LEFT)
                     || (tree->nodes_array[current_element].right_index == NO_LINK));

            if (last_direction == EDGE_DIR_LEFT)
            {
                if (StackPush(bypass_stack, (size_t) EDGE_DIR_RIGHT) != 0)
                {
                    return TREE_RETURN_STACK_ERROR;
                }

                current_element = tree->nodes_array[current_element].right_index;
            }
        }
    } while (GetStackSize(bypass_stack) != 0);

    fprintf(file_output, ") nil )");

    StackDestroy(bypass_stack);

    return TREE_RETURN_SUCCESS;
}

// ============================== DUMP_FUNCTIONS ==============================

static void PrintTreeInfo(const tree_t tree, const char* current_time, FILE* file_output);
static void PrintHTMLHeader(FILE* log_file, const char* current_time);
static void PrintElementsInfo(const tree_t tree, FILE* file_output);

void
TreeDump(const tree_t tree)
{
    ASSERT(tree != NULL);

    FILE* log_file = GetLogFile();

    if (log_file == NULL)
    {
        return;
    }

    const size_t str_time_size = 100;
    char current_time[str_time_size] = {};
    GetTime(current_time, str_time_size);

    PrintHTMLHeader(log_file, current_time);
    PrintTreeInfo(tree, current_time, log_file);
    PrintElementsInfo(tree, log_file);

    TreeDot(tree, current_time);
}

// ============================== DUMP_HELPERS ================================
static void
PrintHTMLHeader(FILE*       log_file,
                const char* current_time)
{
    ASSERT(log_file != NULL);
    ASSERT(current_time != NULL);

    fprintf(log_file, "<html>\n"
                        "<style>"
                        "body{background-color: rgb(48, 48, 48);}"
                        "h1{color: rgb(212, 58, 56);}"
                        "h2{color: rgba(153, 26, 24, 1);}"
                        "h4{color: rgb(182, 182, 182);}"
                        "</style>"
                        "<h1> TREE_DUMP %s</h1>\n",  current_time);
}

static void
PrintTreeInfo(const tree_t tree,
              const char*  current_time,
              FILE*        file_output)
{
    const ssize_t max_string_size = 50;
    char img_template[max_string_size] = {};
    snprintf(img_template, max_string_size - 1, "<img src=\"%s.png\","
                                                "height = \"20%%\">",
                                                current_time);
    fprintf(file_output, "%s", img_template);

    fprintf(file_output, "<p><h4>Count: %zu.", tree->nodes_count);
    fprintf(file_output, "Capacity: %zu.</h4>", tree->nodes_capacity);
}

static void
PrintElementsInfo(const tree_t tree,
                  FILE*        file_output)
{
    for(size_t index = 0; index < tree->nodes_capacity; index++)
    {
        fprintf(file_output, "<p> <h4> <li>index in table: %zu\n <br/>", tree->nodes_array[index].index_in_tree);
        fprintf(file_output, "left index: %ld \n <br/>", tree->nodes_array[index].left_index);
        fprintf(file_output, "right index: %ld\n <br/>", tree->nodes_array[index].right_index);
        fprintf(file_output, "value:");
        if (tree->nodes_array[index].parent_connection != EDGE_DIR_NO_DIRECTION)
        {
            PrintString(&(tree->nodes_array[index].node_value), file_output);
        }
        fprintf(file_output, "<br/>");
        fprintf(file_output, "parent_index: %ld\n\n </li></p></h4>", tree->nodes_array[index].parent_index);
    }
}

static tree_return_e
TreeDot(const tree_t tree,
        const char*  current_time)
{
    ASSERT(tree != NULL);
    ASSERT(current_time != NULL);

    const size_t max_string_size = 40;

    char name_template[max_string_size] = {};

    snprintf(name_template, max_string_size - 1, "logs/%s.gv", current_time);
    FILE* dot_file = fopen(name_template, "w+");

    if (dot_file == NULL)
    {
        return TREE_RETURN_OPEN_FILE_ERROR;
    }

    static const char* GRAPH_VIZ_OPTIONS =R"(graph  G{
                                            bgcolor = "#303030";
                                            splines = polyline;
                                            node [shape = record, style = filled,
                                            color = "#b16261", fillcolor = "#949494"];
                                            edge [width = 2, color = "#949494"])";

    fprintf(dot_file, "%s", GRAPH_VIZ_OPTIONS);

    for (size_t index = 1; index < tree->nodes_capacity; index++)
    {
        if (tree->nodes_array[index].parent_connection != EDGE_DIR_NO_DIRECTION)
        {
            DrawNode(tree->nodes_array + index, dot_file);
        }
    }

    fprintf(dot_file, "}");

    if (fclose(dot_file) != 0)
    {
        return TREE_RETURN_CLOSE_FILE_ERROR;
    }

    const ssize_t max_command_size = 200;
    char command[max_command_size] = {};

    snprintf(command, max_command_size - 1, "dot -Tpng logs/%s.gv -o"
             "logs/%s.png", current_time,
             current_time);

    system(command);

    return TREE_RETURN_SUCCESS;
}

static void
DrawNode(const node_s* node,
         FILE*         dot_file)
{
    ASSERT(node != NULL);
    ASSERT(dot_file != NULL);

    static const char* NODE_TEMPLATE = "%ld[label = \"{parent index = %ld| phys index = %ld"
                                       "| value = %.*s|{left index = %ld | right index = %ld}}\"];\n";
    const size_t string_size = 250;
    char graphviz_node[string_size] = {};

    snprintf(graphviz_node, string_size, NODE_TEMPLATE, node->index_in_tree,
             node->parent_index, node->index_in_tree, node->node_value.string_size,
             node->node_value.string_source ,node->left_index, node->right_index);

    fprintf(dot_file, "%s\n", graphviz_node);

    if (node->left_index != -1)
    {
        fprintf(dot_file, "%zu -- %ld;\n", node->index_in_tree, node->left_index);
    }

    if (node->right_index != -1)
    {
        fprintf(dot_file, "%zu -- %ld;\n", node->index_in_tree, node->right_index);
    }
}

