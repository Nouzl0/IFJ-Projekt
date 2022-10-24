#include "statement_parser.h"
/*
STATEMENT:
----------------------------------------------
T -> TEXT                     // "hello world" 
T -> NUMBER                   // 10
T -> NIL                      // null
T -> IDENTIFIER               // print($x,$y)
T -> VARIABLE                 // $a
T -> LEFT_PAREN T RIGHT_PAREN // (T)
T -> NEG T                    // !T
T -> MINUS T                  // -T
T -> T STAR T                 // T * T
T -> T SLASH T                // T / T
T -> T PLUS T                 // T + T
T -> T MINUS T                // T - T
T -> T EQUAL T                // T == T
T -> T GREATER_EQUAL T        // T >= T
T -> T LESS_EQUAL T           // T <= T
T -> T NOT_EQUAL T            // T != T
T -> T GREATER T              // T > T
T -> T LESS T                 // T < T
T -> T AND T                  // T and T
T -> T OR T                   // T or T
T -> T DOT T                  // T . T
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
int get_stmt_end_index(token_array_t tok_arr, int start_index, token_type ending){
	int index = start_index;
	int offset = 0;
	int parens = 0;
	while (tok_arr.len > index){
		token_t tok = tok_arr.elems[index];
		
		//Nalezeni znaku na kterem ma vyraz koncit
		if(tok.type == ending && !parens){
			return offset;
		}
		
		if(tok.type == LEFT_PAREN){
			index++;
			offset++;
			parens++;
			continue;
		}
		
		if(tok.type == RIGHT_PAREN){
			index++;
			offset++;
			parens--;
			continue;
		}
		
		//Povolene token ve vyrazu
		if(tok.type >= IDENTIFIER && tok.type <= OR){
			index++;
			offset++;
			continue;
		}
		
		//Token nepatri do vyrazu
		return 0;
		
	}
	//Kdyby vyraz obsahoval uplne posledni token
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
ptree_item_t* parse_statement(token_array_t tok_arr, int start_index, int end_index){
	
	ptree_t btree;
	ptree_t* btree_ptr = &btree;
	ptree_ctor(btree_ptr);
	
	int terminal = 0;
	//int non_terminal = 0;
	for (int index = start_index; index < end_index; index++){
		token_t tok = tok_arr.elems[index];
		
		//Zavorky
		if (tok.type == LEFT_PAREN){
			
			index++; //Preskoceni LEFT_PAREN (
			int rec_end_index = get_stmt_end_index(tok_arr, index, RIGHT_PAREN);
			
			if (!rec_end_index){
				//Syntax error pri hledani zavorek a povolenych znaku
				return NULL;
			}
			
			ptree_item_t* stmt_tree_ptr = parse_statement(tok_arr, index, index+rec_end_index);
			//ptree_debug_to_json(stmt_tree_ptr);
			
			stmt_tree_ptr->precedence = -1;
			ptree_extend(btree_ptr, -1, stmt_tree_ptr);

			index+= rec_end_index;
			terminal = 1;
			continue;
		}
		
		
		//Terminal
		if (tok.type <= NIL){
			//Bacha moze to byt funkce
			terminal = 1;
			//non_terminal = 0;
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
				//non_terminal = 1;
				terminal = 0;
				ptree_add_branch(btree_ptr, tok.type, token_debug_get_string(tok.type));
			} else {
				//Unarni operace
				
				ptree_add_branch_prec(btree_ptr, 0, 1, token_debug_get_string(tok.type));
				
				//ptree_add_branch(btree_ptr, NEG, token_debug_get_string(tok.type));
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
		return NULL;

	}
	
	if(!terminal){
		//Syntax error protoze vyraz nekonci terminalem
		return NULL;
	}
	
	return btree_ptr->root;
	
}