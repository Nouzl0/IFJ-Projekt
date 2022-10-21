#include "parser.h"
/*

	Rozdelit parser a statement_parser na dva soubory

*/


int get_stmt_end_index(token_array_t tok_arr, int start_index, token_type ending){
	int index = start_index;
	int offset = 0;
	int parens = 0;
	while (tok_arr.len > index){
		token_t tok = tok_arr.elems[index];
		
		//printf("Start index: %s\n",token_debug_get_string(tok.type));
		
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

binary_tree_t* parse_statement(token_array_t tok_arr, int start_index, int end_index){
	
	binary_tree_t* btree_ptr = binary_tree_init();
	
	int terminal = 0;
	//int non_terminal = 0;
	for (int i = start_index; i < end_index; i++){
		token_t tok = tok_arr.elems[i];
		
		//Vyreseni zavorek
		if (tok.type == LEFT_PAREN){
			
			i++; //Preskoceni LEFT_PAREN (
			int rec_end_index = get_stmt_end_index(tok_arr, i, RIGHT_PAREN);
			
			if (!rec_end_index){
				//Syntax error pri hledani konce vyrazu v zavorkach
				return NULL;
			}
			
			binary_tree_t* stmt_tree_ptr = parse_statement(tok_arr, i, i+rec_end_index);
			stmt_tree_ptr->root->precedence = -1;
			binary_tree_extend(btree_ptr, -1, stmt_tree_ptr->root);

			i+= rec_end_index;
			terminal = 1;
			continue;
		}
		
		
		//Terminal
		if (tok.type <= NIL){
			//Bacha moze to byt funkce
			terminal = 1;
			//non_terminal = 0;
			binary_tree_add_leaf(btree_ptr, tok.content->content);
			continue;
		}
		
		//Unary Non-Terminal
		if (tok.type == NEG && !terminal){
			binary_tree_fork_prec(btree_ptr, 1, 0, token_debug_get_string(tok.type));
			continue;
		}
		
		//Binary-Unary Non-Terminal
		if (tok.type == PLUS || tok.type == MINUS){
			if (terminal){
				//Binarni operace
				//non_terminal = 1;
				terminal = 0;
				binary_tree_fork(btree_ptr, tok.type, token_debug_get_string(tok.type));
			} else {
				//Unarni operace
				//Asi pridat nulu
				//non_terminal = 1;
				//binary_tree_add_leaf(btree_ptr,"in:0");
				
				binary_tree_fork_prec(btree_ptr, 1, 0, token_debug_get_string(tok.type));
				
				//binary_tree_fork(btree_ptr, NEG, token_debug_get_string(tok.type));
			}
			
			continue;
		}
		
		//Binary Non-Terminal
		if (terminal){
			//non_terminal = 1;
			terminal = 0;
			binary_tree_fork(btree_ptr, tok.type, token_debug_get_string(tok.type));
			continue;
		}
		
		//Syntax error jelikoz nemohlo byt uplatneno ani jedno pravidlo
		return NULL;

	}
	
	if(!terminal){
		//Syntax error protoze vyraz nekonci terminalem
		return NULL;
	}
	
	return btree_ptr;
	
}

void parse_token_array(error_handler_t* eh_ptr, token_array_t tok_arr){
	int index = 1; // 0 by mel vzdy byt header
	while(index < tok_arr.len){
		token_t tok = tok_arr.elems[index];
		
		//Funkce
		if(tok.type == FUNC){
			
		}
		
		//Deklarace nebo deklarace s inicializaci
			//Poznam podle toho ze za next 
		if(tok.type >= INT && tok.type <= STRING){
			
		}
		
		
		if((tok.type >= IDENTIFIER && tok.type <= PLUS) || tok.type == LEFT_PAREN){
			
			if (tok.type == VARIABLE && tok_arr.elems[index + 1].type == ASSIGN){
				//Vyraz se bude ukladat do promene
			} else {
				
				int offset = get_stmt_end_index(tok_arr,index,SEMICOLON);
				//TODO: kdyz je offset 0 tak syntax errori
				
				binary_tree_t* stmt_tree_ptr = parse_statement(tok_arr,index,index + offset);
				
				if(stmt_tree_ptr == NULL){
					register_syntax_error(eh_ptr,tok_arr.elems[index].line);
					return;
				} else {
					binary_tree_to_json(stmt_tree_ptr);
				
					//Pridat binarni strom do AST
				}
				
				index += offset;
				
				//printf("Skipping to index: %d by: %d\n",index,offset);
			}
			
		}
		
		
		if(tok.type == RETURN){
			//Klicove solov return
		}
		
		if(tok.type == IF){
			//Klicove solov if
		}
		
		if(tok.type == ELSE){
			//Klicove solov else
		}
		
		
		if(tok.type == WHILE){
			//Klicove solov while
		}
		
		//Syntax error neocekavany token
		
		index++;
	
	}
}