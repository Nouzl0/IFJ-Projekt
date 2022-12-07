/**
 * Project: Implementace překladače imperativního jazyka IFJ22
 * 
 * @file semantic_analyzer.c
 * @brief Semantic analyzer
 * 
 * @author Nikolas Nosál <xnosal01>
 * @author Adam Mrkva <xmrkva04>
 * @author Rostislav Navrátil <xnavra72>
 * @author David Nevrlka <xnevrl00>
 */

#include "semantic_analyzer.h"
STList* sym_table;
STList* func_table;


/**
 * Compares data types
 * Nullable data types are matched with non nullable counter part 
 * 
 * @param expected token to compare against
 * @param returned token to compare with
 * @returns true if expected token is same as returned or normalized returned
 */
bool valid_return_type(token_type expected, token_type returned){
	switch(returned){
		case NIL:
			return expected >= NIL_INT && expected <= VOID;
		case INT:
			return expected == NIL_INT || expected == INT;
		case FLOAT:
			return expected == NIL_FLOAT || expected == FLOAT;
		case STRING:
			return expected == NIL_STRING || expected == STRING;
		default:
			return false;
	}
	return false;
} 


/**
 * Returns non nullable data type by passed nullable data type
 * 
 * @param tok Given nullable data type or any other token
 * @returns non nullable data type or any other token
 */
token_type return_normalize(token_type tok){
	switch(tok){
		case NIL_INT:
			return INT;
		case NIL_FLOAT:
			return FLOAT;
		case NIL_STRING:
			return STRING;
		default:
			return tok;
	}
	return tok;
	
} 


/**
 * Saves function name, return type and parameter types to function table
 * 
 * @param table Given function table
 * @param func_item Pointer to syntax node that stores function definiton
 */
void register_function(STList* table, stx_node_t* func_item){
	
	token_t* name_tok_ptr = func_item->items[0]->token;
	char* func_name = name_tok_ptr->content;
	int params_len = func_item->items[1]->items_len;
	token_type return_type = func_item->token->type;
	
	STElementDataPtr data = ST_DataGet(table, func_name);
	// Function name is already in function table
	if(data){
		semantic_error(SEMANTIC_ERROR, *func_item->token, "Function with this name is already defined");
		return;
	}
	
	ST_Create(table, func_name);
	
	data = ST_DataGet(table, func_name);
	
	data->type = return_type;
	data->tok_ptr = name_tok_ptr;
	
	
	data->params = malloc(sizeof(STDataParam) * params_len);
	data->param_len = func_item->items[1]->items_len;
	
	// Adding parameters name and type to function table 
	for (int i = 0; i < func_item->items[1]->items_len; i++){
		stx_node_t* type_item = func_item->items[1]->items[i];
		data->params[i].type = type_item->token->type;
		stx_node_t* name_item = type_item->items[0];
		data->params[i].param_name = name_item->token->content;
	}
}


/**
 * Returns data type of given expression
 * Calls itself recursively for every operand and function call parameter
 * 
 * @param expr Pointer to expression nodes
 * @returns Token which is data type given expression
 */
