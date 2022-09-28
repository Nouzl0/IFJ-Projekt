#include "token.h"

static trecord_t symbol_register[50] = {
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
	{"$", DOL}
};

static trecord_t keyword_register[11] = {
	{"and", AND}, 
	{"or", OR},
	{"null", NIL},
	{"if", IF},
	{"else", ELSE},
	{"while", WHILE},
	{"return", RETURN},
	{"int", INT},
	{"float", FLOAT},
	{"string", STRING},
	{"function", FUNC}
};

int compare_keywords(char* str_ptr){
	int len = sizeof(keyword_register) / sizeof(keyword_register[0]);
	
	for (int i = 0; i < len; i++){
		if(cstring_compare(keyword_register[i].match,str_ptr)){
			printf("Klicove slovo: %s\n", keyword_register[i].match);
			return cstring_get_length(keyword_register[i].match);
		}
	}
	return 0;
}




/*
int token_array_ctor(token_array_t* token_array){
	
	token_t* tokens = malloc(sizeof(token_t) * TOKEN_ARRAY_BASE_SIZE); 
	
	if(tokens == NULL){
		return 1;
	}
	
	token_array->size = TOKEN_ARRAY_BASE_SIZE;
	token_array->len = 0;
	token_array->elems = tokens;
	return 0;
	
}
*/
/*
int add_new_token(int line){
			
	//Alokuje misto na obsah tokenu
	char* content = malloc(sizeof(char)*TOKEN_BASE_SIZE); 
	//Chyba alokace
	if(content == NULL){
		return 1;
	}
	
	token_t token = {
		.size = TOKEN_BASE_SIZE + 1,
		.len = 0,
		.line = line,
		.content = content
	};
	
	token.len = 4;
	
	free(content);
	return 0;
	

}*/