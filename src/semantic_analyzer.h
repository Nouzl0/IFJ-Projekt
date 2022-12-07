/**
 * Project: Implementace překladače imperativního jazyka IFJ22
 * 
 * @file semantic_analyzer.h
 * @brief Header file for semantic analyzer
 * 
 * @author Nikolas Nosál <xnosal01>
 * @author Adam Mrkva <xmrkva04>
 * @author Rostislav Navrátil <xnavra72>
 * @author David Nevrlka <xnevrl00>
 */

#include <stdlib.h>

#include "error_handler.h"
#include "syntax_tree.h"
#include "symtable.h"

void analyze_ast(stx_node_t* ast_root);