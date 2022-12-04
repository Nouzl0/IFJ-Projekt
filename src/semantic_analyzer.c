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
	
	
	ST_CreateResize(&table, func_name);
	
	STElementDataPtr data = ST_DataGet(table, func_name);
	
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

//Rekurzivne volano potreba zjistovat errory
token_type rec_check_types(expr_node_t* expr){
	if(!expr){
		return NIL;
	}
	if(global_err_ptr->error){
		return VOID;
	}
	
	//Zakladni matematicke operace
	if(expr->token.type >= MINUS && expr->token.type <= LESS){
		//Typy operadnu se musi schodovat
		//a kdyz se neschoduji je potreba pridat do stromu pretypovani
		//Jejich vysledekem je to co do nich leze a u SLASH je to vzdy float
		token_type left = rec_check_types(expr->left);
		token_type right = rec_check_types(expr->right);
		if(expr->token.type >= MINUS && expr->token.type <= STAR){
			//Pridat prevedeni na float kdyz je treba
			if(left == right){
				return left;
			}else{
				semantic_error(VARIABLE_TYPE_ERROR, expr->token, "Invalid type of operand for this operation");
				return VOID;
			} 
		}
		if(expr->token.type == SLASH){
			//Pridat prevedeni na float kdyz je treba
			return FLOAT;
		}
		if (expr->token.type == DOT){
			if(left == STRING && right == STRING){
				return STRING;
			}else{
				semantic_error(VARIABLE_TYPE_ERROR, expr->token, "Invalid type of operand for this operation");
				return VOID;
			}
		}
		if(expr->token.type == TYPE_EQUAL || expr->token.type == TYPE_NOT_EQUAL){
			return INT;
		}
		if(expr->token.type >= EQUAL && expr->token.type <= LESS){
			//Pridat prevedeni na float kdyz je potreba
			if(left == right){
				return INT;
			}else{
				semantic_error(VARIABLE_TYPE_ERROR, expr->token, "Invalid type of operand for this operation");
				return VOID;
			}
		}
		return VOID;
	}
	 
	if(expr->token.type == NUMBER){
		return INT;
	}
	
	if(expr->token.type == FRACTION){
		return FLOAT;
	}
	
	if(expr->token.type == TEXT){
		return STRING;
	}
	
	//Zjisteni typu promene
	if(expr->token.type == VARIABLE){
		STElementDataPtr data = ST_DataGet(sym_table, expr->token.content);
		
		if(data){
			return data->type;
		} else {
			semantic_error(UNDEFINED_VARIABLE_ERROR, expr->token, "Undefined variable");
			return VOID;
		}
	}
	
	//Zjisteni navratoveho typu funkce
	if(expr->token.type == IDENTIFIER){
		STElementDataPtr data = ST_DataGet(func_table, expr->token.content);
		token_type return_type; 
		if(data){
			return_type = data->type;
		} else {
			semantic_error(UNDEFINED_FUNCTION_ERROR, expr->token, "Undefined function");
			return VOID;
		}
		if(data->param_len != expr->params_len){
			semantic_error(FUNCTION_CALL_ERROR, expr->token, "Number of parameters in function call differs from function definition");
			return VOID;
		}
		//Porovnat pocet a typ parametru
		//pri volani funkce
		for(int i = 0; i < expr->params_len; i++){
			token_type eval_type = rec_check_types(expr->params[i]);
			if(data->params[i].type != eval_type){
				semantic_error(FUNCTION_CALL_ERROR, expr->token, "Invalid type of parameter");
				return VOID;
			}
		}
		return return_type;
	}
	return VOID;
}
/*
void analyze_assignexpr(stx_node_t* item){
	//Nazev promene do ktere se prirazuje: item->token->content;
	//Vyraz ktery se prirazuje: item->expr
}

void analyze_retexpr(stx_node_t* item){
	//Vyraz ktery se ma vratit: item->expr
}

void analyze_ifelse(stx_node_t* item){
	//Vyraz podminky: item->expr
	//if blok: item->items[0] 
	//else blok: item->items[1] tento blok tam byt nemusi
}

void analyze_whileblock(stx_node_t* item){
	//Vyraz podminky: item->expr
	//blok: item->items[0]
}
*/
void analyze_block(stx_node_t* block, bool is_root, char* func_name){
	
	if(!block){
		return;
	}
	
	for (int i = 0; i < block->items_len; i++){
		//printf("DEBUG\n");
		if(global_err_ptr->error){
			return;
		}

		stx_node_t* item = block->items[i];
		
		switch(item->type){
			case EXPR:
				rec_check_types(item->expr);
				break;
			
			case ASSIGNEXPR:
				//Vsechny prvky ktere jsou pridane do tabulky
				//jsou na konci bloku zase odebrany
				//tim zajistim aby promena nemohla byt pouzita z venku
				//analyze_assignexpr(item);
				break;
			
			case RETEXPR:
				if(!func_name){
					semantic_error(FUNCTION_RETURN_ERROR, *item->token, "Return statement not in function");
					return;
				}
				
				STElementDataPtr data = ST_DataGet(func_table, func_name);
				
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
				analyze_block(item->items[0], false, func_name);
				//Else block
				analyze_block(item->items[1], false, func_name);
				break;
			
			case WHILEBLOCK:
				rec_check_types(item->expr);
				analyze_block(item, false, func_name);
				break;
				
			default:
				return;
		}
	
	}
	
	
}

/*
void analyze_function(stx_node_t* func_item){
	//bud vytvori novou tabulku nebo bude pouzivat suffix -{nazev_funkce}
	//analyze_item(func_item->items[2]);
	
}
*/

void analyze_ast(stx_node_t* ast_root){
	if(ast_root->items_len < 1){
		//Prazdy blok
		return;
	}
	
	func_table = ST_Init(10);
	
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
			
			//Adding parameter definitions to symbol table as variables
			for (int i = 0; i < item->items[1]->items_len; i++){
				stx_node_t* type_item = item->items[1]->items[i];
				stx_node_t* name_item = type_item->items[0];
			
				ST_CreateResize(&sym_table, name_item->token->content);
	
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