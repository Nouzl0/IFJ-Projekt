#include <stdlib.h>
#include <stdio.h>

#define STRING_BASE_SIZE 10

typedef struct {
	int size;
	int len;
	char* content;
} cstring_t;

cstring_t* cstring_ctor();

void cstring_dtor(cstring_t* cs_ptr);

void cstring_add_char(cstring_t* cs_ptr, char c);

void cstring_print(cstring_t* cs_ptr);

cstring_t* cstring_import(char* str_ptr, int len);

void cstring_import_print(char* str_ptr, int len);