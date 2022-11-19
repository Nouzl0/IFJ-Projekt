#include <stdlib.h>
#include <stdio.h>

#define STRING_BASE_SIZE 10

typedef struct {
	int size;
	int len;
	char* content;
} cstring_t;

void cstring_ctor(cstring_t* cs_ptr);

void cstring_add_char(cstring_t* cs_ptr, char c);

int cstring_compare(char* str_ptr1, char* str_ptr2);
