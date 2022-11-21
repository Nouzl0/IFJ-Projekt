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
 * Shifts buffer and saves characters for variable name
 * When shifts to invalid character saves token to array
 * (Valid variable characters defined in strings_lib.c) 
 *
 * @param sb_ptr Shift buffer pointer (defined in strings_lib.h)
 */
void handle_variable(sbuffer_t* sb_ptr){
	int line = sb_ptr->line;
	int column = sb_ptr->column;
	
	sbuffer_shift(sb_ptr);
	
	str_builder_t cs_ptr;
	str_builder_ctor(&cs_ptr);
	while(is_char_variable_name(sb_ptr->buffer[0])){
		str_builder_append(&cs_ptr, sb_ptr->buffer[0]);
		sbuffer_shift(sb_ptr);
	}
	
	tok_arr_insert(sb_ptr->ta_ptr, VARIABLE, line, column, cs_ptr.content);
}


/**
 * Shifts buffer and saves characters for numeral constant
 * When shifts to end of number saves token to array
 * (Valid number characters defined in strings_lib.c) 
 *
 * @param sb_ptr Shift buffer pointer (defined in strings_lib.h)
 */
void handle_number(sbuffer_t* sb_ptr){
	int line = sb_ptr->line;
	int column = sb_ptr->column;
	token_type type = NUMBER;
	
	str_builder_t cs_ptr;
	str_builder_ctor(&cs_ptr);
	
	// Checks for negative numeral
	if (sb_ptr->buffer[0] == '-'){
		str_builder_append(&cs_ptr, sb_ptr->buffer[0]);
		sbuffer_shift(sb_ptr);
	}
	
	while(is_char_number(sb_ptr->buffer[0])){
		str_builder_append(&cs_ptr, sb_ptr->buffer[0]);
		sbuffer_shift(sb_ptr);
		
		/*
			Decimal point is only valid in between of valid numeral characters
			(.5 and 1. is NOT valid)
		*/
		if (sb_ptr->buffer[0] == '.' && is_char_number(sb_ptr->buffer[1]) ){
			str_builder_append(&cs_ptr, sb_ptr->buffer[0]);
			sbuffer_shift(sb_ptr);
			type = FRACTION;
		}
		
	}

	tok_arr_insert(sb_ptr->ta_ptr, type, line, column, cs_ptr.content);
}


/**
 * Shifts buffer and saves characters for identifier name
 * When shifts to invalid character saves token to array
 * (Valid name characters defined in strings_lib.c) 
 *
 * @param sb_ptr Shift buffer pointer (defined in strings_lib.h)
 */
void handle_identifier(sbuffer_t* sb_ptr){
	int line = sb_ptr->line;
	int column = sb_ptr->column;
	
	str_builder_t cs_ptr;
	str_builder_ctor(&cs_ptr);
	while(is_char_variable_name(sb_ptr->buffer[0])){
		str_builder_append(&cs_ptr, sb_ptr->buffer[0]);
		sbuffer_shift(sb_ptr);
	}

	tok_arr_insert(sb_ptr->ta_ptr, IDENTIFIER, line, column, cs_ptr.content);
}


/**
 * Shifts buffer until it arrives to newline character
 * Ignoring all characters when shifting
 *
 * @param sb_ptr Shift buffer pointer (defined in strings_lib.h)
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
 * Shifts buffer until it arrives to block comment end (star+slash)
 * Ignoring all characters when shifting
 * When shifts to EOF registers error
 *
 * @param sb_ptr Shift buffer pointer (defined in strings_lib.h)
 */
void handle_block_comment(sbuffer_t *sb_ptr){
	while(1){
		if(sb_ptr->buffer[0] == '*' && sb_ptr->buffer[1] == '/'){
			sbuffer_skip(sb_ptr,2);
			return;
		}
		// Checks for EOF
		if(sb_ptr->end_index < 1){
			lex_error(sb_ptr->line, "Missing comment ending");
			return;
		}
		
		sbuffer_shift(sb_ptr);
	}
}


/**
 * Shifts buffer until it arrives to end of string character (") or (')
 * then saves token to array
 * All characters shifted upon are saved to cstring (defined in strings_lib.h)
 * When shifts to EOF registers error
 *
 * @param sb_ptr Shift buffer pointer (defined in strings_lib.h)
 */
