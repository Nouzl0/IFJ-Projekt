/**
 * Project: Implementace překladače imperativního jazyka IFJ22
 * 
 * @file error_handler.h
 * @brief Header file for error handling
 * 
 * @author Nikolas Nosál <xnosal01>
 * @author Adam Mrkva <xmrkva04>
 * @author Rostislav Navrátil <xnavra72>
 * @author David Nevrlka <xnevrl00>
 */

#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "tokens_lib.h"

#define LEX_ERROR 1

#define SYNTAX_ERROR 2

#define UNDEFINED_FUNCTION_ERROR 3

#define FUNCTION_CALL_ERROR 4

#define UNDEFINED_VARIABLE_ERROR 5

#define FUNCTION_RETURN_ERROR 6

#define VARIABLE_TYPE_ERROR 7

#define SEMANTIC_ERROR 8

#define PROGRAM_ERROR 99

typedef struct {
    bool quiet_errors;
	int error;
	int semantic_code;
} errors_t;

extern errors_t* global_err_ptr;

void errors_ctor();

void alloc_error();

void lex_error(int line, char* buffer);

void syntax_error(token_t token, char* info);

void semantic_error(int error_code, token_t token, char* info);