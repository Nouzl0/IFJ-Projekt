#include <stdlib.h>

#include "cstring.h"

#define TOKEN_ARRAY_BASE_SIZE 20

typedef enum {
	HEADER,         // <?php
	FOOTER,         // ?>
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
	SLASH,          // /
	DDOT,           // :
	
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
	
	IDENTIFIER,             // Nazev funkce
	VARIABLE,       // Nazev promene
	NUMBER,         // Obsah cele cislo 
	FRACTION,       // Obsah cisla s desetinou carou
	TEXT            // Obsah retezce
	
} token_type;


static const char *TOKEN_ENUM_STRINGS[] = {
    "HEADER",
	"FOOTER",
	"EQUAL",
	"NOT_EQUAL",
	"GREATER_EQUAL",
	"LESS_EQUAL",
	"GREATER",
	"LESS",
	"ASSIGN",
	"NEG",
	"LEFT_PAREN",
	"RIGHT_PAREN",
	"LEFT_BRACE",
	"RIGHT_BRACE",
	"COMMA",
	"DOT",
	"MINUS",
	"PLUS",
	"SEMICOLON",
	"STAR",
	"SLASH",
	"DDOT",
	"AND",
	"OR",
	"NIL",
	"IF",
	"ELSE",
	"WHILE",
	"FUNC",
	"RETURN",
	"INT",
	"FLOAT",
	"STRING",
	"IDENTIFIER",
	"VARIABLE",
	"NUMBER",
	"FRACTION",
	"TEXT"
};


typedef struct {
	char* match;
	token_type type;
} trecord_t;


//Token
typedef struct {
	token_type type;
	int line;
    cstring_t* content;
} token_t;

//Pole tokenu
typedef struct {
    int size;
    int len;
    token_t *elems;
} token_array_t;

//Vytváří token a přidává ho do pole tokenu
//int add_new_token(int line);

trecord_t keyword_register[11];

trecord_t symbol_register[22];

int token_compare_keywords(char* str_ptr, int* ttype_ptr);

int token_compare_symbol(char* str_ptr, int* ttype_ptr);

int token_array_ctor(token_array_t* token_array);

int token_array_add(token_array_t* ta_ptr, token_type type, int line, cstring_t* str_ptr);

void token_array_dtor(token_array_t* token_array);