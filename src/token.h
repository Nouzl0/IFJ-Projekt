#include <stdlib.h>

#include "cstring.h"

#define TOKEN_ARRAY_BASE_SIZE 20
#define TOKEN_BASE_SIZE 6

typedef enum {
	
	EQUAL,          // ==
	NOT_EQUAL,      // !=
	GREATER_EQUAL,  // >=
	LESS_EQUAL,     // <=
	GREATER,        // >
	LESS,           // <
	ASSIGN,         // =
	NEG,            // !
	LEFT_PAREN,     // (
	RIGHT_PAREN,    // )
	LEFT_BRACE,     // {
	RIGHT_BRACE,    // }
	COMMA,          // ,
	DOT,            // .
	MINUS,          // -
	PLUS,           // +
	SEMICOLON,      // ;
	STAR,           // *
	DOL,            // &
	SLASH,          // /
	
	
	AND, 
	OR,
	NIL,
	IF,
	ELSE,
	WHILE,
	FUNC,
	RETURN,
	INT,
	FLOAT,
	STRING,  
	
	IDENTIFIER    // Promena, jmeno funkce                
} token_type;

typedef struct {
	char* match;
	token_type type;
} trecord_t;


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

int compare_keywords(char* str_ptr);