token_type rec_check_types(expr_node_t* expr){
	if(!expr){
		return NIL;
	}
	if(global_err_ptr->error){
		return VOID;
	}
	
	// All operands
	if(expr->token.type >= MINUS && expr->token.type <= LESS){
		token_type left = rec_check_types(expr->left);
		token_type right = rec_check_types(expr->right);
		
		// Math and value comparison operators
		if( (expr->token.type >= MINUS && expr->token.type <= SLASH) || (expr->token.type >= TYPE_EQUAL && expr->token.type <= LESS) ){
			if(left == STRING || left == NIL || right == STRING || right == NIL){
				semantic_error(VARIABLE_TYPE_ERROR, expr->token, "Invalid type of operand for this operation");
				return VOID;
			}
			
			// Division
			if(expr->token.type == SLASH){
				return FLOAT;
			}
			
			if(left == FLOAT || right == FLOAT){
				return FLOAT;
			} else {
				return INT;
			}
			
		}
		
		// String concat
		if (expr->token.type == DOT){
			if(left == STRING && right == STRING){
				return STRING;
			}else{
				semantic_error(VARIABLE_TYPE_ERROR, expr->token, "Invalid type of operand for this operation");
				return VOID;
			}
		}
		
		// Type comparators
		if(expr->token.type == TYPE_EQUAL || expr->token.type == TYPE_NOT_EQUAL){
			return INT;
		}
		
		return VOID;
	}
	
	// Constants to data types
	if(expr->token.type == NUMBER){
		return INT;
	}
	
	if(expr->token.type == FRACTION){
		return FLOAT;
	}
	
	if(expr->token.type == TEXT){
		return STRING;
	}
	
	// Variable to data type
	if(expr->token.type == VARIABLE){
		STElementDataPtr data = ST_DataGet(sym_table, expr->token.content);
		
		if(data){
			return data->type;
		} else {
			semantic_error(UNDEFINED_VARIABLE_ERROR, expr->token, "Undefined variable");
			return VOID;
		}
	}
	
	// Function return type
	if(expr->token.type == IDENTIFIER){
		STElementDataPtr data = ST_DataGet(func_table, expr->token.content);
		token_type return_type; 
		if(data){
			return_type = data->type;
			
			if(return_type == VOID){
				return_type = NIL;
			}
			
		} else {
			semantic_error(UNDEFINED_FUNCTION_ERROR, expr->token, "Undefined function");
			return VOID;
		}
		
		// write builtin with random number of parameters
		if(data->param_len == -1){
			for(int i = 0; i < expr->params_len; i++){
				rec_check_types(expr->params[i]);
			}
			return NIL;
		}
		// intval builtin with more possible data types of parameter
		if(data->param_len == -2){
			if(expr->params_len == 1){
				token_type eval_type = rec_check_types(expr->params[0]);
				if(eval_type == NIL || eval_type == FLOAT){
					return INT;
				}
					
			}
			semantic_error(FUNCTION_CALL_ERROR, expr->token, "Invalid format of parameters in calling builtin function");
			return VOID;
		}
		
		// floatval builtin with more possible data types of parameter
		if(data->param_len == -3){
			if(expr->params_len == 1){
				token_type eval_type = rec_check_types(expr->params[0]);
				if(eval_type == NIL || eval_type == INT){
					return FLOAT;
				}
					
			}
			semantic_error(FUNCTION_CALL_ERROR, expr->token, "Invalid format of parameters in calling builtin function");
			return VOID;
		}
		
		// Compares number of parameters in call with definition
		if(data->param_len != expr->params_len){
			semantic_error(FUNCTION_CALL_ERROR, expr->token, "Number of parameters in function call differs from function definition");
			return VOID;
		}
		
		// Compares types of parameters in call with definition
		for(int i = 0; i < expr->params_len; i++){
			token_type eval_type = rec_check_types(expr->params[i]);
			if(data->params[i].type != eval_type){
				semantic_error(FUNCTION_CALL_ERROR, expr->token, "Invalid type of parameter");
				return VOID;
			}
		}
		
		return return_normalize(return_type);
	}
	return expr->token.type;
}


/**
 * Goes through items in syntax tree code block and checks for right data types
 * Calls itself recursively for sub code blocks of if, while and function 
 * 
 * @param block Pointer to syntax node 
 * @param func_name String that stores name of function
 */
void analyze_block(stx_node_t* block, char* func_name){
	
	if(!block){
		return;
	}

	STElementDataPtr data;
	for (int i = 0; i < block->items_len; i++){
		if(global_err_ptr->error){
			return;
		}
		
		stx_node_t* item = block->items[i];
		switch(item->type){
			case EXPR:
				rec_check_types(item->expr);
				break;
			
			case ASSIGNEXPR:;
				char* var_name = item->token->content;
				
				token_type var_type = rec_check_types(item->expr);				
				
				data = ST_DataGet(sym_table, var_name);
				
				// Variable already exists, only changes type
				if(data){
					data->type = var_type;
					data->tok_ptr = item->token;
				} else {
				// Inserts variable name to symbol table
					ST_Create(sym_table, var_name);
					data = ST_DataGet(sym_table, var_name);
					data->type = var_type;
					data->tok_ptr = item->token;
				}
				
				break;
			
			case RETEXPR:
				// Return statement allowed only in function code block
				if(!func_name){
					semantic_error(FUNCTION_RETURN_ERROR, *item->token, "Return statement not in function");
					return;
				}
				
				data = ST_DataGet(func_table, func_name);
				
				if(!valid_return_type(data->type,rec_check_types(item->expr))){
					if(global_err_ptr->error){
						return;
					}
					semantic_error(FUNCTION_RETURN_ERROR, *item->token, "Returned type differs from function definition");
				}
				break;
			
			case IFELSE:
				rec_check_types(item->expr);
				//If block
				analyze_block(item->items[0], func_name);
				//Else block
				analyze_block(item->items[1], func_name);
				break;
			
			case WHILEBLOCK:
				rec_check_types(item->expr);
				analyze_block(item, func_name);
				break;
				
			default:
				continue;
		}
	
	}
	
}


