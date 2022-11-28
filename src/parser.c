#include "parser.h"
/*
	TODO:
	
	cele prepsat !
	
	Kazde pravidlo by melo mit definovanou svoji funkci


	pridat check next protoze se muze jednat o posledni token tudiz
	neplatny index


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


int get_braces_end_index(tok_arr_t tok_arr,int index){
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


void recursive_parser(stree_item_t* st_root, error_handler_t* eh_ptr, tok_arr_t tok_arr, int start, int end){
	
	int index = start;
	while(index < end){
		token_t tok = tok_arr.elems[index];
		
		//Deklarace funkce
		if(tok.type == FUNC && tok_arr.elems[index + 1].type == IDENTIFIER && tok_arr.elems[index + 2].type == LEFT_PAREN){
			stree_item_t* function_item = stree_new_item(FUNCBLOCK,3);
			stree_item_t* name_item = stree_new_item(FUNCNAME,0);
			name_item->token = &tok_arr.elems[index + 1];
			
			stree_insert_to_block(function_item,name_item);
			stree_item_t* params_item = stree_new_item(FUNCPARAMS,5);
			stree_insert_to_block(function_item,params_item);
			
			stree_insert_to_block(st_root,function_item);
		
			index += 3;
			tok = tok_arr.elems[index];
		
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
				
				//printf("konec: %s\n",token_enum_to_string(tok.type));
				
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
			
			recursive_parser(function_body,eh_ptr,tok_arr,index,index+offset);
			
			stree_insert_to_block(function_item,function_body);
			
			index+= offset;
			
			
		}
		
		//Ulozeni vyrazu do promene nebo samotny vyraz
		if((tok.type >= IDENTIFIER && tok.type <= PLUS) || tok.type == LEFT_PAREN){
			
			//Ulozeni vyrazu do promene
			if (tok.type == VARIABLE && tok_arr.elems[index + 1].type == ASSIGN){
				
				index += 2;
				//printf("debug: %s\n",token_enum_to_string(tpl.type));
				int offset = get_stmt_end_index(eh_ptr,tok_arr,index,SEMICOLON,0);
				
				if (!offset || eh_ptr->error){
					//Vyraz je prazdny nebo je jeho zadani neplatne
					register_syntax_error(eh_ptr,tok.line,tok.column);
					return;
				}
				
				stree_item_t* assign_item = stree_new_item(ASSIGNSTMT,0);
				assign_item->token = &tok_arr.elems[index-2];
				
				ptree_item_t* stmt_tree_ptr = parse_statement(eh_ptr,tok_arr,index,index + offset);
				
				//ptree_debug_to_json(stmt_tree_ptr);
				
				
				
				if(stmt_tree_ptr == NULL){
					//Syntakticka chyba ve vyrazu
					return;
				} else {
					assign_item->stmt = stmt_tree_ptr;
				}
				
				stree_insert_to_block(st_root,assign_item);
				
				index += offset;
				
			//Samotny vyraz
			} else {
				
				int offset = get_stmt_end_index(eh_ptr,tok_arr,index,SEMICOLON,0);
				if (!offset || eh_ptr->error){
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
		
		//Klicove solovo RETURN
		if(tok.type == RETURN){
			index++;
			tok = tok_arr.elems[index];
			
			int offset = get_stmt_end_index(eh_ptr,tok_arr,index,SEMICOLON,0);
			if (!offset || eh_ptr->error){
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
		
		//Klicove slovo IF
		if(tok.type == IF && tok_arr.elems[index + 1].type == LEFT_PAREN){
			index += 2;
			int offset = get_stmt_end_index(eh_ptr,tok_arr,index,RIGHT_PAREN,0);
			if (!offset || eh_ptr->error){
				//Vyraz je prazdny nebo je jeho zadani neplatne
				return;
			}
			
			stree_item_t* ifelse_item = stree_new_item(IFELSE,2);
			stree_insert_to_block(st_root, ifelse_item);
			
			
			ptree_item_t* stmt_tree_ptr = parse_statement(eh_ptr,tok_arr,index,index + offset);
			
			if(stmt_tree_ptr == NULL){
				//Strom neexistuje
				return;
			}
			
			index += offset;
			index++;
			
			ifelse_item->stmt = stmt_tree_ptr;
			
			tok = tok_arr.elems[index];
			
			if(tok.type != LEFT_BRACE){
				//Chybi oteviraci slozena zavorka
				return;
			}
			
			index++;
			
			offset = get_braces_end_index(tok_arr,index);
			
			stree_item_t* if_body = stree_new_block(st_root->level+1);
			
			recursive_parser(if_body,eh_ptr,tok_arr,index,index+offset);
			
			stree_insert_to_block(ifelse_item,if_body);
			
			index+= offset;
			
			tok = tok_arr.elems[index];
			if (tok.type != RIGHT_BRACE){
				//Chybi uzaviraci slozena zavorka
				return;
			}
			
			
			
			if (tok_arr.elems[index+1].type == ELSE){
				
				if(tok_arr.elems[index+2].type != LEFT_BRACE){
					//Chybi oteviraci slozena zavorka
					return;
				}
				
				
				index+= 3;
				
				offset = get_braces_end_index(tok_arr,index);
				stree_item_t* else_body = stree_new_block(st_root->level+1);
				recursive_parser(else_body,eh_ptr,tok_arr,index,index+offset);
				stree_insert_to_block(ifelse_item,else_body);
				index += offset;
			}
			
			
		}
		
		//Klicove slovo while
		if(tok.type == WHILE && tok_arr.elems[index + 1].type == LEFT_PAREN){
			
			index += 2;
			int offset = get_stmt_end_index(eh_ptr,tok_arr,index,RIGHT_PAREN,0);
			if (!offset || eh_ptr->error){
				//Vyraz je prazdny nebo je jeho zadani neplatne
				return;
			}

			stree_item_t* while_block = stree_new_item(WHILEBLOCK,5);
			stree_insert_to_block(st_root, while_block);
			
			ptree_item_t* stmt_tree_ptr = parse_statement(eh_ptr,tok_arr,index,index + offset);
			
			if(stmt_tree_ptr == NULL){
				//Strom neexistuje
				return;
			}
			
			index += offset;
			
			while_block->stmt = stmt_tree_ptr;
			
			//printf("\n Breakpoint: %s \n",token_enum_to_string(tok_arr.elems[index].type));
			index++;
			if(tok_arr.elems[index].type != LEFT_BRACE){
				//Chybi oteviraci slozena zavorka
				return;
			}
		
			index++;
		
		
			offset = get_braces_end_index(tok_arr,index);
			while_block->level = st_root->level+1;
			
			recursive_parser(while_block,eh_ptr,tok_arr,index,index+offset);
			index+= offset;
			
			index++;
		
			if (tok_arr.elems[index].type != RIGHT_BRACE){
				//Chybi uzaviraci slozena zavorka
				return;
			}
		
		
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
stree_item_t* parse_token_array(error_handler_t* eh_ptr, tok_arr_t tok_arr){
	stree_item_t* st_root = stree_new_block(0);
	if(!(
		tok_arr.elems[0].type == HEADER &&
		tok_arr.elems[1].type == IDENTIFIER &&
		tok_arr.elems[2].type == LEFT_PAREN &&
		tok_arr.elems[3].type == IDENTIFIER &&
		tok_arr.elems[4].type == ASSIGN &&
		tok_arr.elems[5].type == NUMBER &&
		tok_arr.elems[6].type == RIGHT_PAREN &&
		tok_arr.elems[7].type == SEMICOLON
	)){
		register_syntax_error(eh_ptr,tok_arr.elems[0].line,tok_arr.elems[0].column);
		return NULL;
	}
	
	recursive_parser(st_root,eh_ptr,tok_arr,7,tok_arr.len-1);
	return st_root;
}
/*

if(tok.type == IF && tok_arr.elems[index + 1].type == LEFT_PAREN){
			index += 2;
			int offset = get_stmt_end_index(eh_ptr,tok_arr,index,RIGHT_PAREN,0);
			if (!offset || eh_ptr->error){
				//Vyraz je prazdny nebo je jeho zadani neplatne
				return;
			}
			
			stree_item_t* ifelse_item = stree_new_item(IFELSE,2);
			stree_insert_to_block(st_root, ifelse_item);
			
			
			ptree_item_t* stmt_tree_ptr = parse_statement(eh_ptr,tok_arr,index,index + offset);
			
			if(stmt_tree_ptr == NULL){
				//Strom neexistuje
				return;
			}
			
			index += offset;
			index++;
			
			ifelse_item->stmt = stmt_tree_ptr;
			
			tok = tok_arr.elems[index];
			
			if(tok.type != LEFT_BRACE){
				//Chybi oteviraci slozena zavorka
				return;
			}
			
			index++;
			
			offset = get_braces_end_index(tok_arr,index);
			
			stree_item_t* if_body = stree_new_block(st_root->level+1);
			
			recursive_parser(if_body,eh_ptr,tok_arr,index,index+offset);
			
			stree_insert_to_block(ifelse_item,if_body);
			
			index+= offset;
			
			tok = tok_arr.elems[index];
			if (tok.type != RIGHT_BRACE){
				//Chybi uzaviraci slozena zavorka
				return;
			}
			
			
			
			if (tok_arr.elems[index+1].type == ELSE){
				
				if(tok_arr.elems[index+2].type != LEFT_BRACE){
					//Chybi oteviraci slozena zavorka
					return;
				}
				
				
				index+= 3;
				
				offset = get_braces_end_index(tok_arr,index);
				stree_item_t* else_body = stree_new_block(st_root->level+1);
				recursive_parser(else_body,eh_ptr,tok_arr,index,index+offset);
				stree_insert_to_block(ifelse_item,else_body);
				index += offset;
			}
			
			
		}


//Klicove solovo RETURN
		if(tok.type == RETURN){
			index++;
			tok = tok_arr.elems[index];
			
			int offset = get_stmt_end_index(eh_ptr,tok_arr,index,SEMICOLON,0);
			if (!offset || eh_ptr->error){
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


*/


