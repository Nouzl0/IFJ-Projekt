#include "lex.h"
/*
	TODO:
	
	Pridat vice povolenych znaku pro promene a identifikatory 
	jako cisla a _
	
	Mohlo by se hodit pocitani otevrenych a uzavrenych zavorek
	Nektere funkce by mozna sly implementovat pomoci jedne
	
	Bylo by dobre nastavit limit velikosti nazvu promnenych
	ale stejne to nebude asi testovane takze tak
*/


/**
 * 
 * 
 * @param sb_ptr Ukazatel na posuvny buffer
 */
void handle_variable(sbuffer_t* sb_ptr){
	int line = sb_ptr->line;
	int column = sb_ptr->column;
	
	sbuffer_shift(sb_ptr);
	
	cstring_t cs_ptr;
	cstring_ctor(&cs_ptr);
	while(is_char_variable_name(sb_ptr->buffer[0])){
		cstring_add_char(&cs_ptr, sb_ptr->buffer[0]);
		sbuffer_shift(sb_ptr);
	}
	
	token_array_add(sb_ptr->ta_ptr, VARIABLE, line, column, cs_ptr.content);
}


/**
 * 
 * 
 * @param sb_ptr Ukazatel na posuvny buffer
 */
void handle_number(sbuffer_t* sb_ptr){
	int line = sb_ptr->line;
	int column = sb_ptr->column;
	token_type type = NUMBER;
	
	cstring_t cs_ptr;
	cstring_ctor(&cs_ptr);
	
	if (sb_ptr->buffer[0] == '-'){
		cstring_add_char(&cs_ptr, sb_ptr->buffer[0]);
		sbuffer_shift(sb_ptr);
	}
	
	while(is_char_number(sb_ptr->buffer[0])){
		cstring_add_char(&cs_ptr, sb_ptr->buffer[0]);
		sbuffer_shift(sb_ptr);
		
		//Desetina tecka musi byt vzdy mezi cisly (.5 a 1. nejsou validni)
		if (sb_ptr->buffer[0] == '.' && is_char_number(sb_ptr->buffer[1]) ){
			cstring_add_char(&cs_ptr, sb_ptr->buffer[0]);
			sbuffer_shift(sb_ptr);
			type = FRACTION;
		}
		
	}

	token_array_add(sb_ptr->ta_ptr, type, line, column, cs_ptr.content);
}


/**
 * 
 * 
 * @param sb_ptr Ukazatel na posuvny buffer
 */
void handle_identifier(sbuffer_t* sb_ptr){
	int line = sb_ptr->line;
	int column = sb_ptr->column;
	
	cstring_t cs_ptr;
	cstring_ctor(&cs_ptr);
	while(is_char_variable_name(sb_ptr->buffer[0])){
		cstring_add_char(&cs_ptr, sb_ptr->buffer[0]);
		sbuffer_shift(sb_ptr);
	}

	token_array_add(sb_ptr->ta_ptr, IDENTIFIER, line, column, cs_ptr.content);
}


/**
 * Posunuje buffer do te doby nez narazi na novy radek
 * nebo konec souboru
 * 
 * @param sb_ptr Ukazatel na posuvny buffer
 */
void handle_line_comment(sbuffer_t *sb_ptr){
	int line = sb_ptr->line; 
	while(1){
		if(line != sb_ptr->line || sb_ptr->end_index < 1){
			break;
		}
		sbuffer_shift(sb_ptr);
	}
}


/**
 * Posunuje buffer do te doby nez narazi na konec blokoveho
 * komentare nebo konec souboru pri kterem zaznamena chybu
 *
 * @param eh_ptr Ukazatel na zaznam chyb
 * @param sb_ptr Ukazatel na posuvny buffer
 */
void handle_block_comment(error_handler_t* eh_ptr, sbuffer_t *sb_ptr){
	while(1){
		if(sb_ptr->buffer[0] == '*' && sb_ptr->buffer[1] == '/'){
			sbuffer_skip(sb_ptr,2);
			return;
		}
		//Detekuje nekonecny komentar
		if(sb_ptr->end_index < 1){
			register_lex_error(eh_ptr, sb_ptr->line, "Missing comment ending");
			return;
		}
		
		sbuffer_shift(sb_ptr);
	}
}
/**
 * Posunuje buffer do te doby nez narazi na konec retezce
 * nebo ukonceni souboru pri kterem zaznamena chybu
 * Kazdy posunuty znak uklada do instance cstring
 * a ukazatel na ni pridava do tokenu
 *
 * @param eh_ptr Ukazatel na zaznam chyb
 * @param sb_ptr Ukazatel na posuvny buffer
 */
