#include <stdlib.h>

#include "error_handler.h"
#include "lex.h"
#include "parser.h"


int main(){

	//Init
	error_handler_t error_handler;
	error_handler_ctor(&error_handler);
	token_array_t token_array;
	token_array_ctor(&token_array);
	
	
	lex_tokenize_file(&error_handler, &token_array,"./example2.php");
	handle_lex_error(error_handler);
	
	
	token_array_debug_print(token_array);
	

	parse_token_array(&error_handler,token_array);
	handle_syntax_error(error_handler);
	
	
	
	token_array_dtor(&token_array);
	
	
	
	return 0;
}