void handle_text(sbuffer_t* sb_ptr){
	char term_char = sb_ptr->buffer[0]; //Storing which character started the string
	str_builder_t cs_ptr;
	str_builder_ctor(&cs_ptr);
	sbuffer_shift(sb_ptr);
	int line = sb_ptr->line;
	int column = sb_ptr->column;
	while(1){
		if(sb_ptr->buffer[0] == term_char){
			tok_arr_insert(sb_ptr->ta_ptr, TEXT, line, column, cs_ptr.content);
			sbuffer_shift(sb_ptr);
			return;
		}
		// Checks for EOF
		if(sb_ptr->end_index < 1){
			lex_error(sb_ptr->line, "Missing string ending");
			return;
		}
		str_builder_append(&cs_ptr,sb_ptr->buffer[0]);
		sbuffer_shift(sb_ptr);
	}
	
}


/**
 * Defines rules for calling functions that shift buffer and save tokens
 * with corresponding content
 * Looping through rules until shift buffer shifts upon EOF or 
 * there is forbidden character
 *
 * @param ta_ptr token array pointer that stores saved tokens
 * @param source file pointer which is read from
 */
void lex_tokenize(tok_arr_t* ta_ptr, FILE* source){
	sbuffer_t* sb = sbuffer_init(source);
	sb->ta_ptr = ta_ptr;
	
	while(sb->end_index > 0){
		// Variable name
		if(sb->buffer[0] == '$' &&  is_char_variable_name(sb->buffer[1])){
			handle_variable(sb);
			continue;
		}
		// Any numeral constant
		if( (is_char_number(sb->buffer[0])) || (sb->buffer[0] =='-' && is_char_number(sb->buffer[1])) ){
			handle_number(sb);
			continue;
		}
		
		// Keyword or identifier
		if(is_char_letter(sb->buffer[0])){
			int token_type = 0;
			int to_skip = token_compare_keywords(sb->buffer,&token_type);
			if(to_skip){
				/*	
				  If there are more then zero characters to skip
				  it means that there was a keyword match 
				*/
				char next = sb->buffer[to_skip];
				if(!is_char_variable_name(next)){
					/*	
					  There should NOT be any valid name characters after matched keyword
					  otherwise it means that matched keyword is part of name
					*/
					tok_arr_insert(sb->ta_ptr, token_type, sb->line, sb->column, NULL);
					sbuffer_skip(sb,to_skip);
					continue;
				}
			}
			// Keyword was not found in current content of shift buffer
			handle_identifier(sb);
			continue;
		}
		
		// Multi-line and single-line comments
		if(sb->buffer[0] == '/'){
			if(sb->buffer[1] == '/'){
				handle_line_comment(sb);
				continue;
			}
			if(sb->buffer[1] == '*'){
				handle_block_comment(sb);
				continue;
			}
		}
		
		// String constant content
		if(sb->buffer[0] == '"' || sb->buffer[0] == '\''){
			handle_text(sb);
			continue;
		}
		
		// Operators and other permited symbols, php header and footer
		int token_type = 0;
		int to_skip = token_compare_symbol(sb->buffer, &token_type);
		if(to_skip){
			// If there are characters to skip it matched with valid symbol
			tok_arr_insert(sb->ta_ptr, token_type, sb->line, sb->column, NULL);
			sbuffer_skip(sb,to_skip);
			continue;
		}
		
		// Other permited characters (defined in strings_lib.c)
		if(is_char_permited(sb->buffer[0])){
			sbuffer_shift(sb);
			continue;
		}
		
		// Non of the rules above can be applied
		lex_error(sb->line,sb->buffer);
		free(sb);
		return;
	} 
	
	free(sb);
	return;
}

/**
 * Helper function for calling lex_tokenize
 * Opens and closes file with given file name
 * and feeds it to lex_tokenize
 *
 * @param ta_ptr token array pointer that stores saved tokens
 * @param file_name string that stores given file name
 */
void lex_tokenize_file(tok_arr_t* ta_ptr, char* file_name){
	
	FILE* f = fopen(file_name,"r");

	if (f == NULL){
		printf("Source file not found\n");
		return;
	}

	lex_tokenize(ta_ptr,f);
	fclose(f);
	
}
