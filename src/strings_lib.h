/**
 * Project: Implementace překladače imperativního jazyka IFJ22
 * 
 * @file strings_lib.h
 * @brief Header file for shift buffer and helping functions
 * 
 * @author Nikolas Nosál <xnosal01>
 * @author Adam Mrkva <xmrkva04>
 * @author Rostislav Navrátil <xnavra72>
 * @author David Nevrlka <xnevrl00>
 */

#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

bool is_char_number(char c);
bool is_char_alpha(char c);
bool is_char_capital(char c);
bool is_char_valid_body_of_id(char c);
bool is_char_letter(char c);
bool is_char_variable_name(char c);
bool is_char_permited(char c);
bool is_char_valid_hex(char c);
bool is_char_valid_octal(char c);

/*******************
	Shift buffer
********************/

#define BUFFER_SIZE 9

typedef struct {
	FILE* source;
	bool found_end;
	int end_index;
	int line;
	int column;
	//int exact_column;
	void* ta_ptr;
	char buffer[BUFFER_SIZE];
} sbuffer_t;

sbuffer_t* sbuffer_init(FILE* source);

void sbuffer_ctor(sbuffer_t* sb_ptr, FILE* source);

void sbuffer_shift(sbuffer_t* sb);

void sbuffer_skip(sbuffer_t* sb, int to_skip);


/*********************
	String builder
**********************/

#define STRING_BASE_SIZE 10

typedef struct {
	int size;
	int len;
	char* content;
} str_builder_t;

void str_builder_ctor(str_builder_t* cs_ptr);

void str_builder_append(str_builder_t* cs_ptr, char c);

bool str_builder_cmp(char* str_ptr1, char* str_ptr2);

bool str_cmp_files(char* file_name1, char* file_name2);

void str_ptrint_file(char* file_name);