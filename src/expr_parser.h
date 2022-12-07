/**
 * Project: Implementace překladače imperativního jazyka IFJ22
 * 
 * @file expr_parser.h
 * @brief Header file for expression parser
 * 
 * @author Nikolas Nosál <xnosal01>
 * @author Adam Mrkva <xmrkva04>
 * @author Rostislav Navrátil <xnavra72>
 * @author David Nevrlka <xnevrl00>
 */

#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "error_handler.h"
#include "expr_tree.h"

expr_node_t* expr_parse(tok_arr_t* tok_arr, token_type expr_end_type);