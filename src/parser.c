#include "parser.h"

/*

Parser rules:
------------------------------------------------
//  Assign to variable
S -> VARIABLE, ASSIGN, {Expression}, SEMICOLON

// Expression
S -> [VARIABLE|IDENTIFIER|NUMBER|TEXT|NIl] -> {Expression}, SEMICOLON

// Function definition
S -> FUNC, IDENTIFIER, LEFT_PAREN, {[STRING|INT|FLOAT], VARIABLE}, COMMA, {...},
     RIGHT_PAREN, DDOT, [NIL_INT|NIL_FLOAT|NIL_STRING|STRING|INT|FLOAT], LEFT_BRACE, {Parser}, RIGHT_BRACE

// return statement
S -> RETURN, {Expression}, SEMICOLON

// if statement
S -> IF, LEFT_PAREN, {Expression}, RIGHT_PAREN, LEFT_BRACE, {Parser}, RIGHT_BRACE (ELSE, LEFT_BRACE, {Parser}, RIGHT_BRACE)

// while statement
S -> WHILE, LEFT_PAREN, {Expression}, RIGHT_PAREN, LEFT_BRACE, {Parser}, RIGHT_BRACE


Abstract syntax tree representation:
------------------------------------------------
BLOCK->items = {[ASSIGNEXPR|EXPR|RETEXPR|FUNCBLOCK|IFELSE|WHILEBLOCK],...}

ASSIGNEXPR->token - Name of variable that is assigned to
ASSIGNEXPR->expr  - Expression tree of assigning expression

EXPR->expr - Expression tree

FUNCBLOCK->token - Stores function return data type
FUNCBLOCK->items = [FUNCNAME][FUNCPARAMS][BLOCK]

	FUNCNAME->token - Name of function

	FUNCPARAMS->items = {[PARAMTYPE],...}
	
		PARAMTYPE->token - Data type of n function parameter
		PARAMTYPE->items = [PARAM]
			
			PARAM->token - Name of n function parameter

RETEXPR->expr - Expression tree of expression requested to return

IFELSE->expr - Expression tree of condition
IFELSE->items = [BLOCK][BLOCK]

WHILEBLOCK->expr - Expression tree of condition
WHILEBLOCK->items = {[ASSIGNEXPR|EXPR|RETEXPR|FUNCBLOCK|IFELSE|WHILEBLOCK],...}
*/


//Label
void parser_build_block(stree_item_t* parent_node, tok_arr_t* ta_ptr, bool is_root);

/**
 * Creates sub trees and insterts data for function name,
 * parameters and code block
 * Sub trees are appended to super tree give as parameter 
 *
 * @param parent_node Pointer to super tree for appending parsed sub trees
 * @param ta_ptr Pointer to token array
 */
void parser_build_function(stree_item_t* parent_node, tok_arr_t* ta_ptr){
	// Creating parent function node and adding to tree
	stree_item_t* func_node = stree_new_item(FUNCBLOCK,3);
	stree_insert_to_block(parent_node,func_node);
	
	// Creating child node for function name
	stree_item_t* name_node = stree_new_item(FUNCNAME,0);
	stree_insert_to_block(func_node,name_node);
	name_node->token = tok_arr_get_offset(ta_ptr,-2);
	
	// Creating parent node for function parameters nodes
	stree_item_t* params_node = stree_new_item(FUNCPARAMS,5);
	stree_insert_to_block(func_node,params_node);
	
	// Creating parent node for function body code block
	stree_item_t* body_node = stree_new_block(1);
	stree_insert_to_block(func_node,body_node);
	
	int separator = 1;
	// Loops until all parameters are parsed
	while(true){
		// Parameter type and parameter name
		if(separator && tok_arr_cmp_range(ta_ptr, INT, STRING) && tok_arr_cmp_offset(ta_ptr,VARIABLE,1)){
			
			// Creating parent node that stores type
			stree_item_t* type_node = stree_new_item(PARAMTYPE,1);
			stree_insert_to_block(params_node,type_node);
			type_node->token = tok_arr_get(ta_ptr);
			
			// Creating child node that stores parameter name
			stree_item_t* param_node = stree_new_item(PARAM,0);
			stree_insert_to_block(type_node,param_node);
			param_node->token = tok_arr_get_offset(ta_ptr,1);
			
			tok_arr_inc(ta_ptr,2);
			separator = 0;

		// Comma as parameter separator
		} else if (!separator && tok_arr_cmp(ta_ptr,COMMA)) {
			separator = 2;
			tok_arr_inc(ta_ptr,1);
			
		// Right parenthesis as parameters terminating token
		} else if (separator < 2 && tok_arr_cmp(ta_ptr,RIGHT_PAREN)){
			tok_arr_inc(ta_ptr,1);
			break;
			
		// Other invalid tokens
		} else {
			syntax_error(*tok_arr_get(ta_ptr), "Invalid format of function parameters");
			return;
		}
	}
	
	//Checks right format of footer of function definition
	if(tok_arr_cmp_skip(ta_ptr,DDOT) && tok_arr_cmp_range(ta_ptr,NIL_INT,STRING) && tok_arr_cmp_offset(ta_ptr,LEFT_BRACE,1)){
		func_node->token = tok_arr_get(ta_ptr);
		tok_arr_inc(ta_ptr,2);
	} else {
		syntax_error(*tok_arr_get(ta_ptr), "Invalid format of function return type");
		return;
	}
		
	parser_build_block(body_node,ta_ptr,0);
	
}


