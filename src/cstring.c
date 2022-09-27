#include "cstring.h"

cstring_t* cstring_ctor(){
	//Alokuje samoty string builder
	cstring_t* cs_ptr = malloc(sizeof(cstring_t));
	//Alokuje pole znaku
	char* str_ptr = malloc(sizeof(char) *STRING_BASE_SIZE );

	if (cs_ptr == NULL || str_ptr == NULL){
		return NULL;
	}
	
	cs_ptr->size = STRING_BASE_SIZE;
	cs_ptr->len = 0;
	cs_ptr->content = str_ptr;

	return cs_ptr;
}

void cstring_dtor(cstring_t* cs_ptr){
	free(cs_ptr->content);
	free(cs_ptr);
}

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

void cstring_print(cstring_t* cs_ptr){
	for (int i = 0; i < cs_ptr->len; i++){
		printf("|%c|",cs_ptr->content[i]);
	}
	
	printf("%c",'\n');
}

cstring_t* cstring_import(char* str_ptr, int len){
	cstring_t* cs_ptr = malloc(sizeof(cstring_t));
	char* content_ptr = malloc(sizeof(char) * (len + 1));
	
	if (cs_ptr == NULL || str_ptr == NULL){
		return NULL;
	}
	
	cs_ptr->size = len + 1;
	cs_ptr->len = len;
	cs_ptr->content = content_ptr;

	for (int i = 0; i < len; i++){
		cs_ptr->content[i] = str_ptr[i];
	}

	cs_ptr->content[len] = '\0';

	return cs_ptr;
	
}

void cstring_import_print(char* str_ptr, int len){
	cstring_t* cs_ptr = cstring_import(str_ptr,len);
	cstring_print(cs_ptr);
	cstring_dtor(cs_ptr);
}