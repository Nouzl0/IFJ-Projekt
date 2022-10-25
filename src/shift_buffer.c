#include "shift_buffer.h"
/* Zjistuje jestli je znak cislo */
int is_char_number(char c){
	return c >= '0' && c <= '9';
}
/* Zjistuje jestli je znak male pismeno */
int is_char_alpha(char c){
	return c >= 'a' && c <= 'z';
}
/* Vraci jestli je znak velke pismeno */
int is_char_capital(char c){
	return c >= 'A' && c <= 'Z';
}
/* Vraci jestli je znak pismeno */
int is_char_letter(char c){
	return is_char_alpha(c) || is_char_capital(c); 
}
/* Vraci jestli je znak platny nazev promene */
int is_char_variable_name(char c){
	return is_char_letter(c) || is_char_number(c) || c == '_';
}
/* Vraci jestli je znak whitespace */
int is_char_permited(char c){
	return (
		c == 32 || // Space
		c == 13 || // RF line ending
		c == 10 || // LF line ending
		c == 9     // Tab
	);
}
/**
 * Posunuje znaky v posuvnem bufferu
 * 
 * @param sb_ptr Ukazatel na posuvny buffer
 * 
 */
void sbuffer_shift(sbuffer_t* sb_ptr){
	sb_ptr->column++;	
	
	for (int i = 0; i < BUFFER_SIZE - 1; i++){
		sb_ptr->buffer[i] = sb_ptr->buffer[i+1];
	}
	
	//Zaregistruje novy radek
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
 * Naplni posuvny buffer
 * 
 * @param source Soubor ze ktereho se maji znaky cist
 * 
 * @returns Null pri nepovedene alokaci jinak ukazatel na instanci posuvneho bufferu
 */
sbuffer_t* sbuffer_init(FILE* source){
	
	sbuffer_t* sb = malloc(sizeof(sbuffer_t));
	
	if (sb == NULL){
		return NULL;
	}

	sb->found_end = 0;
	sb->line = 1;
	sb->end_index = BUFFER_SIZE;
	sb->source = source;
	sb->column = 1;
	
	for(int i = 0; i < BUFFER_SIZE; i++){
		sb->buffer[i] = 0;
	}
	
	for (int i = 0; i < BUFFER_SIZE; i++){

		sbuffer_shift(sb);
		
		if(sb->found_end){
			sb->end_index = i;
			break;
		}	
	}
	
	sb->column = 1;
	
	return sb;
}
/**
 * Posunuje znaky v posuvnem registru o toklik o kolik je zadano
 * 
 * @param sb_ptr Ukazatel na posuvny buffer
 * @param to_skip Pocet o kolik se ma buffer posunout
 */
void sbuffer_skip(sbuffer_t* sb_ptr, int to_skip){
	for (int i = 0; i < to_skip; i++){
		sbuffer_shift(sb_ptr);
	}
}
