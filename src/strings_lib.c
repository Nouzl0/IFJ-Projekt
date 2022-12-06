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
bool is_char_valid_hex(char c){
	if(c >= 'a' && c <= 'f'){
		return true;
	}
	if(c >= 'A' && c <= 'F'){
		return true;
	}
	return is_char_number(c);
}
bool is_char_valid_octal(char c){
	return c >= '0' && c <= '8';
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


/**
 * Sets necessary values in instance of shift buffer before usage
 * Fills shift buffer with initial characters from source
 * 
 * @param sb_ptr Pointer to given instance of shift buffer
 * @param source File pointer of opened file ready for reading
 * @returns Shift buffer pointer if allocates successfuly otherwise NULL
 */
void sbuffer_ctor(sbuffer_t* sb_ptr, FILE* source){
	sb_ptr->found_end = 0;
	sb_ptr->line = 1;
	sb_ptr->end_index = BUFFER_SIZE;
	sb_ptr->source = source;
	sb_ptr->column = 1;
	
	for(int i = 0; i < BUFFER_SIZE; i++){
		sb_ptr->buffer[i] = 0;
	}
	
	sbuffer_skip(sb_ptr,BUFFER_SIZE);
	sb_ptr->column = 1;
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


bool str_cmp_files(char* file_name1, char* file_name2){
	FILE* f1 = fopen(file_name1,"r");
	FILE* f2 = fopen(file_name2,"r");
	
	if(!f1 || !f2){
		return 1; 
	}
	
	while(true){
		char c1 = fgetc(f1);
		char c2 = fgetc(f2);
	
		if(c1 != c2){
			fclose(f1);
			fclose(f2);
			return 1;
		}
		
		if(c1 == EOF){
			fclose(f1);
			fclose(f2);
			return 0;
		}
		
	}
}

void str_ptrint_file(char* file_name){
	FILE* file = fopen(file_name,"r");
	printf("---- %s ----\n",file_name);
	if(!file){
		printf("File not found!");
		fclose(file);
		return;
	}
	
	while(true){
		char c = fgetc(file);
		if(c == EOF){
			break;
		}
		printf("%c",c);
	}
	fclose(file);
	printf("\n----------------------------\n");
}