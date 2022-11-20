#include <stdlib.h>

#include "error_handler.h"
errors_t global_err;
errors_t* global_err_ptr = &global_err;

#include "lex.h"
#include "parser.h"
#include "semantic_analyzer.h"


int main(){
	
	errors_t_ctor(global_err_ptr);

	//Init
	error_handler_t error_handler;
	error_handler_ctor(&error_handler);
	token_array_t token_array;
	token_array_ctor(&token_array);
	
	
	lex_tokenize_file(&token_array,"./example2.php");
	
	if(global_err_ptr->error){
		token_array_dtor(&token_array);
		return global_err_ptr->error;
	}
	
	
	
	token_array_debug_pretty_print(token_array);
	

	stree_item_t* ast = parse_token_array(&error_handler,token_array);
	handle_syntax_error(error_handler);
	
	stree_json_debug_print(ast);
	
	
	
	//analyze_ast(ast);
	
	
	
	stree_dtor(&ast);
	token_array_dtor(&token_array);
	
	
	
	return 0;
}