//NEW

//Label
void parser_build_block(stree_item_t* parent_node, tok_arr_t* ta_ptr, bool brace_needed);


void parser_build_function(stree_item_t* parent_node, tok_arr_t* ta_ptr){
	// Creating parent function node and adding to tree
	stree_item_t* func_node = stree_new_item(FUNCBLOCK,3);
	stree_insert_to_block(parent_node,func_node);
	
	// Creating child node for function name
	stree_item_t* name_node = stree_new_item(FUNCNAME,0);
	stree_insert_to_block(func_node,name_node);
	name_node->token = tok_arr_get_offset(ta_ptr,-2);
	
	// Creating parent node for function parameters nodes
	stree_item_t* params_node = stree_new_item(FUNCPARAMS,5);
	stree_insert_to_block(func_node,params_node);
	
	// Creating parent node for function body code block
	stree_item_t* body_node = stree_new_block(1);
	stree_insert_to_block(func_node,body_node);
	
	int separator = 1;
	// Loops until all parameters are parsed
	while(true){
		// Parameter type and parameter name
		if(separator && tok_arr_cmp_range(ta_ptr, INT, STRING) && tok_arr_cmp_offset(ta_ptr,VARIABLE,1)){
			
			// Creating parent node that stores type
			stree_item_t* type_node = stree_new_item(PARAMTYPE,1);
			stree_insert_to_block(params_node,type_node);
			type_node->token = tok_arr_get(ta_ptr);
			
			// Creating child node that stores parameter name
			stree_item_t* param_node = stree_new_item(PARAM,0);
			stree_insert_to_block(type_node,param_node);
			param_node->token = tok_arr_get_offset(ta_ptr,1);
			
			tok_arr_inc(ta_ptr,2);
			separator = 0;

		// Comma as parameter separator
		} else if (!separator && tok_arr_cmp(ta_ptr,COMMA)) {
			separator = 2;
			tok_arr_inc(ta_ptr,1);
			
		// Right parenthesis as parameters terminating token
		} else if (separator < 2 && tok_arr_cmp(ta_ptr,RIGHT_PAREN)){
			tok_arr_inc(ta_ptr,1);
			break;
			
		// Other invalid tokens
		} else {
			syntax_error(*tok_arr_get(ta_ptr), "Invalid format of function parameters");
			return;
		}
	}
	
	//Checks right format of footer of function definition
	if(tok_arr_cmp_skip(ta_ptr,DDOT) && tok_arr_cmp_range(ta_ptr,NIL_INT,STRING) && tok_arr_cmp_offset(ta_ptr,LEFT_BRACE,1)){
		func_node->token = tok_arr_get(ta_ptr);
		tok_arr_inc(ta_ptr,2);
	} else {
		syntax_error(*tok_arr_get(ta_ptr), "Invalid format of function return type");
		return;
	}
		
	parser_build_block(body_node,ta_ptr,1);
	
	for (int i = -2; i < 3; i++){
		printf("Debug %d: %s \n",i, token_enum_to_string(tok_arr_get_offset(ta_ptr,i)->type));
	}
	
	
}


