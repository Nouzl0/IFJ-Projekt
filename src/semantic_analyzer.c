#include "semantic_analyzer.h"
STList* sym_table;
STList* func_table;


//void semantic_error(int error_code, token_t token, char* info);
/**
 * 
 * 
 * @param table 
 * @param func_item 
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



void register_function(STList* table, stx_node_t* func_item){
	
	token_t* name_tok_ptr = func_item->items[0]->token;
	char* func_name = name_tok_ptr->content;
	int params_len = func_item->items[1]->items_len;
	token_type return_type = func_item->token->type;
	
	STElementDataPtr data = ST_DataGet(table, func_name);
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
	
	for (int i = 0; i < func_item->items[1]->items_len; i++){
		stx_node_t* type_item = func_item->items[1]->items[i];
		data->params[i].type = type_item->token->type;
		stx_node_t* name_item = type_item->items[0];
		data->params[i].param_name = name_item->token->content;
	}
}


token_type rec_check_types(expr_node_t* expr){
	if(!expr){
		return NIL;
	}
	if(global_err_ptr->error){
		return VOID;
	}
	
	// All operands
	if(expr->token.type >= MINUS && expr->token.type <= LESS){
		//Typy operadnu se musi schodovat
		//a kdyz se neschoduji je potreba pridat do stromu pretypovani
		//Jejich vysledekem je to co do nich leze a u SLASH je to vzdy float
		token_type left = rec_check_types(expr->left);
		token_type right = rec_check_types(expr->right);
		if(expr->token.type >= MINUS && expr->token.type <= STAR){
			if(left == STRING || right == STRING){
				semantic_error(VARIABLE_TYPE_ERROR, expr->token, "Invalid type of operand for this operation");
				return VOID;
			}
			/*
			if(left == right){
				return left;
			}
			*/
			//expr->left
			//expr->right
			/*
			expr_node_t* new_node_ptr = malloc(sizeof(expr_node_t));
			new_node_ptr->is_terminal = 1;
			new_node_ptr->precedence = 0;
			new_node_ptr->token.type = IDENTIFIER;
			new_node_ptr->token.line = 0;
			new_node_ptr->token.column = tok;
			new_node_ptr->token.content = intval;
			new_node_ptr->params_len = 1;
			new_node_ptr->params = expr->left , expr->right;
			new_node_ptr->right = NULL;
			new_node_ptr->left = NULL;
			*/
			/*
			expr_node_t* new_node_ptr;
			if(left != right){
				
			}
			
			if(left == INT){
				
			} else if(left == FLOAT){
				
			}
			
			semantic_error(VARIABLE_TYPE_ERROR, expr->token, "Invalid type of operand for this operation");
			return VOID;
			*/
			
			return left;
			
			//Pridat prevedeni na float kdyz je treba
			if(left == right){
				return left;
			}else{
				semantic_error(VARIABLE_TYPE_ERROR, expr->token, "Invalid type of operand for this operation");
				return VOID;
			} 
		}
		
		// Division
		if(expr->token.type == SLASH){
			//Pridat prevedeni na float kdyz je treba
			return FLOAT;
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
		
		// Value comparators
		if(expr->token.type >= EQUAL && expr->token.type <= LESS){
			//Pridat prevedeni na float kdyz je potreba
			return left;
			
			if(left == right){
				return INT;
			}else{
				semantic_error(VARIABLE_TYPE_ERROR, expr->token, "Invalid type of operand for this operation");
				return VOID;
			}
			
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
		return return_type;
	}
	return expr->token.type;
}

