#include "parser.h"
/*
	TODO:
	
	Kazde pravidlo by melo mit definovanou svoji funkci

	Strom by se mel skladat z itemu ktere muzou mit nekonecne mnoho deti
	item bude reprezentovat pravidlo a jeho deti statement a nebo pole statementu
	nebo dalsi item
	
	if item bude ukazovat na pole podminek a bloku kodu pro reprezentaci else if 

	pridat check next protoze se muze jednat o posledni token tudiz
	neplatny index

	pridat parsovani declare(strict_types=1);

*/

/*

PRAVIDLA PRO PARSER:
------------------------------------------------
// Definice funkce by nemela byt parsovana v rekurzivnim volani.
// function funkce(string $x, ...): string { }
S -> FUNC, IDENTIFIER, LEFT_PAREN, {[STRING|INT|FLOAT], VARIABLE}, COMMA, {...}, RIGHT_PAREN, DDOT, [STRING|INT|FLOAT], LEFT_BRACE, {PARSER}, RIGHT_BRACE


// $a = 10 + 3 * 8;
S -> VARIABLE, ASSIGN, {STATEMENT}, SEMICOLON


// $a + $b;
// funkce(32,64);
// 20 * 10;
// "jo" . "ne";
S -> [VARIABLE|IDENTIFIER|NUMBER|TEXT|NIL|NEG|MINUS|PLUS] -> {STATEMENT}, SEMICOLON


// return $x;
S -> RETURN, {STATEMENT}, SEMICOLON


// if (x > 3){ }
S -> IF, LEFT_PAREN, {STATEMENT}, RIGHT_PAREN, LEFT_BRACE, {PARSER}, RIGHT_BRACE


// else { }
S -> ELSE, LEFT_BRACE, {PARSER}, RIGHT_BRACE


// while(x > 3) { }
S -> WHILE, LEFT_PAREN, {STATEMENT}, RIGHT_PAREN, LEFT_BRACE, {PARSER}, RIGHT_BRACE

*/

/*


function concat(string $x, string $y): string {
		$x = $x . $y;
		return $x . " " . $y;
	}
*/

int get_braces_end_index(token_array_t tok_arr,int index){
	int offset = 0;
	int braces = 0;
	while(1){
		token_t tok = tok_arr.elems[index+offset];
		
		if(tok.type == RIGHT_BRACE){
			if(braces){
				braces--;
			} else {
				return offset;
			}
		} else if(tok.type == LEFT_BRACE){
			braces++;
		}
		
		offset++;
		
	}
}


