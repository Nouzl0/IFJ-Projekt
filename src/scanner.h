#include <stdio.h>
#include <stdlib.h>

#include "string.h"
#include "token.h"

#define BUFFER_SIZE 8

typedef struct {
	FILE* source;
	int found_end;
	int end_index;
	char buffer[BUFFER_SIZE];
} sbuffer_t;

/*
typedef struct {
    int size;
	int len;
    char* content;
} scanner_t;
*/

void sbuffer_shift(sbuffer_t* sb);

sbuffer_t* sbuffer_init(FILE* source);

void scanner_tokenize(FILE* source);

void scanner_tokenize_file(char* file_name);
