#include "statement_parser.h"
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

/**
 * Vraci pocet carek v poli tokenu podle zadaneho indexu
 *
 * @param tok_arr Pole tokenu
 * @param start_index Index v poli tokenu kde ma vyraz zancinat
 * @returns 0 kdyz vyraz neni platny nebo index konce vyrazu
 */
 
int get_count_of_commas(tok_arr_t tok_arr, int start_index){
	int index = start_index;
	int parens = 0;
	int count = 0;
	while (tok_arr.len > index){
		token_t tok = tok_arr.elems[index];
		//Detekuje carku
		if (tok.type == COMMA && !parens){
			count++;
		//Detekuje ukonceni vyrazu budto zadanym tokenem nebo carkou
		} else if (tok.type == RIGHT_PAREN && !parens){	
			return count;
		//Detekuje levou zavorku
		} else if (tok.type == LEFT_PAREN){
			parens++;
		//Detekuje pravou zavorku ktera neukoncuje vyraz ale znaci precedenci ve vyrazu
		} else if (tok.type == RIGHT_PAREN){
			parens--;
		} 
		
		index++;
		
	}
	return 0;
}


/**
 * Vraci index konce vyrazu v poli tokenu podle
 * tokenu ktery ma vyraz ukoncit
 *
 * @param tok_arr Pole tokenu
 * @param start_index Index v poli tokenu kde ma vyraz zancinat
 * @param ending Typ tokenu kterym ma vyraz koncit
 * @returns 0 kdyz vyraz neni platny nebo index konce vyrazu
 */
int get_stmt_end_index(error_handler_t* eh_ptr, tok_arr_t tok_arr, int start_index, token_type ending, int delimiter){
	int index = start_index;
	int offset = 0;
	int parens = 0;
	while (tok_arr.len > index+offset){
		token_t tok = tok_arr.elems[index+offset];
		
		//Detekuje ukonceni vyrazu budto zadanym tokenem nebo carkou
		if (delimiter && (tok.type == ending || tok.type == COMMA) && !parens ){
			return offset;
		//Detekuje ukonci pouze zadanym token
		} else if(!delimiter && tok.type == ending && !parens){
			return offset;
		//Detekuje levou zavorku
		} else if (tok.type == LEFT_PAREN){
			parens++;
		//Detekuje pravou zavorku ktera neukoncuje vyraz ale znaci precedenci ve vyrazu
		} else if (tok.type == RIGHT_PAREN){
			parens--;
		//Detekuje token ktery ve vyrazu neni povoleny
		} else if (tok.type < IDENTIFIER || tok.type > COMMA){
			//Token nepatri do vyrazu
			register_syntax_error(eh_ptr,tok.line,tok.column);
			return offset;
		}
		
		offset++;
		
	}
	
	//Kdyby vyraz obsahoval uplne posledni token coz znamena ze neni ukoncen
	register_syntax_error(eh_ptr,tok_arr.elems[index+offset].line,tok_arr.elems[index+offset].column);
	return offset;
}


/**
 * Vytvari z vyrazu ve tvaru pole tokenu strom precedence
 * 
 * @param tok_arr Pole tokenu
 * @param start_index Index v poli tokenu kde ma vyraz zancinat
 * @param end_index Index v poli tokenu na kterem ma vyraz koncit
 * @returns NULL kdyz je vyraz neplatny jinak ukazatel na korenovy prvek
 * stromu precedence
 */
