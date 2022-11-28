#include "expr_parser.h"
/*

TODO:
	Asi by bylo dobre vytvorit strukturu stmt parseru a do ni
	vlozit promene ktere jsou potreba v kazde funkci
	
	statement prejmenovat na expression
	stmt na expr
	
	Mozna by bylo dobre aby kazde pravidlo melo definovanou svoji funkci

	Upravit komentare

	Parsovani carek ve vyrazu pridat jako funkci pro pole tokenu

STATEMENT:
----------------------------------------------
F -> TEXT                     // "hello world" 
F -> NUMBER                   // 10
F -> NIL                      // null
F -> IDENTIFIER               // print($x,$y)
F -> VARIABLE                 // $a
F -> LEFT_PAREN T RIGHT_PAREN // (F)
F -> NEG F                    // !F
F -> MINUS F                  // -F
F -> F STAR F                 // F * F
F -> F SLASH F                // F / F
F -> F PLUS F                 // F + F
F -> F MINUS F                // F - F
F -> F EQUAL F                // F == F
F -> F GREATER_EQUAL F        // F >= F
F -> F LESS_EQUAL F           // F <= F
F -> F NOT_EQUAL F            // F != F
F -> F GREATER F              // F > F
F -> F LESS F                 // F < F
F -> F AND F                  // F and F
F -> F OR F                   // F or F
F -> F DOT F                  // F . F
*/


// Label
expr_node_t* expr_recursive_parser(tok_arr_t* ta_ptr, token_type expr_end_type);


/**
 * Calls expr_recursive_parser on tokens in given token array
 * that represents function call in expression
 * Given token array should have index pointing to name of the function
 * After calling this function the index of token array 
 * should point to next token after right parenthesis
 * Inserts returned subtree from expr_recursive_parser to given super tree
 * 
 * @param tok_arr Token array with given expression tokens
 * @param et_ptr expression tree pointer to given super tree
 */
void expr_function_parser(tok_arr_t* ta_ptr, expr_tree_t* et_ptr){
	
	expr_node_t* new_node = expr_tree_add_leaf(et_ptr, *tok_arr_get(ta_ptr));
	new_node->params_len = 0;
	new_node->params = NULL;
	
	// Skips identifier and left parenthesis token
	tok_arr_inc(ta_ptr,2);
	
	int commas = tok_arr_get_commas(ta_ptr);
	// Error while searching for commas in function parameters
	if(commas == -1){
		syntax_error(*tok_arr_get(ta_ptr), "Invalid function parameters format");
		return;
	}
	
	// Only one parameter given or empty
	if(commas == 0){
		expr_node_t* param_expr_ptr = expr_recursive_parser(ta_ptr, RIGHT_PAREN);
		if (global_err_ptr->error){
			expr_node_dtor(param_expr_ptr);
			return;
		}
		
		// Checks if function has parameter
		if(param_expr_ptr){
			new_node->params_len = 1;
			new_node->params = malloc(sizeof(expr_node_t*));
			new_node->params[0] = param_expr_ptr;
		}
		// Skips terminating token
		tok_arr_inc(ta_ptr,1);
	} else {
	// Two or more parameters given
		new_node->params_len = commas + 1;
		new_node->params = malloc(sizeof(expr_node_t*) * (commas + 1));
		/*
		  If there is an error in parsing parameters all
		  elements of array are initialized anyway
		*/
		for(int i = 0; i < new_node->params_len; i++){
			new_node->params[i] = NULL;
		}
		
		//Looping through all parameters
		for(int i = 0; i < commas + 1; i++){
			expr_node_t* param_expr_ptr = NULL;
			// Last parameter is terminated by right parenthesis others by comma
			if(i == commas){
				param_expr_ptr = expr_recursive_parser(ta_ptr, RIGHT_PAREN);
			} else {
				param_expr_ptr = expr_recursive_parser(ta_ptr, COMMA);
			}
			
			if(global_err_ptr->error){
				expr_node_dtor(param_expr_ptr);
				return;
			}
			// Empty paramater is allowed only if there is no comma
			if(!param_expr_ptr){
				syntax_error(*tok_arr_get(ta_ptr), "Empty parameter in function call with multiple parameters");
				return;
			}
			
			// Skips terminating token
			tok_arr_inc(ta_ptr,1);
			
			new_node->params[i] = param_expr_ptr;
		}
		
	}
	
}

/**
 * Calls expr_recursive_parser on tokens in given token array
 * Given token array should have index pointing to start of the expression
 * After calling this function the index of token array 
 * should point to next token after right parenthesis
 * Inserts returned subtree from expr_recursive_parser to given super tree
 * 
 * @param tok_arr Token array with given expression tokens
 * @param et_ptr expression tree pointer to given super tree
 */
