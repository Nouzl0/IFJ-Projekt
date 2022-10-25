#include "parser.h"
/*
	TODO:
	Rozdelit parser a statement_parser na dva soubory

	Prvni token musi byt header 

*/

/*

PRAVIDLA PRO PARSER:
------------------------------------------------
// Definice funkce by nemela byt parsovana v rekurzivnim volani.
// function funkce(string $x, ...): string { }
S -> FUNC, IDENTIFIER, LEFT_PAREN, {[STRING|INT|FLOAT], VARIABLE}, COMMA, {...}, RIGHT_PAREN, DDOT, [STRING|INT|FLOAT], LEFT_BRACE, {PARSER}, RIGHT_BRACE


// string $a = 10 + 3 * 8;
S -> [STRING|INT|FLOAT], VARIABLE, ASSIGN, {STATAEMENT}, SEMICOLON 


// string $a;
S -> [STRING|INT|FLOAT], VARIABLE, SEMICOLON 


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
				
				int offset = get_stmt_end_index(eh_ptr,tok_arr,index,SEMICOLON);
				if (!offset){
					//Nebyl nalezen ani jeden prvek do vyrazu
					return;
				}
				
				ptree_item_t* stmt_tree_ptr = parse_statement(eh_ptr,tok_arr,index,index + offset);
				
				if(stmt_tree_ptr == NULL){
					//Syntakticka chyba ve vyrazu
					return;
				} else {
					ptree_debug_to_json(stmt_tree_ptr);
					//Pridat binarni strom do AST
				}
				
				index += offset;
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