#include "statement_parser.h"
/*
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
 * Vraci index konce vyrazu v poli tokenu podle
 * tokenu ktery ma vyraz ukoncit
 *
 * @param tok_arr Pole tokenu
 * @param start_index Index v poli tokenu kde ma vyraz zancinat
 * @param ending Typ tokenu kterym ma vyraz koncit
 * @returns 0 kdyz vyraz neni platny nebo index konce vyrazu
 */
int get_stmt_end_index(error_handler_t* eh_ptr, token_array_t tok_arr, int start_index, token_type ending){
	int index = start_index;
	int offset = 0;
	int parens = 0;
	while (tok_arr.len > index){
		token_t tok = tok_arr.elems[index+offset];
		
		//Detekuje ukonceni vyrazu
		if(tok.type == ending && !parens){
			return offset;
		
		//Detekuje levou zavorku
		} else if (tok.type == LEFT_PAREN){
			parens++;
		//Detekuje pravou zavorku ktera neukoncuje vyraz ale znaci precedenci ve vyrazu
		} else if (tok.type == RIGHT_PAREN){
			parens--;
		//Detekuje token ktery ve vyrazu neni povoleny
		} else if (tok.type < IDENTIFIER || tok.type > OR){
			//Token nepatri do vyrazu
			register_syntax_error(eh_ptr,tok_arr.elems[index].line,tok_arr.elems[index].column);
			return 0;
		}
		
		offset++;
		
	}
	
	//Kdyby vyraz obsahoval uplne posledni token coz znamena ze neni ukoncen strednikem
	return 0;
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
ptree_item_t* parse_statement(error_handler_t* eh_ptr, token_array_t tok_arr, int start_index, int end_index){
	
	ptree_t btree;
	ptree_t* btree_ptr = &btree;
	ptree_ctor(btree_ptr);
	
	int terminal = 0;
	int index = start_index;
	for (; index < end_index; index++){
		token_t tok = tok_arr.elems[index];
		
		//Zavorky
		if (tok.type == LEFT_PAREN){
			
			index++; //Preskoceni LEFT_PAREN (
			int rec_end_index = get_stmt_end_index(eh_ptr, tok_arr, index, RIGHT_PAREN);
			
			if (!rec_end_index){
				//Syntax error pri hledani zavorek a povolenych znaku
				return NULL;
			}
			
			//Rekurzivni volani na vytvoreni stromu pro vyraz v zavorkach
			ptree_item_t* stmt_tree_ptr = parse_statement(eh_ptr, tok_arr, index, index+rec_end_index);
			
			if (stmt_tree_ptr == NULL){
				//Syntax error pri vytvareni stromu
				return NULL;
			}
			
			//Pripojuje strom zavorek do stavajiciho vyrazu
			stmt_tree_ptr->precedence = -1;
			ptree_extend(btree_ptr, -1, stmt_tree_ptr);
			
			//Posunuje se o tolik jak je dlouhy vyraz v zavorkach vcetne RIGHT_PAREN )
			index+= rec_end_index;
			terminal = 1;
			continue;
		}
		
		
		//Terminal
		if (tok.type <= NIL){
			//Bacha moze to byt funkce
			terminal = 1;
			prec_tree_add_leaf(btree_ptr, tok.content->content);
			continue;
		}
		
		//Unary Non-Terminal
		if (tok.type == NEG && !terminal){
			ptree_add_branch_prec(btree_ptr, 1, 0, token_debug_get_string(tok.type));
			continue;
		}
		
		//Binary-Unary Non-Terminal
		if (tok.type == PLUS || tok.type == MINUS){
			if (terminal){
				//Binarni operace
				terminal = 0;
				ptree_add_branch(btree_ptr, tok.type, token_debug_get_string(tok.type));
			} else {
				//Unarni operace
				ptree_add_branch_prec(btree_ptr, 0, 1, token_debug_get_string(tok.type));
			}
			
			continue;
		}
		
		//Binary Non-Terminal
		if (terminal){
			//non_terminal = 1;
			terminal = 0;
			ptree_add_branch(btree_ptr, tok.type, token_debug_get_string(tok.type));
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