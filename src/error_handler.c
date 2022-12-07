/**
 * Project: Implementace překladače imperativního jazyka IFJ22
 * 
 * @file error_handler.c
 * @brief Error handling
 * 
 * @author Nikolas Nosál <xnosal01>
 * @author Adam Mrkva <xmrkva04>
 * @author Rostislav Navrátil <xnavra72>
 * @author David Nevrlka <xnevrl00>
 */

#include "error_handler.h"

void errors_ctor(errors_t* err_ptr){
	err_ptr->quiet_errors = false;
	err_ptr->error = 0;
}

void alloc_error(){
	global_err_ptr->error = PROGRAM_ERROR;
	if(!global_err_ptr->quiet_errors){
		fprintf(stderr, "Memory allocation error");
	}
}

void lex_error(int line, char* buffer){
	global_err_ptr->error = LEX_ERROR;
	if(!global_err_ptr->quiet_errors){
		fprintf(stderr, "Invalid character on line: %d in: %s\n", line, buffer);
	}
}

void syntax_error(token_t token, char* info){
	global_err_ptr->error = SYNTAX_ERROR;
	if(!global_err_ptr->quiet_errors){
		fprintf(stderr, "Syntax error on line: %d, column: %d, reason: %s\n", token.line, token.column, info);
	}
}

void semantic_error(int error_code, token_t token, char* info){
	global_err_ptr->error = error_code;
	if(!global_err_ptr->quiet_errors){
		fprintf(stderr, "Semantic error on line: %d, column: %d, reason: %s\n", token.line, token.column, info);
	}
}