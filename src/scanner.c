#include "scanner.h"

int is_char_number(char c){
	return c >= '0' && c <= '9';
}

int is_char_alpha(char c){
	return c >= 'a' && c <= 'z';
}

int is_char_capital(char c){
	return c >= 'A' && c <= 'Z';
}

int is_char_valid_body_of_id(char c){
	return is_char_number(c) || is_char_alpha(c) || is_char_capital(c) || c == '_';
}


/*
void read_id(sb){
	cs_ptr = cstring_ctor();
	cstring_add_char(cs_ptr, char c){
	
}
*/



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
	
	if(c == EOF){
		sb->found_end = 1;
		sb->end_index = BUFFER_SIZE - 1;
	}
	
	
}

void sbuffer_skip(sbuffer_t* sb, int to_skip){
	for (int i = 0; i < to_skip; i++){
		sbuffer_shift(sb);
	}
}

sbuffer_t* sbuffer_init(FILE* source){
	
	sbuffer_t* sb = malloc(sizeof(sbuffer_t));
	
	if (sb == NULL){
		return NULL;
	}

	sb->found_end = 0;
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

void scanner_tokenize(FILE* source){
	
	sbuffer_t* sb = sbuffer_init(source);
	
	while(sb->end_index > 0){
		
		//printf("%c",sb->buffer[0]);

		//Handling keywordu a mozneho idetifikatoru
		if(is_char_alpha(sb->buffer[0])){
			
			int to_skip = compare_keywords(sb->buffer);
			
			if(to_skip){
				sbuffer_skip(sb,to_skip);
			} else {
				cstring_t* cs_ptr = cstring_ctor();
				
				while(is_char_valid_body_of_id(sb->buffer[0])){
					cstring_add_char(cs_ptr, sb->buffer[0]);
					sbuffer_shift(sb);
				}
				
				printf("Idecko: %s\n",cs_ptr->content);
				cstring_dtor(cs_ptr);
			}
			continue;
	
		}

		//Handling intu
		if(is_char_number(sb->buffer[0])){
			printf("Cislo: ");
			while (is_char_number(sb->buffer[0])){
				printf("%c",sb->buffer[0]);
				sbuffer_shift(sb);
			}
			printf("\n");
			continue;
		}

		sbuffer_shift(sb);
	} 
	
	
	//cstring_import_print(sb->buffer, BUFFER_SIZE);
	//cstring_import_print(sb->buffer, BUFFER_SIZE);
	
	free(sb);
	
	
	//cstring_new("procedure");
	
	
	//32 space
	//13 newline RF
	//10 newline LF
	//9  tab
	
}

void scanner_tokenize_file(char* file_name){
	
	FILE* f = fopen(file_name,"r");

	if (f == NULL){
		return;
	}

	scanner_tokenize(f);
	
	free(f);
}
