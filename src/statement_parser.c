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
int expr_rec_parser(ptree_item_t** root_ptr, tok_arr_t* ta_ptr, token_type* expr_end_types, int types_len){	
	// Empty expression
	if(tok_arr_cmp(ta_ptr,expr_end_types[0])){
		return 0;
	}
	
	ptree_t etree;
	ptree_t* etree_ptr = &etree;
	ptree_ctor(etree_ptr);
	
	
	
	int terminal = 0;
	while (!tok_arr_on_end(ta_ptr) && !tok_arr_cmp_arr(ta_ptr,expr_end_types,types_len)){
		
		if (tok_arr_cmp(ta_ptr,LEFT_PAREN)){
			// Skips left paren
			tok_arr_inc(ta_ptr,1);
			
			printf("Debug: %s\n",token_enum_to_string(tok_arr_get(ta_ptr)->type));
			
			ptree_item_t* expr_ptr = NULL;
			
			token_type paren_end[] = {RIGHT_PAREN};
			
			// Invalid expression in parens 
			if(expr_rec_parser(&expr_ptr, ta_ptr,paren_end,1)){
				ptree_dtor(expr_ptr);
				return 1;
			}
			
			// Non empty parens
			if(expr_ptr){
				expr_ptr->precedence = -1;
				ptree_extend(etree_ptr, -1, expr_ptr);
				terminal = 1;
			}
			
			// Skips right paren
			tok_arr_inc(ta_ptr,1);
		}
		
		// Terminals
		if (tok_arr_cmp_range(ta_ptr,IDENTIFIER,NIL) && !terminal){
			prec_tree_add_leaf(etree_ptr, *tok_arr_get_next(ta_ptr));
			terminal = 1;
			continue;
		}
		
		// Non-Terminal
		if (tok_arr_cmp_range(ta_ptr,MINUS,LESS) && terminal){
			ptree_add_branch(etree_ptr, *tok_arr_get_next(ta_ptr));
			terminal = 0;
			continue;
		}
		
		syntax_error(*tok_arr_get_offset(ta_ptr,-1), "Unexpected token in expression");
		return 1;
		
	}
	
	// Every non empty expression have to end with terminal
	if(etree_ptr->root && !terminal){
		syntax_error(*tok_arr_get_offset(ta_ptr,-1), "Unexpected expression ending");
		return 1;
	}
	
	(*root_ptr) = etree_ptr->root;
	
	return 0;
}


ptree_item_t* expr_parse(tok_arr_t* ta_ptr, token_type expr_end_type){
	
	ptree_item_t* expr_ptr = NULL;
	
	token_type end_types[] = {expr_end_type};
	if(expr_rec_parser(&expr_ptr, ta_ptr, end_types, 1)){
		ptree_dtor(expr_ptr);
		return NULL;
	}
	
	return expr_ptr;
}