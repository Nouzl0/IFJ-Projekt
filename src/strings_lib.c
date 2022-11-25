#include "strings_lib.h"
/***********************************
	Character grouping functions
************************************/
bool is_char_number(char c){
	return c >= '0' && c <= '9';
}
bool is_char_alpha(char c){
	return c >= 'a' && c <= 'z';
}
bool is_char_capital(char c){
	return c >= 'A' && c <= 'Z';
}
bool is_char_letter(char c){
	return is_char_alpha(c) || is_char_capital(c); 
}
bool is_char_variable_name(char c){
	return is_char_letter(c) || is_char_number(c) || c == '_';
}
bool is_char_permited(char c){
	return (
		c == 32 || // Space
		c == 13 || // RF line ending
		c == 10 || // LF line ending
		c == 9     // Tab
	);
}

/**************************************
	Shift buffer handling functions
***************************************/

/**
 * Shifts characters in array by one to the left
 * Last character is read from source
 * First character is rewriten 
 * If newline character is read incrementing internal counter
 * If EOF is read shifting now adds null character to end
 * until EOF is not on first index
 *
 * @param sb_ptr Pointer to instance of shift buffer
 * 
 */
void sbuffer_shift(sbuffer_t* sb_ptr){
	sb_ptr->column++;	
	
	for(int i = 0; i < BUFFER_SIZE - 1; i++){
		sb_ptr->buffer[i] = sb_ptr->buffer[i+1];
	}
	
	if(sb_ptr->buffer[0] == '\n'){
		sb_ptr->line++;
		sb_ptr->column = 0;
	}
	
	if(sb_ptr->found_end){
		sb_ptr->end_index--;
		sb_ptr->buffer[BUFFER_SIZE - 1] = '\0';
		return;
	}
	
	char c = fgetc(sb_ptr->source);
	sb_ptr->buffer[BUFFER_SIZE - 1] = c;
	
	if(c == EOF){
		sb_ptr->found_end = 1;
		sb_ptr->end_index = BUFFER_SIZE - 1;
	}
	
}
/**
 * Creates instance of shift buffer and shifts until array is full
 * 
 * @param source File pointer of opened file ready for reading
 * 
 * @returns Shift buffer pointer if allocates successfuly otherwise NULL
 */
sbuffer_t* sbuffer_init(FILE* source){
	
	sbuffer_t* sb = malloc(sizeof(sbuffer_t));
	
	if(sb == NULL){
		return NULL;
	}

	sb->found_end = 0;
	sb->line = 1;
	sb->end_index = BUFFER_SIZE;
	sb->source = source;
	sb->column = 1;
	
	for(int i = 0; i < BUFFER_SIZE; i++){

		sbuffer_shift(sb);

	}
	
	sb->column = 1;
	
	return sb;
}
/**
 * Calls sbuffer_shift given number of times
 * 
 * @param sb_ptr Pointer to instance of shift buffer
 * @param to_skip Given number of needed shifts
 */
void sbuffer_skip(sbuffer_t* sb_ptr, int to_skip){
	for(int i = 0; i < to_skip; i++){
		sbuffer_shift(sb_ptr);
	}
}

/****************************************
	String builder handling functions
*****************************************/


/**
 * Inits string builder structure
 * Allocates character array for new string 
 *
 * @param cs_ptr Pointer to instance of string builder for init
 */
void str_builder_ctor(str_builder_t* cs_ptr){
	char* str_ptr = malloc(sizeof(char) *STRING_BASE_SIZE );

	if(str_ptr == NULL){
		cs_ptr = NULL;
		return;
	}
	
	cs_ptr->size = STRING_BASE_SIZE;
	cs_ptr->len = 0;
	cs_ptr->content = str_ptr;
}


/**
 * Adding given character to end of character array
 * 
 * @params cs_ptr Pointer to insance of string builder
 * @params c Added character
 */
void str_builder_append(str_builder_t* cs_ptr, char c){
	if(cs_ptr->len + 1 >= cs_ptr->size){
		// Grows character array
		cs_ptr->content = realloc(cs_ptr->content, cs_ptr->size * 2 * sizeof(char));
		
		if(cs_ptr->content == NULL){
			return;
		}
		
		cs_ptr->size =  cs_ptr->size * 2;
	}
	
	cs_ptr->content[cs_ptr->len] = c;
	cs_ptr->len++;
	cs_ptr->content[cs_ptr->len] = '\0';
}

/**
 * Compares string by first given string
 * When strings are matching to the end of the first string
 * it returns match even if the second string is longer
 *
 * @params str_ptr1 First string to compare
 * @params str_ptr2 Second string to compare
 *
 * @returns true if string are matching otherwise false
 */
bool str_builder_cmp(char* str_ptr1, char* str_ptr2){
	int i = 0;
	while(str_ptr1[i] != '\0'){
		if(str_ptr1[i] != str_ptr2[i]){
			return 0;
		}
		i++;
	}
	return 1;
}