#include "scanner.h"

void sbuffer_shift(sbuffer_t* sb, char c){
	for (int i = 0; i < BUFFER_SIZE - 1; i++){
		sb->buffer[i] = sb->buffer[i+1];
	}
	sb->buffer[BUFFER_SIZE - 1] = c;
	
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
	
	for (int i = 0; i < BUFFER_SIZE; i++){
		char c = fgetc(source);

		sbuffer_shift(sb,c);
		
		if(sb->found_end){
			sb->end_index = i;
			break;
		}
		
	}
	
	return sb;
}

void scanner_tokenize(FILE* source){
	
	sbuffer_t* sb = sbuffer_init(source);
	
	while(!sb->found_end){
		cstring_import_print(sb->buffer, BUFFER_SIZE);
		char c = fgetc(source);
		sbuffer_shift(sb, c);
	} 
	
	
	
	
	//32 space
	//13 newline RF
	//10 newline LF
	//9  tab
	
	/*
	cstring_import_print(sb->buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(sb, 'h');
	cstring_import_print(sb->buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(sb, 'e');
	cstring_import_print(sb->buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(sb, 'l');
	cstring_import_print(sb->buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(sb, 'l');
	cstring_import_print(sb->buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(sb, 'o');
	cstring_import_print(sb->buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(sb, ' ');
	cstring_import_print(sb->buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(sb, 'w');
	cstring_import_print(sb->buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(sb, 'o');
	cstring_import_print(sb->buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(sb, 'r');
	cstring_import_print(sb->buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(sb, 'l');
	cstring_import_print(sb->buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(sb, 'd');
	cstring_import_print(sb->buffer, BUFFER_SIZE);
	printf("\n");
	*/
	
	/*
	sbuffer_shift(&sb, 'e');
	sbuffer_shift(&sb, 'l');
	sbuffer_shift(&sb, 'l');
	*/
	
	
	/*
	while (1){
		char c = fgetc(source);
		//curr_char_ptr = 1;

		if(c == EOF){
		  break;
		}
		
		//printf("%d\n",c);
		
	}
	*/
}

void scanner_tokenize_file(char* file_name){
	
	FILE* f = fopen(file_name,"r");

	if (f == NULL){
		return;
	}

	scanner_tokenize(f);
	
	free(f);
}
