#include <stdlib.h>

#define TOKEN_ARRAY_BASE_SIZE 20
#define TOKEN_BASE_SIZE 6

//Token
typedef struct {
    int size;
	int len;
	int line;
    char* content;
} token_t;

//Pole tokenu
typedef struct {
    int size;
    int len;
    token_t *elems;
} token_array_t;

//Vytváří token a přidává ho do pole tokenu
//int add_new_token(int line);

int token_array_ctor(token_array_t* token_array);