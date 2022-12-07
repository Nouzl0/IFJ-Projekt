/**
 * Project: Implementace překladače imperativního jazyka IFJ22
 * 
 * @file tokens_lib.c
 * @brief Tokens array
 * 
 * @author Nikolas Nosál <xnosal01>
 * @author Adam Mrkva <xmrkva04>
 * @author Rostislav Navrátil <xnavra72>
 * @author David Nevrlka <xnevrl00>
 */

#include "tokens_lib.h"

/*
TODO:
	Pridat komentaro pro pomocne funkce pro pruchod pole tokenu
*/

#define KEYWORD_REGISTER_LENGTH 10
static trecord_t keyword_register[KEYWORD_REGISTER_LENGTH] = {
	{"null", NIL},
	{"if", IF},
	{"else", ELSE},
	{"while", WHILE},
	{"function", FUNC},
	{"return", RETURN},
	{"int", INT},
	{"float", FLOAT},
	{"string", STRING},
	{"void", VOID}
};

#define SYMBOL_REGISTER_LENGTH 26
static trecord_t symbol_register[SYMBOL_REGISTER_LENGTH] = {
	{"?int", NIL_INT},
	{"?float", NIL_FLOAT},
	{"?string", NIL_STRING},
	{"<?php", HEADER},
	{"?>", FOOTER},
	{"===", TYPE_EQUAL},
	{"!==", TYPE_NOT_EQUAL},
	{"==", EQUAL},
	{"!=", NOT_EQUAL},
	{">=", GREATER_EQUAL},
	{"<=", LESS_EQUAL},
	{">", GREATER},
	{"<", LESS},
	{"=", ASSIGN},
	{"(", LEFT_PAREN},
	{")", RIGHT_PAREN},
	{"{", LEFT_BRACE},
	{"}", RIGHT_BRACE},
	{",", COMMA},
	{".", DOT},
	{"-", MINUS},
	{"+", PLUS},
	{";", SEMICOLON},
	{"*", STAR},
	{"/", SLASH},
	{":", DDOT}
};


/**
 * Searches given string in token strings stored in register
 * Goes through whole register comparing one by one
 * Uses str_builder_cmp defined in strings_lib.h
 *
 * @param reg Pointer to register which stores strings for search
 * @param reg_len Register length
 * @param str_ptr String that is compared by
 * @param ttype_ptr Pointer to index in register for found string
 * 
 * @returns string length if found otherwise 0
 */
int token_compare(trecord_t* reg, int reg_len, char* str_ptr, int* ttype_ptr){
	for (int i = 0; i < reg_len; i++){
		if (str_builder_cmp(reg[i].match,str_ptr)){
			*ttype_ptr = reg[i].type;
			return strlen(reg[i].match); 
		}
	}
	return 0;	
}


/**
 * Searches if given string is valid keyword
 * 
 * @param str_ptr String that is searched
 * @param ttype_ptr Pointer to index in register for found string
 * 
 * @returns keyword length if found otherwise 0
 */
int token_compare_keywords(char* str_ptr, int* ttype_ptr){
	return token_compare(keyword_register,KEYWORD_REGISTER_LENGTH,str_ptr,ttype_ptr);
}


/**
 * Searches if given string is in symbol register
 * 
 * @param str_ptr String that is searched
 * @param ttype_ptr Pointer to index in register for found string
 * 
 * @returns found symbol length if found otherwise 0
 */
int token_compare_symbol(char* str_ptr, int* ttype_ptr){
	return token_compare(symbol_register,SYMBOL_REGISTER_LENGTH,str_ptr,ttype_ptr);
}


/**
 * Returns string form for corresponding enum of token type
 * 
 * @param type Given type in enum form 
 *
 * @returns string Representation of token type
 */
char* token_enum_to_string(token_type type){
	static char *TOKEN_ENUM_STRINGS[] = {
		// Nullable Data types
		"NIL_INT",
		"NIL_FLOAT",
		"NIL_STRING",
		
		// Data types
		"VOID",
		"INT",
		"FLOAT",
		"STRING",
		
	// Terminals
		"IDENTIFIER",
		"VARIABLE",
		"NUMBER",
		"FRACTION",
		"TEXT",
		"NIL",
		
	// Non-Terminals
		
		// Basic math
		"MINUS",
		"PLUS",
		"STAR",
		"SLASH",
		"DOT",
		
		// Comparators
		"TYPE_EQUAL",
		"TYPE_NOT_EQUAL",
		"EQUAL",
		"NOT_EQUAL",
		"GREATER_EQUAL",
		"LESS_EQUAL",
		"GREATER",
		"LESS",
		
		// Other symbols
		"COMMA",
		"ASSIGN",
		"LEFT_PAREN",
		"RIGHT_PAREN",
		"LEFT_BRACE",
		"RIGHT_BRACE",
		"SEMICOLON",
		"DDOT",
		
		// Keywords
		"IF",
		"ELSE",
		"WHILE",
		"FUNC",
		"RETURN",
		
		// Others
		"HEADER",
		"FOOTER"
	};
	return TOKEN_ENUM_STRINGS[type];
}


