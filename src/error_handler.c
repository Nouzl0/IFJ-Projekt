#include "error_handler.h"

void handle_program_error(){
	fprintf(stderr, "Unrecognized internal program error, try to compile again!");
	exit(PROGRAM_ERROR);
}


void error_handler_ctor(error_handler_t* eh_ptr){
	eh_ptr->quiet_errors = false;
	eh_ptr->error = false;
	eh_ptr->semantic_code = 0;
}


void register_lex_error(error_handler_t* eh_ptr, int line, char* buffer){
	eh_ptr->error = true;
	if(!eh_ptr->quiet_errors){
		fprintf(stderr, "Invalid character on line: %d in: %s\n", line, buffer);
	}
}


void handle_lex_error(error_handler_t eh){
	if (eh.error){
		exit(LEX_ERROR);
	}
}


void register_syntax_error(error_handler_t* eh_ptr, int line, int column){
	eh_ptr->error = true;
	if(!eh_ptr->quiet_errors){
		fprintf(stderr, "Unexpected token on line: %d column: %d\n", line, column);
	}
}
	
void handle_syntax_error(error_handler_t eh){
	if (eh.error){
		exit(SYNTAX_ERROR);
	}
}


//NEW

void errors_ctor(errors_t* err_ptr){
	err_ptr->quiet_errors = false;
	err_ptr->error = 0;
}

void alloc_error(){
	global_err_ptr->error = PROGRAM_ERROR;
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
		fprintf(stderr, "Syntax error on line: %d, column: %d, reason: %s", token.line, token.column, info);
	}
}

void semantic_error(int error_code, token_t token, char* info){
	global_err_ptr->error = error_code;
	if(!global_err_ptr->quiet_errors){
		fprintf(stderr, "Semantic error on line: %d, column: %d, reason: %s", token.line, token.column, info);
	}
}