/**
 * Project: Implementace překladače imperativního jazyka IFJ22
 * 
 * @file tokens_lib.h
 * @brief Header file for tokens array
 * 
 * @author Nikolas Nosál <xnosal01>
 * @author Adam Mrkva <xmrkva04>
 * @author Rostislav Navrátil <xnavra72>
 * @author David Nevrlka <xnevrl00>
 */

#pragma once
#include <stdlib.h>
#include <string.h>

#include "strings_lib.h"

#define TOKEN_ARRAY_BASE_SIZE 20

typedef enum {
	// Nullable Data types
	NIL_INT,
	NIL_FLOAT,
	NIL_STRING,
	
	// Data types
	VOID,
	INT,
	FLOAT,
	STRING,
	
// Terminals
	IDENTIFIER,     // Function name
	VARIABLE,       // Variable name
	NUMBER,         // Interger constant 
	FRACTION,       // Float constant
	TEXT,           // String constant
	NIL,            // null
	
// Non-Terminals
	// Basic math
	MINUS,          // -
	PLUS,           // +
	STAR,           // *
	SLASH,          // /
	DOT,            // .
	// Comparators
	TYPE_EQUAL,     // ===
	TYPE_NOT_EQUAL, // !==
	EQUAL,          // ==
	NOT_EQUAL,      // !=
	GREATER_EQUAL,  // >=
	LESS_EQUAL,     // <=
	GREATER,        // >
	LESS,           // <
	
	// Other symbols
	COMMA,          // ,
	ASSIGN,         // =
	LEFT_PAREN,     // (
	RIGHT_PAREN,    // )
	LEFT_BRACE,     // {
	RIGHT_BRACE,    // }
	SEMICOLON,      // ;
	DDOT,           // :
	
	// Keywords
	IF,             // if
	ELSE,           // else
	WHILE,          // while
	FUNC,           // function
	RETURN,         // return
	
	// Others
	HEADER,         // <?php
	FOOTER          // ?>
	
} token_type;

typedef struct {
	char* match;
	token_type type;
} trecord_t;

typedef struct {
	token_type type;
	int line;
	int column;
    char* content;
} token_t;

typedef struct {
    int size;
    int len;
	int index;
    token_t *elems;
} tok_arr_t;


int token_compare_keywords(char* str_ptr, int* ttype_ptr);

int token_compare_symbol(char* str_ptr, int* ttype_ptr);

int tok_arr_ctor(tok_arr_t* token_array);

void tok_arr_insert(tok_arr_t* ta_ptr, token_type type, int line, int colum, char* str_ptr);

void tok_arr_dtor(tok_arr_t* token_array);

bool tok_arr_on_end(tok_arr_t* ta_ptr);

bool tok_arr_cmp(tok_arr_t* ta_ptr, token_type type);

bool tok_arr_cmp_arr(tok_arr_t* ta_ptr, token_type* types, int types_len);

bool tok_arr_cmp_range(tok_arr_t* ta_ptr, token_type start_type, token_type end_type);

bool tok_arr_cmp_offset(tok_arr_t* ta_ptr, token_type type, int offset);

bool tok_arr_cmp_skip(tok_arr_t* ta_ptr, token_type type);

void tok_arr_inc(tok_arr_t* ta_ptr, int value);

int tok_arr_get_commas(tok_arr_t* ta_ptr);

token_t* tok_arr_get(tok_arr_t* ta_ptr);

token_t* tok_arr_get_next(tok_arr_t* ta_ptr);

token_t* tok_arr_get_offset(tok_arr_t* ta_ptr, int offset);

char* token_enum_to_string(token_type type);

void tok_arr_debug_print(tok_arr_t token_array);