void parser_build_expr(stree_item_t* parent_node, tok_arr_t* ta_ptr){
	
	ptree_item_t* expr_ptr = NULL;
	
	if(tok_arr_cmp(ta_ptr,VARIABLE) && tok_arr_cmp_offset(ta_ptr,ASSIGN,1)){
		// Creating parent node that holds name of variable assigning to
		stree_item_t* assign_node = stree_new_item(ASSIGNSTMT,0);
		stree_insert_to_block(parent_node,assign_node);
		assign_node->token = tok_arr_get(ta_ptr);
		tok_arr_inc(ta_ptr,2);
		
		expr_ptr = expr_parse(ta_ptr,SEMICOLON);
		assign_node->stmt = expr_ptr;
		
	} else {
		expr_ptr = expr_parse(ta_ptr,SEMICOLON);
		stree_insert_stmt(parent_node, expr_ptr);
	}
	
	if(global_err_ptr->error){
		return;
	}
	
	if(!expr_ptr){
		syntax_error(*tok_arr_get(ta_ptr), "Valid expression expected");
		return; 
	}
	
}


void parser_build_ifelse(stree_item_t* parent_node, tok_arr_t* ta_ptr){
	ptree_item_t* expr_ptr = expr_parse(ta_ptr,RIGHT_PAREN);
	
	if(global_err_ptr->error){
		return;
	}
	
	if(!expr_ptr){
		syntax_error(*tok_arr_get(ta_ptr), "Valid expression expected");
		return; 
	}
	
	// Creating parent node that stores condition expression, if and else code blocks
	stree_item_t* ifelse_node = stree_new_item(IFELSE,2);
	stree_insert_to_block(parent_node, ifelse_node);
	ifelse_node->stmt = expr_ptr;	
	
	// Checks expression terminating token and if code block starting token
	if (!tok_arr_cmp_skip(ta_ptr,RIGHT_PAREN) || !tok_arr_cmp_skip(ta_ptr,LEFT_BRACE)){
		syntax_error(*tok_arr_get(ta_ptr), "Invalid token, expected ) or {");
		return;
	}
	
	stree_item_t* if_body_node = stree_new_block(parent_node->level+1);
	stree_insert_to_block(ifelse_node,if_body_node);
	
	parser_build_block(if_body_node,ta_ptr,1);
	
	//printf("Debug -1: %s \n", token_enum_to_string(tok_arr_get_offset(ta_ptr,-1)->type));
	//printf("Debug 0: %s \n", token_enum_to_string(tok_arr_get(ta_ptr)->type));
	//printf("Debug 1: %s \n", token_enum_to_string(tok_arr_get_offset(ta_ptr,1)->type));
	
	
	
	// Checks if code block terminating brace }
	if (!tok_arr_cmp_skip(ta_ptr,RIGHT_BRACE)){
		syntax_error(*tok_arr_get(ta_ptr), "Missing } after if code block");
		return;
	}
	
	
	// Checks for else keyword
	if (tok_arr_cmp(ta_ptr,ELSE)){
		
		// Checks else code block starting brace {
		if (!tok_arr_cmp_skip(ta_ptr,RIGHT_BRACE)){
			syntax_error(*tok_arr_get(ta_ptr), "Missing { before else code block");
			return;
		}
		
		stree_item_t* else_body_node = stree_new_block(parent_node->level+1);
		stree_insert_to_block(ifelse_node,else_body_node);
		
		parser_build_block(else_body_node,ta_ptr,1);
		
		// Checks else code block terminating brace }
		if (!tok_arr_cmp_skip(ta_ptr,RIGHT_BRACE)){
			syntax_error(*tok_arr_get(ta_ptr), "Missing } after else code block");
			return;
		}
		
	}
	

}