ptree_item_t* parse_statement(error_handler_t* eh_ptr, tok_arr_t tok_arr, int start_index, int end_index){
	
	ptree_t btree;
	ptree_t* btree_ptr = &btree;
	ptree_ctor(btree_ptr);
	
	int terminal = 0;
	int index = start_index;
	for (; index < end_index; index++){
		token_t tok = tok_arr.elems[index];
		
		//Zavorky
		if (tok.type == LEFT_PAREN && !terminal){
			
			index++; //Preskoceni LEFT_PAREN (
			int end_offset = get_stmt_end_index(eh_ptr, tok_arr, index, RIGHT_PAREN, 0);
			
			if (eh_ptr->error){
				//Syntax error pri hledani zavorek a povolenych znaku
				return NULL;
			}
			
			ptree_item_t* stmt_tree_ptr = NULL;
			//Aby nebyl tvoren precedencni strom z prazdnych zavorek
			if (end_offset){
				//Rekurzivni volani na vytvoreni stromu pro vyraz v zavorkach
				stmt_tree_ptr = parse_statement(eh_ptr, tok_arr, index, index+end_offset);
				terminal = 1;
			}
			
			//Kdyz byl precedenci strom uspesne vytvoren
			if (stmt_tree_ptr != NULL){
				//Pripojuje precedencni strom zavorek do stavajiciho vyrazu
				stmt_tree_ptr->precedence = -1;
				ptree_extend(btree_ptr, -1, stmt_tree_ptr);
			}
			
			//Posunuje se o tolik jak je dlouhy vyraz v zavorkach
			index+= end_offset;
			continue;
		}
		
		
		//Terminal
		if (tok.type <= NIL && !terminal){
			//Volani funkce
			if (tok.type == IDENTIFIER && tok_arr.elems[index+1].type == LEFT_PAREN){
				
				ptree_item_t* ptree_item_ptr = prec_tree_add_leaf(btree_ptr, tok);
				
				index += 2;
				int commas = get_count_of_commas(tok_arr,index);
				ptree_item_ptr->params_len = 0;
				
				
				if(commas){
					ptree_item_ptr->params_len = commas + 1;
					ptree_item_ptr->params = malloc(sizeof(ptree_item_t*) * ptree_item_ptr->params_len); 
				}
				
				int count = 0;
				for (; count < commas; count++){
					int end_offset = get_stmt_end_index(eh_ptr, tok_arr, index, COMMA, 0);
					
					if (!end_offset){
						//Syntax error jeliko mezi carkami neni vyraz
						return NULL;
					}
						
					ptree_item_ptr->params[count] = parse_statement(eh_ptr,tok_arr,index,index+end_offset);
					
					//printf("Zacatek: %s\n",token_enum_to_string(tok_arr.elems[index].type));
					//printf("Konec: %s\n",token_enum_to_string(tok_arr.elems[index+end_offset].type));
					
					index += end_offset;
					index++; //Preskakuje ,
				}
				
				int end_offset = get_stmt_end_index(eh_ptr, tok_arr, index, RIGHT_PAREN, 0);
				
				if (!end_offset && commas){
					return NULL;
					//Syntax error, jsou zaznamenany carky ale posledni vyraz je prazdny
				}

				if (end_offset){
					
					if(!commas){
						ptree_item_ptr->params_len = 1;
						ptree_item_ptr->params = malloc(sizeof(ptree_item_t*));
					}
					
					ptree_item_ptr->params[count] = parse_statement(eh_ptr,tok_arr,index,index+end_offset);
				}
				
				
				//printf("Zacatek: %s\n",token_enum_to_string(tok_arr.elems[index].type));
				//printf("Konec: %s\n",token_enum_to_string(tok_arr.elems[index+end_offset].type));
				
				index += end_offset;
				
				terminal = 1;
				continue;
			} else if(tok.type != IDENTIFIER){
				prec_tree_add_leaf(btree_ptr, tok);
				terminal = 1;
				continue;
			}
		}
		
		//Binary Non-Terminal
		if (terminal){
			//non_terminal = 1;
			terminal = 0;
			ptree_add_branch(btree_ptr, tok);
			continue;
		}
		
		//Syntax error jelikoz nemohlo byt uplatneno ani jedno pravidlo
		register_syntax_error(eh_ptr,tok.line,tok.column);
		return NULL;

	}
	
	//Kazdy vyraz musi vzdy koncit neterminalem
	if(!terminal){
		register_syntax_error(eh_ptr,tok_arr.elems[index].line,tok_arr.elems[index].column);
		return NULL;
	}
	
	return btree_ptr->root;
	
}



//NEW

// Label
bool expr_recursive_parser(ptree_item_t** root_ptr, tok_arr_t* ta_ptr, token_type expr_end_type);


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
 * @returns 1 if there is an error otherwise 0
 */
