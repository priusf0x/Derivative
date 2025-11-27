#include "latex_dump.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "Assert.h"
#include "derivative.h"
#include "tools.h"

static const char* latex_log_file_name = "logs/latex_dump.tex"; 

void static 
WriteConstInFile(derivative_t deritative,
                 size_t       current_node, 
                 FILE*        output_file);

void static 
WriteVarInFile(const derivative_t deritative,
               size_t             current_node,
               FILE*              output_file);

void static 
WriteOperationInFile(derivative_t deritative,
                     size_t       current_node,
                     FILE*        output_file);

static FILE*
GetLatexLogFile();

// ============================= LOGGING_IN_FILE ==============================

static void
WriteExpression(derivative_t deritative,
                ssize_t      current_node,
                FILE*        output_file);

void
LogDeritativeInLatex(derivative_t deritative,
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
    
    fprintf(output_file, "\\begin{equation}{\n ");
    WriteExpression(deritative, (ssize_t) current_node, output_file);
    fprintf(output_file, "\n} \\end{equation}\n");
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
WriteSubExpression(derivative_t deritative,
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

    const char* header_text = 
    "\\documentclass[a4paper,10pt]{article}\n"
    "\\usepackage[left=2cm,right=2cm,top=2cm,bottom=2cm]{geometry}\n"
    "\\usepackage[utf8]{inputenc}\n"
    "\\usepackage{mathtools}\n"
    "\\usepackage{amsmath,amsfonts,amssymb}\n"
    "\\usepackage{float}\n"
    "\\begin{document}{ \n \n";
    
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

    const char* end_text = "}\\end{document}\n";

    fprintf(output_file, "%s", end_text);

    fclose(output_file);

    SystemCall("pdflatex %s -f y 1>latex_output.log", latex_log_file_name);
    SystemCall("rm *.aux *.log >latex_output.log");
}

void static 
WriteConstInFile(derivative_t deritative,
                 size_t       current_node, 
                 FILE*        output_file)
{
    ASSERT(deritative  != NULL);
    ASSERT(output_file != NULL);
    
    double number = deritative->ariphmetic_tree->
                nodes_array[current_node].node_value.expression.constant;

    if(CheckIfInteger(number))
    {
        fprintf(output_file, "%ld", (long) number);
    }
    else
    {
        fprintf(output_file, "%f", number);
    }
}

void static 
WriteVarInFile(const derivative_t deritative,
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
WriteOperationInFile(derivative_t deritative,
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
            WriteExpression(deritative, node.left_index, output_file);
            fprintf(output_file, " + ");
            WriteExpression(deritative, node.right_index, output_file);
            break;

        case OPERATOR_MINUS:    
            WriteSubExpression(deritative, node.left_index, output_file);
            fprintf(output_file, " - ");
            WriteSubExpression(deritative, node.right_index, output_file);
            break;

        case OPERATOR_MUL:
            WriteSubExpression(deritative, node.left_index, output_file);
            fprintf(output_file, " \\times ");
            WriteSubExpression(deritative, node.right_index, output_file);
            break;

        case OPERATOR_DIV: 
            fprintf(output_file, "{");
            WriteExpression(deritative, node.left_index, output_file);
            fprintf(output_file, " \\over ");
            WriteExpression(deritative, node.right_index, output_file);
            fprintf(output_file, "}");
            break;

        case OPERATOR_SIN:
            fprintf(output_file, " \\sin{ ");
            WriteSubExpression(deritative, node.left_index, output_file);
            fprintf(output_file, "}");
            break;

        case OPERATOR_COS:
            fprintf(output_file, " \\cos{ ");
            WriteSubExpression(deritative, node.left_index, output_file);
            fprintf(output_file, "}");
            break;

        case OPERATOR_POWER:
            WriteSubExpression(deritative, node.left_index, output_file); 
            fprintf(output_file, "^{ ");
            WriteExpression(deritative, node.right_index, output_file);
            fprintf(output_file, "}");
            break;

        case OPERATOR_LN:
            fprintf(output_file, "\\ln(");
            WriteExpression(deritative, node.left_index, output_file);
            fprintf(output_file, ")");
            break;

        case OPERATOR_EXP:
            fprintf(output_file, "e^{ ");
            WriteExpression(deritative, node.left_index, output_file);
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
WriteExpression(derivative_t deritative,
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
WriteSubExpression(derivative_t deritative,
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

    WriteExpression(deritative, current_node, output_file);

    if (!is_single)
    {
        fprintf(output_file, ")}");
    }
}

