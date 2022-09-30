#include "shift_buffer.h"

int is_char_number(char c){
	return c >= '0' && c <= '9';
}

int is_char_alpha(char c){
	return c >= 'a' && c <= 'z';
}

int is_char_capital(char c){
	return c >= 'A' && c <= 'Z';
}

int is_char_letter(char c){
	return is_char_alpha(c) || is_char_capital(c); 
}

int is_char_variable_name(char c){
	return is_char_letter(c) || is_char_number(c) || c == '_';
}

	//32 space
	//13 newline RF
	//10 newline LF
	//9  tab

int is_char_permited(char c){
	return (
		c == 32 || // Space
		c == 13 || // RF line ending
		c == 10 || // LF line ending
		c == 9     // Tab
	);
}

void sbuffer_shift(sbuffer_t* sb){	
	
	for (int i = 0; i < BUFFER_SIZE - 1; i++){
		sb->buffer[i] = sb->buffer[i+1];
	}
	
	if(sb->found_end){
		sb->end_index--;
		sb->buffer[BUFFER_SIZE - 1] = '\0';
		return;
	}
	
	char c = fgetc(sb->source);
	sb->buffer[BUFFER_SIZE - 1] = c;
	
	if(sb->buffer[0] == 13){
		sb->line++;
		return;
	}
	
	if(c == EOF){
		sb->found_end = 1;
		sb->end_index = BUFFER_SIZE - 1;
	}
	
}

sbuffer_t* sbuffer_init(FILE* source){
	
	sbuffer_t* sb = malloc(sizeof(sbuffer_t));
	
	if (sb == NULL){
		return NULL;
	}

	sb->found_end = 0;
	sb->line = 1;
	sb->end_index = BUFFER_SIZE;
	sb->source = source;
	
	for (int i = 0; i < BUFFER_SIZE; i++){

		sbuffer_shift(sb);
		
		if(sb->found_end){
			sb->end_index = i;
			break;
		}	
	}
	
	return sb;
}

void sbuffer_skip(sbuffer_t* sb, int to_skip){
	for (int i = 0; i < to_skip; i++){
		sbuffer_shift(sb);
	}
}