/**
 * Inits token array structure and allocates space for tokens
 * 
 * @param ta_ptr Pointer to given token array ready for init
 * 
 * @returns 1 if allocates successfuly otherwise 0
 */
int tok_arr_ctor(tok_arr_t* ta_ptr){
	
	token_t* tokens = malloc(sizeof(token_t) * TOKEN_ARRAY_BASE_SIZE); 
	
	if (tokens == NULL){
		return 1;
	}
	
	ta_ptr->size = TOKEN_ARRAY_BASE_SIZE;
	ta_ptr->len = 0;
	ta_ptr->index = 0;
	ta_ptr->elems = tokens;
	return 0;	
}


/**
 * Inserts new token to token array
 * 
 * @param token_type Type of inserted token
 * @param line Line where the token was read
 * @param column Column where the token was read
 * @param str_ptr Pointer to string that is content of token if needed otherwise NULL
 */
void tok_arr_insert(tok_arr_t* ta_ptr, token_type type, int line, int column, char* str_ptr){
	
	if (ta_ptr->len >= ta_ptr->size){
		// Grows array
		ta_ptr->elems = realloc(ta_ptr->elems, ta_ptr->size * 2 * sizeof(token_t));
		
		if (ta_ptr->elems == NULL){
			return;
		}
		
		ta_ptr->size =  ta_ptr->size * 2;
	}
	
	token_t token = {
		.type = type,
		.line = line,
		.column = column,
		.content = str_ptr
	};
	
	ta_ptr->elems[ta_ptr->len] = token;
	ta_ptr->len++;
	
}


/**
 * Checks if index is on end of token array
 * 
 * @param ta_ptr Pointer to token array
 * @returns 1 if index is on end otherwise 0
 */
bool tok_arr_on_end(tok_arr_t* ta_ptr){
	return ta_ptr->index >= ta_ptr->len;
}


/**
 * Compares given token type with token type on current index
 * 
 * @param ta_ptr Pointer to token array
 * @param type Token type to compare
 * @returns 1 if token types are same otherwise 0
 */
bool tok_arr_cmp(tok_arr_t* ta_ptr, token_type type){
	if(ta_ptr->index < ta_ptr->len){
		return ta_ptr->elems[ta_ptr->index].type == type;
	}
	return false;
}


/**
 * Compares token types on and after current index
 *
 * @param ta_ptr Pointer to token array
 * @param types Array of token types to compare
 * @param types_len Length of token types array
 * @returns 1 if token types math given array otherwise 0
 */
bool tok_arr_cmp_arr(tok_arr_t* ta_ptr, token_type* types, int types_len){
	if( (ta_ptr->index + types_len-1) < ta_ptr->len){
		for (int i = 0; i < types_len; i++){
			token_t tok = ta_ptr->elems[ta_ptr->index + i];
			if (tok.type != types[i]){
				return false;
			}
		}
		return true;
	}
	return false;
}


/**
 * Compares token type on current index with types in between of given range
 * Range is defined as start type and end type in token_type enum
 *
 * @param ta_ptr Pointer to token array
 * @param types Array of token types for comparison
 * @param types_len Length of token types array
 * @returns 1 if token type on current index is in given array otherwise 0
 */
bool tok_arr_cmp_range(tok_arr_t* ta_ptr, token_type start_type, token_type end_type){
	if (ta_ptr->index < ta_ptr->len){
		token_t tok = ta_ptr->elems[ta_ptr->index];
		return tok.type >= start_type && tok.type <= end_type;
	}
	return false;
}


/**
 * Compares given token type with token type on current index + offset
 * 
 * @param ta_ptr Pointer to token array
 * @param type Token type to compare
 * @param offset Offset for current index
 * @returns 1 if token types are same otherwise 0
 */
