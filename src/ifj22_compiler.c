#include <stdlib.h>

#include "error_handler.h"
errors_t global_err;
errors_t* global_err_ptr = &global_err;

#include "lex.h"
#include "parser.h"
#include "semantic_analyzer.h"
/*
TODO:
	vytvorit soubor rozdeleni.txt a pouzit
	odradkovani LF ne CR LF

*/

int main(){
	errors_ctor(global_err_ptr);

	tok_arr_t token_array;
	tok_arr_ctor(&token_array);
		
	lex_tokenize_file(&token_array,"./example2.php");
	
	if(global_err_ptr->error){
		tok_arr_dtor(&token_array);
		return global_err_ptr->error;
	}
	/*
	ptree_item_t* tree = expr_parse(&token_array, SEMICOLON);
	
	ptree_debug_to_json(tree);
	*/


	//replace in nearest future
	error_handler_t error_handler;
	error_handler_ctor(&error_handler);
	
	
	tok_arr_debug_print(token_array);
	

	stree_item_t* ast = parse_token_array(&error_handler,token_array);
	handle_syntax_error(error_handler);
	
	stree_json_debug_print(ast);
	
	
	
	analyze_ast(ast);
	
	
	
	stree_dtor(&ast);
	tok_arr_dtor(&token_array);
	
	
	
	return 0;
}