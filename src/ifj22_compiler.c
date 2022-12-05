#include <stdlib.h>

#include "error_handler.h"
errors_t global_err;
errors_t* global_err_ptr = &global_err;

#include "lex.h"
#include "parser.h"
#include "semantic_analyzer.h"
#include "code_generation.h"

/*
TODO:
	vytvorit soubor rozdeleni.txt a pouzit
	odradkovani LF ne CR LF

*/

int main(){	
	
	errors_ctor(global_err_ptr);

	tok_arr_t token_array;
	tok_arr_ctor(&token_array);
		
	lex_tokenize_file(&token_array,"./code_tests/ifelse.php");
	tok_arr_debug_print(token_array);
	
	if(global_err_ptr->error){
		tok_arr_dtor(&token_array);
		return global_err_ptr->error;
	}
	
	stx_node_t* ast = parser_build_all(&token_array);
	stx_tree_to_json(ast);
	
	if(global_err_ptr->error){
		stx_node_dtor(ast);
		tok_arr_dtor(&token_array);
		return global_err_ptr->error;
	}
	
	analyze_ast(ast);
	
	if(global_err_ptr->error){
		stx_node_dtor(ast);
		tok_arr_dtor(&token_array);
		return global_err_ptr->error;
	}
	
	// generates code from ast - working on it
	generate_code(ast);

	
	stx_node_dtor(ast);
	tok_arr_dtor(&token_array);
	
	
	return 0;
}