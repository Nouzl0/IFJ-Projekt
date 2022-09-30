#include "lex.h"

int handle_variable(sbuffer_t* sb_ptr){
	sbuffer_shift(sb_ptr);
	
	cstring_t* cs_ptr = cstring_ctor();
	while(is_char_variable_name(sb_ptr->buffer[0])){
		cstring_add_char(cs_ptr, sb_ptr->buffer[0]);
		sbuffer_shift(sb_ptr);
	}
	printf("VARIABLE: %s\n",cs_ptr->content);
	cstring_dtor(cs_ptr);

	return 0;
}

int handle_number(sbuffer_t* sb_ptr){
	cstring_t* cs_ptr = cstring_ctor();
	while(is_char_number(sb_ptr->buffer[0])){
		cstring_add_char(cs_ptr, sb_ptr->buffer[0]);
		sbuffer_shift(sb_ptr);
	}
	printf("NUMBER: %s\n",cs_ptr->content);
	cstring_dtor(cs_ptr);

	return 0;
}

int handle_keyword(sbuffer_t* sb_ptr,int index){
	printf("KEYWORD: %s\n", keyword_register[index].match);
	sbuffer_skip(sb_ptr,cstring_get_length(keyword_register[index].match));
	return 1;
}

int handle_identifier(sbuffer_t* sb_ptr){
	cstring_t* cs_ptr = cstring_ctor();
	while(is_char_variable_name(sb_ptr->buffer[0])){
		cstring_add_char(cs_ptr, sb_ptr->buffer[0]);
		sbuffer_shift(sb_ptr);
	}
	printf("IDENTIFIER: %s\n",cs_ptr->content);
	cstring_dtor(cs_ptr);

	return 0;
}



void handle_line_comment(sbuffer_t *sb_ptr){
	while(1){
		if(sb_ptr->buffer[0] == 10 || sb_ptr->found_end){
			break;
		}
		sbuffer_shift(sb_ptr);
	}
}

int handle_block_comment(sbuffer_t *sb_ptr){
	while(1){
		if(sb_ptr->buffer[0] == '*' && sb_ptr->buffer[1] == '/'){
			sbuffer_skip(sb_ptr,2);
			return 0;
		}
		
		if(sb_ptr->found_end){
			return 1;
		}
		
		sbuffer_shift(sb_ptr);
	}
}

int handle_text(sbuffer_t* sb_ptr){
	char term_char = sb_ptr->buffer[0];
	cstring_t* cs_ptr = cstring_ctor();
	sbuffer_shift(sb_ptr);
	while(1){
		if(sb_ptr->buffer[0] == term_char){
			printf("TEXT: %s\n",cs_ptr->content);
			cstring_dtor(cs_ptr);
			sbuffer_shift(sb_ptr);
			return 0;
		}
		
		if(sb_ptr->found_end){
			return 1;
		}
		cstring_add_char(cs_ptr,sb_ptr->buffer[0]);
		sbuffer_shift(sb_ptr);
	}
	
}

void handle_symbol(sbuffer_t* sb_ptr, int index){
	printf("SYMBOL: %s\n", symbol_register[index].match);
	sbuffer_skip(sb_ptr,cstring_get_length(symbol_register[index].match));
}

void lex_tokenize(token_array_t* ta_ptr, FILE* source){
	
	//token_array_add(ta_ptr,VARIABLE,0,NULL)
	
	sbuffer_t* sb = sbuffer_init(source);
	
	while(sb->end_index > 0){
		
		//Jmeno promene
		if(sb->buffer[0] == '$'){
			handle_variable(sb);
			continue;
		}

		//Vsechny cisla
		if(is_char_number(sb->buffer[0])){
			handle_number(sb);
			continue;
		}
		
		//Klicove slovo a identifikator
		if(is_char_letter(sb->buffer[0])){
			
			int keyword_index = token_compare_keywords(sb->buffer);
			
			if(keyword_index != -1){
				char next = sb->buffer[cstring_get_length(keyword_register[keyword_index].match)];
				if(!is_char_variable_name(next)){
					handle_keyword(sb,keyword_index);
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
				handle_block_comment(sb);
				continue;
			}
		}
		
		//Obsah retezcu
		if(sb->buffer[0] == '"' || sb->buffer[0] == '\''){
			handle_text(sb);
			continue;
		}
		
		//Operatory a symboly + php header a footer
		int index = token_compare_symbol(sb->buffer);
		if(index > -1){
			handle_symbol(sb,index);
			continue;
		}
		
		//Ostatni povolene znaky
		if(is_char_permited(sb->buffer[0])){
			sbuffer_shift(sb);
			continue;
		}
		
		printf("Chyba na radku %d. ve znaku: |%c| ",sb->line,sb->buffer[0]);
		break;
	} 
	
	
	
	free(sb);
	
	

	
}

void lex_tokenize_file(token_array_t* ta_ptr, char* file_name){
	
	FILE* f = fopen(file_name,"r");

	if (f == NULL){
		return;
	}

	lex_tokenize(ta_ptr,f);
	
	free(f);
}
