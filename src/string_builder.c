#include <stdio.h>

#include "string_builder.h"

string_builder_t* string_builder_ctor(){
	//Alokuje samoty string builder
	string_builder_t* sb_ptr = malloc(sizeof(string_builder_t));
	//Alokuje pole znaku
	char* str_ptr = malloc(sizeof(char) *STRING_BASE_SIZE );

	if (sb_ptr == NULL || str_ptr == NULL){
		//TODO: alloc error handle
		return NULL;
	}
	
	sb_ptr->size = STRING_BASE_SIZE;
	sb_ptr->len = 0;
	sb_ptr->content = str_ptr;

	return sb_ptr;
}

void string_builder_add_char(string_builder_t* sb_ptr, char c){
	//Zjistuje jestli je misto
	if(sb_ptr->len + 1 >= sb_ptr->size){
		sb_ptr->content = realloc(sb_ptr->content, sb_ptr->size * 2 * sizeof(char));
		
		if (sb_ptr->content == NULL){
			//TODO: alloc error handle
		}
		
		sb_ptr->size =  sb_ptr->size * 2;
	}
	
	sb_ptr->content[sb_ptr->len] = c;
	sb_ptr->len++;
	sb_ptr->content[sb_ptr->len] = '\0';
}

void string_builder_print(string_builder_t* sb_ptr){
	for (int i = 0; i < sb_ptr->len; i++){
		printf("%c",sb_ptr->content[i]);
	}
	printf("%c",'\n');
}

void string_builder_dtor(string_builder_t* sb_ptr){
	free(sb_ptr->content);
	free(sb_ptr);
}