#include "scanner.h"

void sbuffer_shift(sbuffer_t* sb, char c){
	for (int i = 0; i < BUFFER_SIZE - 1; i++){
		sb->buffer[i] = sb->buffer[i+1];
	}
	sb->buffer[BUFFER_SIZE - 1] = c;
}

void scanner_tokenize(FILE* source){
	
	sbuffer_t sb = {.buffer = { 0 } };
	
	
	sbuffer_shift(&sb, 'h');
	cstring_import_print(sb.buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(&sb, 'e');
	cstring_import_print(sb.buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(&sb, 'l');
	cstring_import_print(sb.buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(&sb, 'l');
	cstring_import_print(sb.buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(&sb, 'o');
	cstring_import_print(sb.buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(&sb, ' ');
	cstring_import_print(sb.buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(&sb, 'w');
	cstring_import_print(sb.buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(&sb, 'o');
	cstring_import_print(sb.buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(&sb, 'r');
	cstring_import_print(sb.buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(&sb, 'l');
	cstring_import_print(sb.buffer, BUFFER_SIZE);
	printf("\n");
	
	sbuffer_shift(&sb, 'd');
	cstring_import_print(sb.buffer, BUFFER_SIZE);
	printf("\n");
	
	
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
