#include <stdlib.h>
#include <stdio.h>

#define BUFFER_SIZE 8

typedef struct {
	FILE* source;
	int found_end;
	int end_index;
	int line;
	void* ta_ptr;
	char buffer[BUFFER_SIZE];
} sbuffer_t;

/*
typedef struct {
    int size;
	int len;
    char* content;
} scanner_t;
*/


int is_char_number(char c);

int is_char_alpha(char c);

int is_char_capital(char c);

int is_char_valid_body_of_id(char c);

int is_char_letter(char c);

int is_char_variable_name(char c);

int is_char_permited(char c);

sbuffer_t* sbuffer_init(FILE* source);

void sbuffer_shift(sbuffer_t* sb);

void sbuffer_skip(sbuffer_t* sb, int to_skip);
