/**
 * Project: Implementace překladače imperativního jazyka IFJ22
 * 
 * @file parser.h
 * @brief Header file for syntax analyzer
 * 
 * @author Nikolas Nosál <xnosal01>
 * @author Adam Mrkva <xmrkva04>
 * @author Rostislav Navrátil <xnavra72>
 * @author David Nevrlka <xnevrl00>
 */

#pragma once

#include <stdlib.h>

#include "error_handler.h"
#include "tokens_lib.h"
#include "expr_parser.h"
#include "syntax_tree.h"

stx_node_t* parser_build_all(tok_arr_t* ta_ptr);