void parser_build_block(stree_item_t* parent_node, tok_arr_t* ta_ptr, bool brace_needed){
	token_type func_decl_header[] = {
		FUNC,IDENTIFIER,LEFT_PAREN
	};
	
	while(!tok_arr_on_end(ta_ptr)){
	
		if (tok_arr_cmp(ta_ptr,RIGHT_BRACE) && brace_needed){
			break;
		}
		
		//Function declaration parsing
		if(tok_arr_cmp_arr(ta_ptr,func_decl_header,3)){
			tok_arr_inc(ta_ptr,3);
			parser_build_function(parent_node, ta_ptr);
			continue;
		}
		
		//Expression and assigning expression to variable
		if(tok_arr_cmp_range(ta_ptr,IDENTIFIER,NIL) || tok_arr_cmp(ta_ptr,LEFT_PAREN)){
			parser_build_expr(parent_node, ta_ptr);
			continue;
		}
		
		//Return expression
		if(tok_arr_cmp(ta_ptr,RETURN)){
			tok_arr_inc(ta_ptr,1);
			
			ptree_item_t* expr_ptr = expr_parse(ta_ptr,SEMICOLON);
			
			if(global_err_ptr->error){
				return;
			}
			
			stree_item_t* return_node = stree_new_item(RETSTMT,0);
			stree_insert_to_block(parent_node, return_node);
			return_node->stmt = expr_ptr;
			//tok_arr_inc(ta_ptr,1);
		}
		
		//If and else statement
		if(tok_arr_cmp(ta_ptr,IF) && tok_arr_cmp_offset(ta_ptr,LEFT_PAREN,1)){
			tok_arr_inc(ta_ptr,2);
			parser_build_ifelse(parent_node, ta_ptr);
			continue;
		}
		
		tok_arr_inc(ta_ptr,1);
	}
	
	//Checknut kdyz je brace needed musi koncit na brace jinak syntax error
	
}


stree_item_t* parser_build_all(tok_arr_t* ta_ptr){

	token_type header_types[] = {
		HEADER,IDENTIFIER,LEFT_PAREN,IDENTIFIER,ASSIGN,NUMBER,RIGHT_PAREN,SEMICOLON
	};

	if(!tok_arr_cmp_arr(ta_ptr,header_types,8)){
		syntax_error(*tok_arr_get(ta_ptr), "Invalid format of file header");
		return NULL;
	}
	
	// Skips header
	tok_arr_inc(ta_ptr, 8);

	stree_item_t* root_node = stree_new_block(0);
	parser_build_block(root_node, ta_ptr, 0);

	return root_node;

}

//stree_item_t* parser()