/**
 * Inserts builtin functions definitions to function table
 * 
 * @param table Pointer to function table
 */
void insert_builtins(STList* table){
	STElementDataPtr data;
	
	ST_Create(table, "reads");
	data = ST_DataGet(table, "reads");
	data->type = NIL_STRING;
	data->param_len = 0;
	
	ST_Create(table, "readi");
	data = ST_DataGet(table, "readi");
	data->type = NIL_INT;
	data->param_len = 0;
	
	ST_Create(table, "readf");
	data = ST_DataGet(table, "readf");
	data->type = NIL_FLOAT;
	data->param_len = 0;
	
	ST_Create(table, "write");
	data = ST_DataGet(table, "write");
	data->type = NIL;
	data->param_len = -1;
	
	ST_Create(table, "intval");
	data = ST_DataGet(table, "intval");
	data->type = INT;
	data->param_len = -2;
	
	ST_Create(table, "floatval");
	data = ST_DataGet(table, "floatval");
	data->type = FLOAT;
	data->param_len = -3;
	
	ST_Create(table, "strlen");
	data = ST_DataGet(table, "strlen");
	data->type = INT;
	data->param_len = 1;
	data->params = malloc(sizeof(STDataParam) * data->param_len);
	data->params[0].type = STRING;
	data->params[0].param_name = NULL;
 	
	
	ST_Create(table, "substring");
	data = ST_DataGet(table, "substring");
	data->type = NIL_STRING;
	data->param_len = 3;
	data->params = malloc(sizeof(STDataParam) * data->param_len);
	data->params[0].type = STRING;
	data->params[0].param_name = NULL;
	data->params[1].type = INT;
	data->params[1].param_name = NULL;
	data->params[2].type = INT;
	data->params[2].param_name = NULL;
	
	ST_Create(table, "ord");
	data = ST_DataGet(table, "ord");
	data->type = INT;
	data->param_len = 1;
	data->params = malloc(sizeof(STDataParam) * data->param_len);
	data->params[0].type = STRING;
	data->params[0].param_name = NULL;
	
	ST_Create(table, "chr");
	data = ST_DataGet(table, "chr");
	data->type = STRING;
	data->param_len = 1;
	data->params = malloc(sizeof(STDataParam) * data->param_len);
	data->params[0].type = INT;
	data->params[0].param_name = NULL;
	
}


/**
 * Goes through root node of syntax tree and calls analyze functions
 * At the start populates function table with builtins and then with 
 * defined functions
 * Then analyzes functions code block
 * At the end analyzes rest of the items
 *
 * @param ast_root Pointer to root syntax node
 */
void analyze_ast(stx_node_t* ast_root){
	if(ast_root->items_len < 1){
		return;
	}
	
	func_table = ST_Init(10);
	
	insert_builtins(func_table);
	
	
	// Loops through all functions
	for (int i = 0; i < ast_root->items_len; i++){
		stx_node_t* item = ast_root->items[i];
		if (item->type == FUNCBLOCK){
			register_function(func_table,item);
		}
	}

	// Loops through all functions
	for (int i = 0; i < ast_root->items_len; i++){
		if(global_err_ptr->error){
			ST_Dispose(&func_table);
			return;
		}	
		stx_node_t* item = ast_root->items[i];
		if (item->type == FUNCBLOCK){
			// Clean symbol table is created for every function
			sym_table = ST_Init(10);
			
			// Adding function parameter definitions to symbol table as variables
			for (int i = 0; i < item->items[1]->items_len; i++){
				stx_node_t* type_item = item->items[1]->items[i];
				stx_node_t* name_item = type_item->items[0];
			
				ST_Create(sym_table, name_item->token->content);
	
				STElementDataPtr data = ST_DataGet(sym_table, name_item->token->content);
				data->type = type_item->token->type;
				data->tok_ptr = name_item->token;
			}
			
			analyze_block(item->items[2],item->items[0]->token->content);
			
			ST_Dispose(&sym_table);

		}
	}
	
	sym_table = ST_Init(10);
	// Finally analyzes rest of items in root node
	analyze_block(ast_root,NULL);
	ST_Dispose(&sym_table);
	
	ST_Dispose(&func_table);

}