/**
 * Creates sub trees with if condition, if code block and else code block
 *
 * @param parent_node Pointer to super tree for appending parsed sub trees
 * @param ta_ptr Pointer to token array
 */
void parser_build_ifelse(stree_item_t* parent_node, tok_arr_t* ta_ptr){
	ptree_item_t* expr_ptr = expr_parse(ta_ptr,RIGHT_PAREN);
	
	if(global_err_ptr->error){
		return;
	}
	
	if(!expr_ptr){
		syntax_error(*tok_arr_get(ta_ptr), "Valid expression expected");
		return; 
	}
	
	// Creating parent node that stores condition expression, if and else code blocks
	stree_item_t* ifelse_node = stree_new_item(IFELSE,2);
	stree_insert_to_block(parent_node, ifelse_node);
	ifelse_node->stmt = expr_ptr;	
	
	// Checks expression terminating token and if code block starting token
	if (!tok_arr_cmp_skip(ta_ptr,RIGHT_PAREN) || !tok_arr_cmp_skip(ta_ptr,LEFT_BRACE)){
		syntax_error(*tok_arr_get(ta_ptr), "Invalid token, expected ) or {");
		return;
	}
	
	// Creating parent node for if code block
	stree_item_t* if_body_node = stree_new_block(parent_node->level+1);
	stree_insert_to_block(ifelse_node,if_body_node);
	
	parser_build_block(if_body_node,ta_ptr,0);
	
	// Checks for else keyword
	if (tok_arr_cmp(ta_ptr,ELSE)){
		
		// Checks else code block starting brace {
		if (!tok_arr_cmp_offset(ta_ptr,LEFT_BRACE,1)){
			syntax_error(*tok_arr_get(ta_ptr), "Missing { before else code block");
			return;
		}
		
		
		
		tok_arr_inc(ta_ptr,2);
		// Creating parent node for else code block
		stree_item_t* else_body_node = stree_new_block(parent_node->level+1);
		stree_insert_to_block(ifelse_node,else_body_node);
		
		parser_build_block(else_body_node,ta_ptr,0);
		
	}
	

}


/**
 * Creates sub trees with while condition and while code block
 *
 * @param parent_node Pointer to super tree for appending parsed sub trees
 * @param ta_ptr Pointer to token array
 */
void parser_build_while(stree_item_t* parent_node, tok_arr_t* ta_ptr){
	ptree_item_t* expr_ptr = expr_parse(ta_ptr,RIGHT_PAREN);
	
	if(global_err_ptr->error){
		return;
	}
	
	if(!expr_ptr){
		syntax_error(*tok_arr_get(ta_ptr), "Valid expression expected");
		return; 
	}
	
	
	// Creating parent node that stores condition expression and while code block
	stree_item_t* while_node = stree_new_item(WHILEBLOCK,5);
	stree_insert_to_block(parent_node, while_node);
	while_node->stmt = expr_ptr;	
	
	// Checks expression terminating token and while code block starting token
	if (!tok_arr_cmp_skip(ta_ptr,RIGHT_PAREN) || !tok_arr_cmp_skip(ta_ptr,LEFT_BRACE)){
		syntax_error(*tok_arr_get(ta_ptr), "Invalid token, expected ) or {");
		return;
	}

	while_node->level = parent_node->level+1;	
	parser_build_block(while_node,ta_ptr,0);
	

}


/**
 * Goes through token array token by token and builds syntax tree
 * using rules above
 * 
 * @param parent_node Pointer to super tree for appending parsed sub tree
 * @param ta_ptr Pointer to token array
 * @param is_root Determines if it root block or not
 */