void analyze_block(stx_node_t* block, bool is_root, char* func_name){
	
	if(!block){
		return;
	}
	
	char** remove_arr = malloc(sizeof(char*) * 5);
	int remove_arr_size = 5;
	int remove_arr_len = 0;
	
	STElementDataPtr data;
	for (int i = 0; i < block->items_len; i++){
		if(global_err_ptr->error){
			free(remove_arr);
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
				
				if(data){
					data->type = var_type;
					data->tok_ptr = item->token;
				} else {
					ST_Create(sym_table, var_name);
					data = ST_DataGet(sym_table, var_name);
					data->type = var_type;
					data->tok_ptr = item->token;
					/* Unused scope of variables
					if(!is_root){
						
						if(remove_arr_len >= remove_arr_size){
							remove_arr_size = remove_arr_size * 2;
							char** remove_arr = realloc(remove_arr,sizeof(char*) * remove_arr_size);
						}
						
						remove_arr[remove_arr_len] = var_name;
						remove_arr_len++;
					}
					*/
				}
				
				break;
			
			case RETEXPR:
				if(!func_name){
					semantic_error(FUNCTION_RETURN_ERROR, *item->token, "Return statement not in function");
					free(remove_arr);
					return;
				}
				
				data = ST_DataGet(func_table, func_name);
				
				if(!valid_return_type(data->type,rec_check_types(item->expr))){
					if(global_err_ptr->error){
						free(remove_arr);
						return;
					}
					semantic_error(FUNCTION_RETURN_ERROR, *item->token, "Returned type differs from function definition");
				}
				break;
			
			case IFELSE:
				rec_check_types(item->expr);
				//If block
				analyze_block(item->items[0], false, func_name);
				//Else block
				analyze_block(item->items[1], false, func_name);
				break;
			
			case WHILEBLOCK:
				rec_check_types(item->expr);
				analyze_block(item, false, func_name);
				break;
				
			default:
				continue;
		}
	
	}
	/* Unused scope of variables
	if(!is_root){
		for (int i = 0; i < remove_arr_len; i++){
			ST_Delete(sym_table, remove_arr[i]);   
		}
	}
	*/
	free(remove_arr);
	
}

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
	data->param_len = 2;
	data->params = malloc(sizeof(STDataParam) * data->param_len);
	data->params[0].type = STRING;
	data->params[0].param_name = NULL;
	data->params[1].type = STRING;
	data->params[1].param_name = NULL;
	
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

void analyze_ast(stx_node_t* ast_root){
	if(ast_root->items_len < 1){
		//Prazdy blok
		return;
	}
	
	func_table = ST_Init(10);
	
	insert_builtins(func_table);
	
	
	//Prochazi vsechny funkce a bere jenom hlavicku
	for (int i = 0; i < ast_root->items_len; i++){
		stx_node_t* item = ast_root->items[i];
		if (item->type == FUNCBLOCK){
			register_function(func_table,item);
		}
	}

	//Prochazi vsechny funkce 
	for (int i = 0; i < ast_root->items_len; i++){
		if(global_err_ptr->error){
			ST_Dispose(&func_table);
			return;
		}	
		stx_node_t* item = ast_root->items[i];
		if (item->type == FUNCBLOCK){
			sym_table = ST_Init(10);
			
			//Adding function parameter definitions to symbol table as variables
			for (int i = 0; i < item->items[1]->items_len; i++){
				stx_node_t* type_item = item->items[1]->items[i];
				stx_node_t* name_item = type_item->items[0];
			
				ST_Create(sym_table, name_item->token->content);
	
				STElementDataPtr data = ST_DataGet(sym_table, name_item->token->content);
				data->type = type_item->token->type;
				data->tok_ptr = name_item->token;
			}
			
			analyze_block(item->items[2],true,item->items[0]->token->content);
			
			ST_Dispose(&sym_table);

		}
	}
	
	sym_table = ST_Init(10);
	//Prochazi vsechny prvky a anylyzuje je
	analyze_block(ast_root,true,NULL);
	ST_Dispose(&sym_table);
	



	/*
	//ST_Delete(func_table, "getMax" );

	
	STElementDataPtr data = ST_DataGet(func_table, "getMax");
	
	if(data){
		printf("je: %s\n", data->params[0].param_name);
	} else {
		printf("neni\n");
	}
	*/
	ST_Dispose(&func_table);

}