void handle_text(error_handler_t* eh_ptr, sbuffer_t* sb_ptr){
	char term_char = sb_ptr->buffer[0];
	cstring_t cs_ptr;
	cstring_ctor(&cs_ptr);
	sbuffer_shift(sb_ptr);
	int line = sb_ptr->line;
	int column = sb_ptr->column;
	while(1){
		if(sb_ptr->buffer[0] == term_char){
			token_array_add(sb_ptr->ta_ptr, TEXT, line, column, cs_ptr.content);
			sbuffer_shift(sb_ptr);
			return;
		}
		//Detekuje chybejici ukoceni 
		if(sb_ptr->end_index < 1){
			register_lex_error(eh_ptr, sb_ptr->line, "Missing string ending");
			return;
		}
		cstring_add_char(&cs_ptr,sb_ptr->buffer[0]);
		sbuffer_shift(sb_ptr);
	}
	
}
/**
 * Znak po znaku prochazi zadany soubor a vytvari tokeny
 * ktere pote vklada do pole tokenu
 *
 * @param eh_ptr Ukazatel na zaznam chyb
 * @param ta_ptr Ukazatel na pole tokenu
 * @param source Ukazatel na zdroj ze ktereho se maji cist znaky
 */
void lex_tokenize(error_handler_t* eh_ptr, token_array_t* ta_ptr, FILE* source){
	sbuffer_t* sb = sbuffer_init(source);
	
	sb->ta_ptr = ta_ptr;
	
	while(sb->end_index > 1){
		//Jmeno promene
		if(sb->buffer[0] == '$' &&  is_char_variable_name(sb->buffer[1])){
			handle_variable(sb);
			continue;
		}
		//Cislo nebo zaporne cislo
		if( (is_char_number(sb->buffer[0])) || (sb->buffer[0] =='-' && is_char_number(sb->buffer[1])) ){
			handle_number(sb);
			continue;
		}
		
		//Klicove slovo a identifikator
		if(is_char_letter(sb->buffer[0])){
			int token_type = 0;
			int to_skip = token_compare_keywords(sb->buffer,&token_type);
			if(to_skip){
				char next = sb->buffer[to_skip];
				if(!is_char_variable_name(next)){
					token_array_add(sb->ta_ptr, token_type, sb->line, sb->column, NULL);
					sbuffer_skip(sb,to_skip);
					continue;
				}
			} 
			handle_identifier(sb);
			continue;
		}
		
		//Komentare
		if(sb->buffer[0] == '/'){
			if(sb->buffer[1] == '/'){
				handle_line_comment(sb);
				continue;
			}
			if(sb->buffer[1] == '*'){
				handle_block_comment(eh_ptr,sb);
				continue;
			}
		}
		
		//Obsah retezcu
		if(sb->buffer[0] == '"' || sb->buffer[0] == '\''){
			handle_text(eh_ptr,sb);
			continue;
		}
		
		//Operatory a symboly + php header a footer
		int token_type = 0;
		int to_skip = token_compare_symbol(sb->buffer, &token_type);
		if(to_skip){
			token_array_add(sb->ta_ptr, token_type, sb->line, sb->column, NULL);
			sbuffer_skip(sb,to_skip);
			continue;
		}
		
		//Ostatni povolene znaky
		if(is_char_permited(sb->buffer[0])){
			sbuffer_shift(sb);
			continue;
		}
		
		//Chyba nepovoleny znak
		register_lex_error(eh_ptr, sb->line, sb->buffer);
		free(sb);
		return;
	} 
	
	free(sb);
	return;
}

/**
 * Pomocna funkce ktere vola lex_tokenize ale zarucuje
 * otevreni potrebneho souboru podle nazvu
 *
 * @param eh_ptr Ukazatel na zaznam chyb
 * @param ta_ptr Ukazatel na pole tokenu
 * @param file_name Ukazatel na retezec s nazvem souboru
 */
void lex_tokenize_file(error_handler_t* eh_ptr, token_array_t* ta_ptr, char* file_name){
	
	FILE* f = fopen(file_name,"r");

	if (f == NULL){
		printf("Source file not found");
		return;
	}

	lex_tokenize(eh_ptr,ta_ptr,f);
	fclose(f);
	
}
