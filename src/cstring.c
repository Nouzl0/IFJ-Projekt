#include "cstring.h"


/**
 * Vytvari novou instanci struktury cstring_t
 * 
 * @returns NULL kdyz se nepovede alokace jinak ukazatel na instanci cstring_t
 */
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


/**
 * Uvolni pamet vymezenou pro strukturu cstring_t
 * 
 * @params cstring_t Ukazatel na instanci struktury cstring_t
 */
void cstring_dtor(cstring_t* cs_ptr){
	free(cs_ptr->content);
	free(cs_ptr);
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
 * Vypisuje obsah isntance cstring 
 * 
 * @params cstring_t Ukazatel na instanci cstring_t
 */
void cstring_debug_print(cstring_t* cs_ptr){
	for (int i = 0; i < cs_ptr->len; i++){
		printf("|%c|",cs_ptr->content[i]);
	}
	
	printf("%c",'\n');
}


/**
 * Konvertuje retezec na instanci struktury cstring_t
 * 
 * @params str_ptr Ukazatel na retezec
 * @params len Delka daneho retezce
 *
 * @returns NULL kdyz se nepovede alokace jinak ukazatel na instanci cstring_t 
 */
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


/**
 * Vypisuje obash daneho retezce pomoci konvertovani na cstring_t
 * 
 * @params str_ptr Ukazatel na retezec
 * @params len Delka daneho retezce
 */
void cstring_debug_import_print(char* str_ptr, int len){
	cstring_t* cs_ptr = cstring_import(str_ptr,len);
	cstring_debug_print(cs_ptr);
	cstring_dtor(cs_ptr);
}


/**
 * Vraci delku retezce
 * 
 * @params str_ptr Ukazatel na retezec
 *
 * @returns Delku daneho retezce
 */
int cstring_get_length(char* str_ptr){
	int len = 0;
	while(str_ptr[len] != '\0'){
		len++;
	}
	return len;
}


/**
 * Konvertuje retezec na instanci struktury cstring_t kdyz neni znama delka retezce
 * 
 * @params str_ptr Ukazatel na retezec
 *
 * @returns Delku daneho retezce
 */
cstring_t* cstring_new(char* str_ptr){
	return cstring_import(str_ptr,cstring_get_length(str_ptr));
}


/**
 * Porovnava obsah dvou instanci cstring_t
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