bool tok_arr_cmp_offset(tok_arr_t* ta_ptr, token_type type, int offset){
	int index = ta_ptr->index + offset;
	if (index > ta_ptr->len - 1 || index < 0){
		return false;
	}
	return ta_ptr->elems[ta_ptr->index + offset].type == type;
}


/**
 * Compares given token type with token type on current index
 * and increments index
 * 
 * @param ta_ptr Pointer to token array
 * @param type Token type to compare
 * @returns 1 if token types are same otherwise 0
 */
bool tok_arr_cmp_skip(tok_arr_t* ta_ptr, token_type type){
	bool result = tok_arr_cmp(ta_ptr,type);
	ta_ptr->index++;
	return result;
}


/**
 * Increments index with given value
 * 
 * @param ta_ptr Pointer to token array
 * @param value Value to increment with
 */
void tok_arr_inc(tok_arr_t* ta_ptr, int value){
	ta_ptr->index += value;
}


/**
 * Returns pointer to token on current index
 * 
 * @param ta_ptr Pointer to token array
 * @returns Pointer to token if index is not on end otherwise NULL
 */
token_t* tok_arr_get(tok_arr_t* ta_ptr){
	if (ta_ptr->len > ta_ptr->index){
		return &ta_ptr->elems[ta_ptr->index];
	} else {
		return NULL;
	}
}


/**
 * Returns pointer to token on current index and increments index
 * 
 * @param ta_ptr Pointer to token array
 * @returns Pointer to token if index is not on end otherwise NULL
 */
token_t* tok_arr_get_next(tok_arr_t* ta_ptr){
	token_t* next_tok_ptr = tok_arr_get(ta_ptr);
	ta_ptr->index++;
	return next_tok_ptr;
}


/**
 * Returns pointer to token on current index + offset
 * 
 * @param ta_ptr Pointer to token array
 * @returns Pointer to token if index is not on end otherwise NULL
 */
token_t* tok_arr_get_offset(tok_arr_t* ta_ptr, int offset){
	int index = ta_ptr->index + offset;
	if (index > ta_ptr->len - 1 || index < 0){
		return NULL;
	}
	return &ta_ptr->elems[ta_ptr->index + offset];
}


/**
 * Counts number of commas in token array until right parenthesis
 * Helper function for counting parameters in function call
 * 
 * @param ta_ptr Pointer to token array
 * @returns Pointer to token if index is not on end otherwise NULL
 */
int tok_arr_get_commas(tok_arr_t* ta_ptr){
	int index = ta_ptr->index;
	int parens = 0;
	int commas = 0;
	while (index < ta_ptr->len){		
		if(!parens && ta_ptr->elems[index].type == COMMA){
			commas++;
		} else if(ta_ptr->elems[index].type == LEFT_PAREN){
			parens++;
		} else if(ta_ptr->elems[index].type == RIGHT_PAREN){
			if(!parens){
				return commas;
			}
			parens--;
		}
		index++;
	}
	return -1;
}

/**
 * Frees memory allocated by given token array
 * 
 * @param ta_ptr Pointer to token array 
 */
void tok_arr_dtor(tok_arr_t* ta_ptr){
	for (int i = 0; i < ta_ptr->len; i++){
		if (ta_ptr->elems[i].content != NULL){
			// Frees the token content if there is some
			free(ta_ptr->elems[i].content);
			ta_ptr->elems[i].content = NULL;
		}
	
	}
	free(ta_ptr->elems);
}


/**
 * Prints out all tokens in array formated by lines
 * Every token is printed out in exact format: [type,content,column,line]
 * 
 * @param ta_ptr Pointer to token array that is printed
 */

void tok_arr_debug_print(tok_arr_t ta_ptr){
	
	printf("Printing tokens in format: [typ,content,column,line]\n");
	int line = 0;
	for (int i = 0; i < ta_ptr.len; i++){
		
		if (line != ta_ptr.elems[i].line){
			printf("\n");
			line = ta_ptr.elems[i].line;
			printf("%d\t|",line);
		}
		
		if (ta_ptr.elems[i].content == NULL){
			printf("[%s,-,%d,%d]", token_enum_to_string(ta_ptr.elems[i].type),ta_ptr.elems[i].line,ta_ptr.elems[i].column);
		} else {
			printf("[%s,%s,%d,%d]", token_enum_to_string(ta_ptr.elems[i].type), ta_ptr.elems[i].content,ta_ptr.elems[i].line,ta_ptr.elems[i].column);
		}
		
	}
	printf("\n");
}