void token_array_parser(stree_item_t* st_root, error_handler_t* eh_ptr, token_array_t tok_arr, int start, int end){
	
	int index = start;
	while(index < end){
		token_t tok = tok_arr.elems[index];
		
		//Deklarace funkce
		if(tok.type == FUNC && tok_arr.elems[index + 1].type == IDENTIFIER && tok_arr.elems[index + 2].type == LEFT_PAREN){
			index += 3;
			
			tok = tok_arr.elems[index];
			
			stree_item_t* function_item = stree_new_item(FUNCBLOCK,2);
			stree_item_t* params_item = stree_new_item(FUNCPARAMS,5);
			stree_insert_to_block(function_item,params_item);
			
			stree_insert_to_block(st_root,function_item);
		
			while(1){
				if(tok.type >= INT && tok.type <= STRING){
					
					if(tok_arr.elems[index + 1].type == VARIABLE){
						
						stree_item_t* type_item = stree_new_item(PARAMTYPE,1);
						stree_insert_to_block(params_item,type_item);
						stree_item_t* param_item = stree_new_item(PARAM,0);
						stree_insert_to_block(type_item,param_item);
						type_item->token = &tok_arr.elems[index];
						param_item->token = &tok_arr.elems[index + 1];
						index+= 2;
						tok = tok_arr.elems[index];
					}
					
				}
				
				
				if(tok.type == COMMA){
					index++;
					tok = tok_arr.elems[index];
					continue;
				}
				
				if(tok.type == RIGHT_PAREN){
					index++;
					break;
				}
				
				//printf("konec: %s\n",token_debug_get_string(tok.type));
				
				register_syntax_error(eh_ptr,tok.line,tok.column);
				return;
				
			}
			
			
			tok = tok_arr.elems[index];
			if(tok.type != DDOT){
				register_syntax_error(eh_ptr,tok.line,tok.column);
				return;
			}
			index++;
			tok = tok_arr.elems[index];
			
			
			if(tok.type >= VOID && tok.type <= STRING){
				function_item->token = &tok_arr.elems[index];
			} else {
				register_syntax_error(eh_ptr,tok.line,tok.column);
				return;
			}
			
			index += 2;
			
			int offset = get_braces_end_index(tok_arr,index);
			
			//printf("offset: %d\n",get_braces_end_index(tok_arr,index));
			
			stree_item_t* function_body = stree_new_block(1);
			
			token_array_parser(function_body,eh_ptr,tok_arr,index,index+offset);
			
			stree_insert_to_block(function_item,function_body);
			
			index+= offset;
			
			
		}
		
		//Ulozeni vyrazu do promene nebo samotny vyraz
		if((tok.type >= IDENTIFIER && tok.type <= PLUS) || tok.type == LEFT_PAREN){
			
			//Ulozeni vyrazu do promene
			if (tok.type == VARIABLE && tok_arr.elems[index + 1].type == ASSIGN){
				
				int offset = get_stmt_end_index(eh_ptr,tok_arr,index+2,SEMICOLON,0);
				
				if (!offset || eh_ptr->syntax){
					//Vyraz je prazdny nebo je jeho zadani neplatne
					register_syntax_error(eh_ptr,tok.line,tok.column);
					return;
				}
				
				stree_item_t* assign_item = stree_new_item(ASSIGNSTMT,0);
				assign_item->token = &tok_arr.elems[index];
				
				ptree_item_t* stmt_tree_ptr = parse_statement(eh_ptr,tok_arr,index,index + offset);
				
				if(stmt_tree_ptr == NULL){
					//Syntakticka chyba ve vyrazu
					return;
				} else {
					assign_item->stmt = stmt_tree_ptr;
				}
				
				stree_insert_to_block(st_root,assign_item);
				
				index = index + offset + 2;
				
				
				
			//Samotny vyraz
			} else {
				
				int offset = get_stmt_end_index(eh_ptr,tok_arr,index,SEMICOLON,0);
				if (!offset || eh_ptr->syntax){
					//Vyraz je prazdny nebo je jeho zadani neplatne
					return;
				}
				
				ptree_item_t* stmt_tree_ptr = parse_statement(eh_ptr,tok_arr,index,index + offset);
				
				if(stmt_tree_ptr == NULL){
					return;
				} else {
					//ptree_debug_to_json(stmt_tree_ptr);
					stree_insert_stmt(st_root, stmt_tree_ptr);
				}
				
				//ptree_dtor(stmt_tree_ptr);
				index += offset;
			}
			
		}
		
		
		if(tok.type == RETURN){
			index++;
			tok = tok_arr.elems[index];
			
			int offset = get_stmt_end_index(eh_ptr,tok_arr,index,SEMICOLON,0);
			if (!offset || eh_ptr->syntax){
				//Vyraz je prazdny nebo je jeho zadani neplatne
				return;
			}
			
			ptree_item_t* stmt_tree_ptr = parse_statement(eh_ptr,tok_arr,index,index + offset);
			
			if(stmt_tree_ptr == NULL){
				return;
			} else {
				//ptree_debug_to_json(stmt_tree_ptr);
				stree_item_t* return_item = stree_new_item(RETSTMT,0);
				return_item->stmt = stmt_tree_ptr;
				stree_insert_to_block(st_root, return_item);
			}
			
			index += offset;
		}
		
		if(tok.type == IF){
			//Klicove slovo if
		}
		/*
		if(tok.type == ELSE){
			//Klicove slovo else
		}
		*/
		
		if(tok.type == WHILE){
			//Klicove slovo while
		}
		
		//Syntax error neocekavany token
		
		index++;
	
	}
	
}



/**
 * Token po tokenu prochazi pole a vytvari abstraktni syntakticky strom
 * 
 * @param tok_arr Pole tokenu
 * @param start_index Index v poli tokenu kde ma vyraz zancinat
 * @param end_index Index v poli tokenu na kterem ma vyraz koncit
 * @returns NULL kdyz je vyraz neplatny jinak ukazatel na korenovy prvek
 * stromu precedence
 */
void parse_token_array(error_handler_t* eh_ptr, token_array_t tok_arr){
	stree_item_t* st_root = stree_new_block(0);
	/*
	if(tok_arr.elems[0].type != HEADER){
		register_syntax_error(eh_ptr,tok_arr.elems[0].line,tok_arr.elems[0].column);
		return;
	}
	*/
	
	//index 0 by mel vzdy byt header
	token_array_parser(st_root,eh_ptr,tok_arr,1,tok_arr.len-1);
	stree_json_debug_print(st_root);
}
