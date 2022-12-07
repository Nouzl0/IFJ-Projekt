/**
 * Project: Implementace překladače imperativního jazyka IFJ22
 * 
 * @file lex.h
 * @brief Header file for lexical analyzer
 * 
 * @author Nikolas Nosál <xnosal01>
 * @author Adam Mrkva <xmrkva04>
 * @author Rostislav Navrátil <xnavra72>
 * @author David Nevrlka <xnevrl00>
 */

#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "error_handler.h"
#include "strings_lib.h"
#include "tokens_lib.h"

void lex_tokenize(tok_arr_t* ta_ptr, FILE* source);

void lex_tokenize_file(tok_arr_t* ta_ptr, char* file_name);
