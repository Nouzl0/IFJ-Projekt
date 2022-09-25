#include <stdlib.h>

#define STRING_BASE_SIZE 10

typedef struct {
    int size;
	int len;
    char* content;
} string_builder_t;

string_builder_t* string_builder_ctor();

void string_builder_add_char(string_builder_t* sb_ptr, char c);

void string_builder_dtor(string_builder_t* sb_ptr);