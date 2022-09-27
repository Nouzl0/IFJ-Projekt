#include <stdio.h>
#include <stdlib.h>

#include "cstring.h"

#define BUFFER_SIZE 8

typedef struct {
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

void scanner_tokenize(FILE* source);

void scanner_tokenize_file(char* file_name);