bool expr_function_parser(tok_arr_t* ta_ptr, ptree_t* et_ptr){
	// Saves current token that stores function name
	token_t func_tok = *tok_arr_get(ta_ptr);
	// Skips identifier and left parenthesis token
	tok_arr_inc(ta_ptr,2);
	
	int commas = tok_arr_get_commas(ta_ptr);
	// Error while searching for commas in function parameters
	if(commas == -1){
		syntax_error(*tok_arr_get_offset(ta_ptr,-1), "Invalid function parameters format");
		return 1;
	}
	
	// Only one parameter given or empty
	if(commas == 0){
		ptree_item_t* param_expr_ptr = NULL;
		if (expr_recursive_parser(&param_expr_ptr, ta_ptr, RIGHT_PAREN)){
			ptree_dtor(param_expr_ptr);
			return 1;
		}
		
		// Checks if function has parameter
		if(param_expr_ptr){
			ptree_item_t* et_item_ptr = prec_tree_add_leaf(et_ptr, func_tok);
			et_item_ptr->params_len = 1;
			et_item_ptr->params = malloc(sizeof(ptree_item_t*));
			et_item_ptr->params[0] = param_expr_ptr;
		}
		tok_arr_inc(ta_ptr,1);
	} else {
	// Two or more parameters given
		ptree_item_t* et_item_ptr = prec_tree_add_leaf(et_ptr, func_tok);
		et_item_ptr->params_len = commas + 1;
		et_item_ptr->params = malloc(sizeof(ptree_item_t*) * (commas + 1));
		
		//Looping through all parameters
		for(int i = 0; i < commas + 1; i++){
			ptree_item_t* param_expr_ptr = NULL;
			int err = 0;
			// Last parameter is terminated by right parenthesis others by comma
			if(i == commas){
				err = expr_recursive_parser(&param_expr_ptr, ta_ptr, RIGHT_PAREN);
			} else {
				err = expr_recursive_parser(&param_expr_ptr, ta_ptr, COMMA);
			}
			
			if(err){
				ptree_dtor(param_expr_ptr);
				return 1;
			}
			// Empty paramater is allowed only if there is no comma
			if(!param_expr_ptr){
				syntax_error(*tok_arr_get_offset(ta_ptr,-1), "Empty parameter in function call with multiple parameters");
				return 1;
			}
			
			// Skips terminating token
			tok_arr_inc(ta_ptr,1);
			
			et_item_ptr->params[i] = param_expr_ptr;
		}
		
		
	}
	return 0;
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
 * @returns 1 if there is an error otherwise 0
 */
bool expr_parens_parser(tok_arr_t* ta_ptr, ptree_t* et_ptr){
	
	// Skips left parenthesis
	tok_arr_inc(ta_ptr,1);
	
	ptree_item_t* sub_expr_ptr = NULL;
	// Invalid expression in parenthesis 
	if(expr_recursive_parser(&sub_expr_ptr, ta_ptr,RIGHT_PAREN)){
		ptree_dtor(sub_expr_ptr);
		return 1;
	}
	
	// There is no expression in parenthesis
	if(!sub_expr_ptr){
		return 1;
		syntax_error(*tok_arr_get_offset(ta_ptr,-1), "Parenthesis are empty");
	}
	
	sub_expr_ptr->precedence = -1;
	ptree_extend(et_ptr, -1, sub_expr_ptr);
	
	// Skips right parenthesis
	tok_arr_inc(ta_ptr,1);
	return 0;
}

bool expr_recursive_parser(ptree_item_t** root_ptr, tok_arr_t* ta_ptr, token_type expr_end_type){	
	// Empty expression
	if(tok_arr_cmp(ta_ptr,expr_end_type)){
		return 0;
	}
	
	ptree_t et;
	ptree_t* et_ptr = &et;
	ptree_ctor(et_ptr);
	int terminal = 0;
	
	// Loops until terminating token is found
	while(!tok_arr_cmp(ta_ptr,expr_end_type)){
		
		//Checks if there are more tokens
		if(tok_arr_on_end(ta_ptr)){
			syntax_error(*tok_arr_get_offset(ta_ptr,-1), "Unexpected expression ending");
			return 1;
		}
		
		// Function call
		if(tok_arr_cmp(ta_ptr,IDENTIFIER) && tok_arr_cmp_offset(ta_ptr, LEFT_PAREN, 1)){
			if(terminal){
				syntax_error(*tok_arr_get_offset(ta_ptr,-1), "Two or more operands without operator");
				return 1;
			}
			
			if(expr_function_parser(ta_ptr,et_ptr)){
				return 1;
			}
			terminal = 1;
			continue;
		}
		
		// Parenthesis
		if(tok_arr_cmp(ta_ptr,LEFT_PAREN)){
			if(terminal){
				syntax_error(*tok_arr_get_offset(ta_ptr,-1), "There can't be parenthesis after non terminal");
				return 1;
			}
			
			if(expr_parens_parser(ta_ptr,et_ptr)){
				return 1;
			};
			terminal = 1;
			continue;
		}
		
		// Terminals
		if(tok_arr_cmp_range(ta_ptr,VARIABLE,NIL)){
			if(terminal){
				syntax_error(*tok_arr_get_offset(ta_ptr,-1), "Two or more operands without operator");
				return 1;
			}
			prec_tree_add_leaf(et_ptr, *tok_arr_get_next(ta_ptr));
			terminal = 1;
			continue;
		}
		
		// Non-Terminal
		if(tok_arr_cmp_range(ta_ptr,MINUS,LESS)){
			if(!terminal){
				syntax_error(*tok_arr_get_offset(ta_ptr,-1), "Operator missing one or both operands");
				return 1;
			}
			ptree_add_branch(et_ptr, *tok_arr_get_next(ta_ptr));
			terminal = 0;
			continue;
		}
		
		syntax_error(*tok_arr_get_offset(ta_ptr,-1), "Unexpected token in expression");
		return 1;
		
	}
	
	// Non empty expression should end by terminal
	if(!terminal){
		syntax_error(*tok_arr_get_offset(ta_ptr,-1), "Expression ends with operator");
		return 1;
	}
	
	(*root_ptr) = et_ptr->root;
	return 0;
}

// Overlooking function for expr_recursive_parser fucntion 
// that frees memory if there is an error when parsing or creating tree
ptree_item_t* expr_parse(tok_arr_t* ta_ptr, token_type expr_end_type){
	
	ptree_item_t* expr_ptr = NULL;
	
	if(expr_recursive_parser(&expr_ptr, ta_ptr, expr_end_type)){
		ptree_dtor(expr_ptr);
		return NULL;
	}
	
	return expr_ptr;
}