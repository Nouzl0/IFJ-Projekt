#include "token.h"

trecord_t symbol_register[22] = {
	{"<?php", HEADER},
	{"?>", FOOTER},
	{"==", EQUAL},
	{"!=", NOT_EQUAL},
	{">=", GREATER_EQUAL},
	{"<=", LESS_EQUAL},
	{">", GREATER},
	{"<", LESS},
	{"=", ASSIGN},
	{"!", NEG},
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

trecord_t keyword_register[11] = {
	{"and", AND}, 
	{"or", OR},
	{"null", NIL},
	{"if", IF},
	{"else", ELSE},
	{"while", WHILE},
	{"function", FUNC},
	{"return", RETURN},
	{"int", INT},
	{"float", FLOAT},
	{"string", STRING}
};

int token_compare_keywords(char* str_ptr){
	int len = sizeof(keyword_register) / sizeof(keyword_register[0]);
	for (int i = 0; i < len; i++){
		if(cstring_compare(keyword_register[i].match,str_ptr)){
			return i;
		}
	}
	return -1;
}

int token_compare_symbol(char* str_ptr){
	int len = sizeof(symbol_register) / sizeof(symbol_register[0]);
	for (int i = 0; i < len; i++){
		if(cstring_compare(symbol_register[i].match,str_ptr)){
			return i;
		}
	}
	return -1;
}



int token_array_ctor(token_array_t* ta_ptr){
	
	token_t* tokens = malloc(sizeof(token_t) * TOKEN_ARRAY_BASE_SIZE); 
	
	if(tokens == NULL){
		return 1;
	}
	
	ta_ptr->size = TOKEN_ARRAY_BASE_SIZE;
	ta_ptr->len = 0;
	ta_ptr->elems = tokens;
	return 0;	
}




/*
void cstring_add_char(cstring_t* cs_ptr, char c){
	//Zjistuje jestli je misto
	if(cs_ptr->len + 1 >= cs_ptr->size){
		cs_ptr->content = realloc(cs_ptr->content, cs_ptr->size * 2 * sizeof(char));
		
		if (cs_ptr->content == NULL){
			//TODO: alloc error handle
		}
		
		cs_ptr->size =  cs_ptr->size * 2;
	}
	
	cs_ptr->content[cs_ptr->len] = c;
	cs_ptr->len++;
	cs_ptr->content[cs_ptr->len] = '\0';
}
*/



int token_array_add(token_array_t* ta_ptr, token_type type, int line, cstring_t* str_ptr){
	
	if(ta_ptr->len >= ta_ptr->size){
		ta_ptr->elems = realloc(ta_ptr->elems, ta_ptr->size * 2 * sizeof(token_t));
		
		if (ta_ptr->elems == NULL){
			//TODO: alloc error handle
		}
		
		ta_ptr->size =  ta_ptr->size * 2;
	}
	
	token_t token = {
		.type = type,
		.line = line,
		.content = str_ptr
	};
	
	ta_ptr->elems[ta_ptr->len] = token;
	ta_ptr->len++;
	return 0;
	
}

void token_array_dtor(token_array_t* ta_ptr){
	for (int i = 0; i < ta_ptr->len; i++){
		//call dtor on ta_ptr->elems[i].content
	}
	free(ta_ptr->elems);
}