#include "error_handler.h"


void error_handler_ctor(error_handler_t* eh_ptr){
	eh_ptr->lex = 0;
	eh_ptr->syntax = 0;
	eh_ptr->semantic = 0;
}


void register_lex_error(error_handler_t* eh_ptr, int line, char* buffer){
	eh_ptr->lex++;
fprintf(stderr, "Invalid token on line: %d in: %s\n", line, buffer);
}

void handle_lex_error(error_handler_t eh){
	if (eh.lex){
		exit(LEX_ERROR);
	}
}

void handle_program_error(){
	fprintf(stderr, "Unrecognized internal program error");
	exit(PROGRAM_ERROR);
}