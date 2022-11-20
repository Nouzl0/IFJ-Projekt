#include "error_handler.h"

void handle_program_error(){
	fprintf(stderr, "Unrecognized internal program error, try to compile again!");
	exit(PROGRAM_ERROR);
}


void error_handler_ctor(error_handler_t* eh_ptr){
	eh_ptr->quiet_errors = 0;
	eh_ptr->lex = 0;
	eh_ptr->syntax = 0;
	eh_ptr->semantic = 0;
}


void register_lex_error(error_handler_t* eh_ptr, int line, char* buffer){
	eh_ptr->lex++;
	if(!eh_ptr->quiet_errors){
		fprintf(stderr, "Invalid character on line: %d in: %s\n", line, buffer);
	}
}


void handle_lex_error(error_handler_t eh){
	if (eh.lex){
		exit(LEX_ERROR);
	}
}


void register_syntax_error(error_handler_t* eh_ptr, int line, int column){
	eh_ptr->syntax++;
	if(!eh_ptr->quiet_errors){
		fprintf(stderr, "Unexpected token on line: %d column: %d\n", line, column);
	}
}
	
void handle_syntax_error(error_handler_t eh){
	if (eh.syntax){
		exit(SYNTAX_ERROR);
	}
}