void expr_parens_parser(tok_arr_t* ta_ptr, expr_tree_t* et_ptr){
	
	// Skips left parenthesis
	tok_arr_inc(ta_ptr,1);
	
	expr_node_t* sub_expr_ptr = expr_recursive_parser(ta_ptr,RIGHT_PAREN);
	// Invalid expression in parentheses 
	if(global_err_ptr->error){
		expr_node_dtor(sub_expr_ptr);
		return;
	}
	
	// There is no expression in parentheses
	if(!sub_expr_ptr){
		syntax_error(*tok_arr_get(ta_ptr), "Parenthesis are empty");
		return;
	}
	
	sub_expr_ptr->precedence = -1;
	expr_tree_extend(et_ptr, -1, sub_expr_ptr);
	
	// Skips right parenthesis
	tok_arr_inc(ta_ptr,1);
}

/**
 * Goes through given token array token by token
 * and builds expression tree
 * Recursivly calls itself for every fuction parameter and
 * expression in parenthesis 
 * 
 * @param tok_arr Token array with given expression tokens
 * @param expr_end_type Type of token that terminates the expression
 * @returns Pointer to newly created expression tree or NULL
 */
expr_node_t* expr_recursive_parser(tok_arr_t* ta_ptr, token_type expr_end_type){	
	// Empty expression
	if(tok_arr_cmp(ta_ptr,expr_end_type)){
		return NULL;
	}
	
	expr_tree_t et;
	expr_tree_t* et_ptr = &et;
	expr_tree_ctor(et_ptr);
	int terminal = 0;
	
	// Loops until terminating token is found
	while(!tok_arr_cmp(ta_ptr,expr_end_type)){
		
		//Checks if there are more tokens
		if(tok_arr_on_end(ta_ptr)){
			syntax_error(*tok_arr_get_offset(ta_ptr,-1), "Unexpected expression ending");
			return et_ptr->root;
		}
		
		// Function call
		if(tok_arr_cmp(ta_ptr,IDENTIFIER) && tok_arr_cmp_offset(ta_ptr, LEFT_PAREN, 1)){
			if(terminal){
				syntax_error(*tok_arr_get(ta_ptr), "Two or more operands without operator");
				return et_ptr->root;
			}
			
			expr_function_parser(ta_ptr,et_ptr);
			
			if(global_err_ptr->error){
				return et_ptr->root;
			}
			terminal = 1;
			continue;
		}
		
		// Parenthesis
		if(tok_arr_cmp(ta_ptr,LEFT_PAREN)){
			if(terminal){
				syntax_error(*tok_arr_get(ta_ptr), "There can't be parenthesis after non terminal");
				return et_ptr->root;
			}
			
			expr_parens_parser(ta_ptr,et_ptr);
			
			if(global_err_ptr->error){
				return et_ptr->root;
			};
			terminal = 1;
			continue;
		}
		
		// Terminals
		if(tok_arr_cmp_range(ta_ptr,VARIABLE,NIL)){
			if(terminal){
				syntax_error(*tok_arr_get(ta_ptr), "Two or more operands without operator");
				return et_ptr->root;
			}
			expr_tree_add_leaf(et_ptr, *tok_arr_get_next(ta_ptr));
			terminal = 1;
			continue;
		}
		
		// Non-Terminal
		if(tok_arr_cmp_range(ta_ptr,MINUS,LESS)){
			if(!terminal){
				syntax_error(*tok_arr_get(ta_ptr), "Operator missing one or both operands");
				return et_ptr->root;
			}
			expr_tree_add_branch(et_ptr, *tok_arr_get_next(ta_ptr));
			terminal = 0;
			continue;
		}
		
		syntax_error(*tok_arr_get(ta_ptr), "Unexpected token in expression");
		return et_ptr->root;
		
	}
	
	// Non empty expression should end by terminal
	if(!terminal){
		syntax_error(*tok_arr_get_offset(ta_ptr,-1), "Expression ends with operator");
		return et_ptr->root;
	}
	
	return et_ptr->root;
}


/**
 * Calls expr_recursive_parser function and if there is an error
 * calls detor function for expression root node
 * 
 * @param tok_arr Token array with given expression tokens
 * @param expr_end_type Type of token that terminates the expression
 * @returns Pointer to newly created expression tree or NULL
 */
expr_node_t* expr_parse(tok_arr_t* ta_ptr, token_type expr_end_type){	
	expr_node_t* expr_ptr = expr_recursive_parser(ta_ptr, expr_end_type);
	
	if(global_err_ptr->error){
		expr_node_dtor(expr_ptr);
		return NULL;
	}
	
	return expr_ptr;
}