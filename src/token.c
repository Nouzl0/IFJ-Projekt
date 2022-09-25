#include "token.h"


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