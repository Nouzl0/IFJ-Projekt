#include "cstring.h"
/*
TODO:
	
	Vsechny tyto pomocne funkce vlozit do shift buferu a prejmenovat

*/

/**
 * Inicializuje strukturu cstring_t
 * 
 * @returns NULL kdyz se nepovede alokace jinak ukazatel na instanci cstring_t
 */
void cstring_ctor(cstring_t* cs_ptr){
	//Alokuje pole znaku
	char* str_ptr = malloc(sizeof(char) *STRING_BASE_SIZE );

	if (str_ptr == NULL){
		cs_ptr = NULL;
		return;
	}
	
	cs_ptr->size = STRING_BASE_SIZE;
	cs_ptr->len = 0;
	cs_ptr->content = str_ptr;
}


/**
 * Pridava znak do instance cstring_t
 * 
 * @params cstring_t Ukazatel na instanci cstring_t
 * @params c Znak ktery se ma pridat
 */
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

/**
 * Porovnava obsah dvou retezcu podle prvniho 
 *
 * @params str_ptr1 Ukazatel na prvni instanci cstring_t
 * @params str_ptr2 Ukazatel na druhou instanci cstring_t
 *
 * @returns 1 pokud je obsah stejny jinak 0
 */
int cstring_compare(char* str_ptr1, char* str_ptr2){
	int i = 0;
	while(str_ptr1[i] != '\0'){
		if(str_ptr1[i] != str_ptr2[i]){
			return 0;
		}
		i++;
	}
	return 1;
}