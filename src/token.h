#pragma once
#include <stdlib.h>
#include <string.h>

#include "error_handler.h"
#include "cstring.h"
#include "shift_buffer.h"

#define TOKEN_ARRAY_BASE_SIZE 20

/*

TODO: Pridat relativni a absolutni adresovani a pomocne funkce v poli tokenu
at je mozne s nim mnohem jednodusej pracova v parserech

*/

typedef enum {
	//Data types
	VOID,
	INT,
	FLOAT,
	STRING,
	
	//Terminals
	IDENTIFIER,     // Nazev funkce
	VARIABLE,       // Nazev promene
	NUMBER,         // Data celeho cisla 
	FRACTION,       // Data cisla s desetinou carou
	TEXT,           // Data retezce
	NIL,            // null
	
	//Non-Terminals
	NEG,            // !
	//Matika
	MINUS,          // -
	PLUS,           // +
	STAR,           // *
	SLASH,          // /
	DOT,            // .
	//Porovnavace
	TYPE_EQUAL,     // ===
	TYPE_NOT_EQUAL, // !==
	EQUAL,          // ==
	NOT_EQUAL,      // !=
	GREATER_EQUAL,  // >=
	LESS_EQUAL,     // <=
	GREATER,        // >
	LESS,           // <
	AND,            // and
	OR,             // or
	
	//Specialni znaky
	COMMA,          // ,
	ASSIGN,         // =
	LEFT_PAREN,     // (
	RIGHT_PAREN,    // )
	LEFT_BRACE,     // {
	RIGHT_BRACE,    // }
	SEMICOLON,      // ;
	DDOT,           // :
	
	//Klicove slova
	IF,             // if
	ELSE,           // else
	WHILE,          // while
	FUNC,           // function
	RETURN,         // return
	
	//Zbytek
	HEADER,         // <?php
	FOOTER          // ?>
	
} token_type;

typedef struct {
	char* match;
	token_type type;
} trecord_t;


//Token
typedef struct {
	token_type type;
	int line;
	int column;
    char* content;
} token_t;

//Pole tokenu
typedef struct {
    int size;
    int len;
    token_t *elems;
} token_array_t;


int token_compare_keywords(char* str_ptr, int* ttype_ptr);

int token_compare_symbol(char* str_ptr, int* ttype_ptr);

int token_array_ctor(token_array_t* token_array);

void token_array_add(token_array_t* ta_ptr, token_type type, int line, int colum, char* str_ptr);

void token_array_dtor(token_array_t* token_array);

char* token_enum_to_string(token_type type);

void token_array_debug_print(token_array_t token_array);