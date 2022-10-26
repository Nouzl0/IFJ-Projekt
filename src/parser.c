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
	if(tok_arr.elems[0].type != HEADER){
		register_syntax_error(eh_ptr,tok_arr.elems[0].line,tok_arr.elems[0].column);
		return;
	}
	
	
	int index = 1; // 0 by mel vzdy byt header
	while(index < tok_arr.len){
		token_t tok = tok_arr.elems[index];
		
		//Deklarace funkce
		if(tok.type == FUNC){
			
		}
		
		//Deklarace nebo deklarace s inicializaci
		//Poznam podle toho ze za next je bud vyraz nebo semicolon
		if(tok.type >= INT && tok.type <= STRING){
			
		}
		
		//Ulozeni vyrazu do promene nebo samotny vyraz
		if((tok.type >= IDENTIFIER && tok.type <= PLUS) || tok.type == LEFT_PAREN){
			
			//Ulozeni vyrazu do promene
			if (tok.type == VARIABLE && tok_arr.elems[index + 1].type == ASSIGN){
				//dodelat
			} else {
				
				int offset = get_stmt_end_index(eh_ptr,tok_arr,index,SEMICOLON,0);
				if (!offset || eh_ptr->syntax){
					//Vyraz je prazdny nebo je jeho zadani neplatne
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
				
				ptree_dtor(stmt_tree_ptr);
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