#include "latex_dump.h"

#include <stdlib.h>
#include <stdio.h>

#include "Assert.h"
#include "deritative.h"
#include "tools.h"

static const char* latex_log_file_name = "logs/latex_dump.tex"; 

void static 
WriteConstInFile(deritative_t deritative,
                 size_t       current_node, 
                 FILE*        output_file);

void static 
WriteVarInFile(const deritative_t deritative,
               size_t             current_node,
               FILE*              output_file);

void static 
WriteOperationInFile(deritative_t deritative,
                     size_t       current_node,
                     FILE*        output_file);

static FILE*
GetLatexLogFile();

// ============================= LOGGING_IN_FILE ==============================

static void
WriteExpression(deritative_t deritative,
                ssize_t      current_node,
                FILE*        output_file);

void
WriteInLatex(deritative_t deritative,
             size_t       current_node,
             FILE*        output_file)
{
    ASSERT(deritative != NULL);
    
    if (output_file == NULL)
    {
        output_file = GetLatexLogFile();
        if (output_file == NULL)
        {   
            return;
        }
    }

    node_s node = deritative->ariphmetic_tree->nodes_array[current_node];
    
    if ((current_node == 0) 
        && (node.left_index != NO_LINK))
    {
        current_node = (size_t) node.left_index;
    }

    WriteExpression(deritative, (ssize_t) current_node, output_file);

}

// =========================== FILE_USAGE =====================================

static FILE* 
GetLatexLogFile()
{
    static FILE* maxim_gorohov_file = fopen(latex_log_file_name, "w+");
    return maxim_gorohov_file;
}

void 
SetLogFileName(const char* new_name)
{
    ASSERT(new_name != NULL);

    latex_log_file_name = new_name;
}


// ============================ DUMP_HELPERS ==================================

void static  
WriteSubExpression(deritative_t deritative,
                   ssize_t      current_node,
                   FILE*        output_file);

void 
StartLatexDocument(FILE* output_file)
{
    if (output_file == NULL)
    {
        output_file = GetLatexLogFile();
        if (output_file == NULL)
        {   
            return;
        }
    }

    const char* header_text = R"(
    \documentclass[a4paper,10pt]{article}
    \usepackage[left=2cm,right=2cm,top=2cm,bottom=2cm]{geometry}
    \usepackage[utf8]{inputenc}
    \usepackage[T2A]{fontenc}
    \usepackage[russian]{babel}
    \usepackage{amsmath,amsfonts,amssymb}
    \usepackage{float}
    \begin{document})";
    
    fprintf(output_file, "%s", header_text);
}

void 
EndLatexDocument(FILE* output_file)
{
    if (output_file == NULL)
    {
        output_file = GetLatexLogFile();
        if (output_file == NULL)
        {   
            return;
        }
    }

    const char* end_text = "\\end{document}\n";

    fprintf(output_file, "%s", end_text);

    SystemCall("latexmk %s -bibtex -f -c -pdf ", latex_log_file_name);
}

void static 
WriteConstInFile(deritative_t deritative,
                 size_t       current_node, 
                 FILE*        output_file)
{
    ASSERT(deritative  != NULL);
    ASSERT(output_file != NULL);
    
    fprintf(output_file, "%f", 
            deritative->ariphmetic_tree->
                nodes_array[current_node].node_value.expression.constant);
}

void static 
WriteVarInFile(const deritative_t deritative,
               size_t             current_node,
               FILE*              output_file)
{
    ASSERT(deritative != NULL);
    ASSERT(output_file != NULL);

    fprintf(output_file, "%c",
            deritative->ariphmetic_tree->
                nodes_array[current_node].node_value.expression.variable);
}

void static 
WriteOperationInFile(deritative_t deritative,
                     size_t       current_node,
                     FILE*        output_file)
{
    ASSERT(deritative != NULL);
    ASSERT(output_file != NULL);

    node_s node = deritative->ariphmetic_tree->nodes_array[current_node];
    expression_s expr = node.node_value;

    const char* undefined_message = "undefined blyat";

    switch(expr.expression.operation)
    {
        case OPERATOR_PLUS:
            WriteSubExpression(deritative, node.left_index, output_file);
            fprintf(output_file, "+");
            WriteSubExpression(deritative, node.right_index, output_file);
            break;

        case OPERATOR_MINUS:    
            WriteSubExpression(deritative, node.left_index, output_file);
            fprintf(output_file, "-");
            WriteSubExpression(deritative, node.right_index, output_file);
            break;

        case OPERATOR_MUL:
            WriteSubExpression(deritative, node.left_index, output_file);
            fprintf(output_file, "\\times");
            WriteSubExpression(deritative, node.right_index, output_file);
            break;

        case OPERATOR_DIV: 
            fprintf(output_file, "{");
            WriteExpression(deritative, node.left_index, output_file);
            fprintf(output_file, "\\over");
            WriteExpression(deritative, node.right_index, output_file);
            fprintf(output_file, "}");
            break;

        case OPERATOR_SIN:
            fprintf(output_file, "\\sin{");
            WriteSubExpression(deritative, node.left_index, output_file);
            fprintf(output_file, "}");
            break;

        case OPERATOR_COS:
            fprintf(output_file, "\\cos{");
            WriteSubExpression(deritative, node.left_index, output_file);
            fprintf(output_file, "}");
            break;
            
        case OPERATOR_UNDEFINED:
            fprintf(output_file, "%s", undefined_message);
            break;

        default: break;
    }
}

// ============================ WRITE_FUNCTIONS ===============================

static void
WriteExpression(deritative_t deritative,
                ssize_t      current_node,
                FILE*        output_file) 
{
    ASSERT(deritative != NULL);
    ASSERT(output_file != NULL);

    if (current_node == NO_LINK)
    {
        return;
    }

    node_s node = deritative->ariphmetic_tree->nodes_array[current_node];

    switch(node.node_value.expression_type)
    {
        case EXPRESSION_TYPE_CONST:
            WriteConstInFile(deritative, (size_t) current_node, output_file);
            break;
        
        case EXPRESSION_TYPE_VAR:
            WriteVarInFile(deritative, (size_t) current_node, output_file);            
            break;

        case EXPRESSION_TYPE_OPERATOR:
            WriteOperationInFile(deritative, (size_t) current_node, output_file);
            break;

        case EXPRESSION_TYPE_UNDEFINED:
            break;

        default: break;
    }

}

static void
WriteSubExpression(deritative_t deritative,
                   ssize_t      current_node,
                   FILE*        output_file)
{
    ASSERT(deritative != NULL);
    ASSERT(output_file != NULL);

    node_s node = deritative->ariphmetic_tree->nodes_array[current_node];
    
    if (node.parent_connection == EDGE_DIR_NO_DIRECTION)
    {
        return;
    }

    bool is_single = false;
    
    if ((node.right_index == NO_LINK) 
        || (node.left_index == NO_LINK))
    {
        is_single = true;
    }

    if (!is_single)
    {
        fprintf(output_file, "{(");
    }

    WriteInLatex(deritative, (size_t) current_node, output_file);

    if (!is_single)
    {
        fprintf(output_file, ")}");
    }
}

