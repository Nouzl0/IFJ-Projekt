#pragma once
#include <stdlib.h>
#include <stdio.h>

#define LEX_ERROR 1

#define SYNTAX_ERROR 2

#define UNDEFINED_FUNCTION_ERROR 3

#define FUNCTION_CALL_ERROR 4

#define UNDEFINED_VARIABLE_ERROR 5

#define FUNCTION_RETURN_ERROR 6

#define VARIABLE_TYPE_ERROR 7

#define SEMANTIC_ERROR 8

#define PROGRAM_ERROR 99


//Token
typedef struct {
	int lex;
	int syntax;
    int semantic;
} error_handler_t;

void error_handler_ctor(error_handler_t* eh_ptr);

void register_lex_error(error_handler_t* eh_ptr, int line, char* buffer);

void handle_lex_error(error_handler_t eh);

void handle_program_error();