void parser_build_block(stree_item_t* parent_node, tok_arr_t* ta_ptr, bool is_root){
	token_type func_decl_header[] = {
		FUNC,IDENTIFIER,LEFT_PAREN
	};
	
	while(!tok_arr_on_end(ta_ptr)){
	
		if (tok_arr_cmp(ta_ptr,RIGHT_BRACE) && !is_root){
			break;
		}
		
		if(global_err_ptr->error){
			return;
		}
		
		// Assign of expression to variable
		if(tok_arr_cmp(ta_ptr,VARIABLE) && tok_arr_cmp_offset(ta_ptr,ASSIGN,1)){		
			// Creating parent node that holds name of variable assigning to
			stree_item_t* assign_node = stree_new_item(ASSIGNEXPR,0);
			stree_insert_to_block(parent_node,assign_node);
			assign_node->token = tok_arr_get(ta_ptr);
			tok_arr_inc(ta_ptr,2);
			
			ptree_item_t* expr_ptr = expr_parse(ta_ptr,SEMICOLON);
			assign_node->stmt = expr_ptr;
			
			if(!expr_ptr){
				syntax_error(*tok_arr_get(ta_ptr), "Valid expression expected");
				return; 
			}
			
			if(!tok_arr_cmp_skip(ta_ptr,SEMICOLON)){
				syntax_error(*tok_arr_get(ta_ptr), "Invalid expression end probably missing ;");
				return;
			}
			continue;
		}
		
		// Only expression
		if(tok_arr_cmp_range(ta_ptr,IDENTIFIER,NIL) || tok_arr_cmp(ta_ptr,LEFT_PAREN)){
			
			ptree_item_t* expr_ptr = expr_parse(ta_ptr,SEMICOLON);
			stree_insert_stmt(parent_node, expr_ptr);
			
			if(!expr_ptr){
				syntax_error(*tok_arr_get(ta_ptr), "Valid expression expected");
				return; 
			}
			
			if(!tok_arr_cmp_skip(ta_ptr,SEMICOLON)){
				syntax_error(*tok_arr_get(ta_ptr), "Invalid expression end probably missing ;");
			}
			
			continue;
		}
		
		
		// Function declaration
		if(tok_arr_cmp_arr(ta_ptr,func_decl_header,3) && is_root){
			tok_arr_inc(ta_ptr,3);
			parser_build_function(parent_node, ta_ptr);
			continue;
		}
		
		
		//Return statement
		if(tok_arr_cmp(ta_ptr,RETURN)){
			tok_arr_inc(ta_ptr,1);
			
			ptree_item_t* expr_ptr = expr_parse(ta_ptr,SEMICOLON);
			
			if(global_err_ptr->error){
				return;
			}
			
			stree_item_t* return_node = stree_new_item(RETEXPR,0);
			stree_insert_to_block(parent_node, return_node);
			return_node->stmt = expr_ptr;
			tok_arr_inc(ta_ptr,1);
			continue;
		}
		
		//If and else statement
		if(tok_arr_cmp(ta_ptr,IF) && tok_arr_cmp_offset(ta_ptr,LEFT_PAREN,1)){
			tok_arr_inc(ta_ptr,2);
			parser_build_ifelse(parent_node, ta_ptr);
			continue;
		}
		
		//While statement
		if(tok_arr_cmp(ta_ptr,WHILE) && tok_arr_cmp_offset(ta_ptr,LEFT_PAREN,1)){
			tok_arr_inc(ta_ptr,2);
			parser_build_while(parent_node, ta_ptr);
			continue;
		}
		
		//File code ending with php footer
		if(tok_arr_cmp(ta_ptr,FOOTER)){
			break;
		}
		
		syntax_error(*tok_arr_get(ta_ptr), "Unexpected token");
		return;
	
	}
	
	if(!is_root){
		
		if(tok_arr_cmp(ta_ptr,RIGHT_BRACE)){
			tok_arr_inc(ta_ptr,1);
		} else {
			syntax_error(*tok_arr_get(ta_ptr), "Missing } after code block");
		}
		
	}
	
}


/**
 * Checks php and strict header format
 * Calls parser_build_block on root code block
 * 
 * @param ta_ptr Pointer to token array of whole file
 * @returns Pointer to root block representing whole file by syntax tree
 */
stree_item_t* parser_build_all(tok_arr_t* ta_ptr){

	token_type header_types[] = {
		HEADER,IDENTIFIER,LEFT_PAREN,IDENTIFIER,ASSIGN,NUMBER,RIGHT_PAREN,SEMICOLON
	};

	if(!tok_arr_cmp_arr(ta_ptr,header_types,8)){
		syntax_error(*tok_arr_get(ta_ptr), "Invalid format of file header");
		return NULL;
	}
	
	// Skips header
	tok_arr_inc(ta_ptr, 8);

	stree_item_t* root_node = stree_new_block(0);
	parser_build_block(root_node, ta_ptr, 1);

	return root_node;

}
