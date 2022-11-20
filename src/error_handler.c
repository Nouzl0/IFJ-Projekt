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


void register_semantic_error(error_handler_t* eh_ptr, int error_code, int line, int column){
	eh_ptr->error = true;
	eh_ptr->semantic_code = error_code;
	if(!eh_ptr->quiet_errors){
		fprintf(stderr, "Unexpected token on line: %d column: %d\n", line, column);
	}
}
	
void handle_semantic_error(error_handler_t eh){
	if (eh.error){
		exit(eh.semantic_code);
	}
}




//NEW

void errors_t_ctor(errors_t* err_ptr){
	err_ptr->quiet_errors = false;
	err_ptr->error = 0;
}

void alloc_error(errors_t* err_ptr){
	err_ptr->error = 99;
}

void lex_error(errors_t* err_ptr, int line, char* buffer){
	err_ptr->error = 1;
	if(!err_ptr->quiet_errors){
		fprintf(stderr, "Invalid character on line: %d in: %s\n", line, buffer);
	}
}