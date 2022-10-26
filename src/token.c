#include "token.h"

#define KEYWORD_REGISTER_LENGTH 12
trecord_t keyword_register[KEYWORD_REGISTER_LENGTH] = {
	{"and", AND}, 
	{"or", OR},
	{"null", NIL},
	{"if", IF},
	{"else", ELSE},
	{"while", WHILE},
	{"function", FUNC},
	{"return", RETURN},
	{"int", INT},
	{"float", FLOAT},
	{"string", STRING},
	{"void", VOID}
};

#define SYMBOL_REGISTER_LENGTH 22
trecord_t symbol_register[SYMBOL_REGISTER_LENGTH] = {
	{"<?php", HEADER},
	{"?>", FOOTER},
	{"==", EQUAL},
	{"!=", NOT_EQUAL},
	{">=", GREATER_EQUAL},
	{"<=", LESS_EQUAL},
	{">", GREATER},
	{"<", LESS},
	{"=", ASSIGN},
	{"!", NEG},
	{"(", LEFT_PAREN},
	{")", RIGHT_PAREN},
	{"{", LEFT_BRACE},
	{"}", RIGHT_BRACE},
	{",", COMMA},
	{".", DOT},
	{"-", MINUS},
	{"+", PLUS},
	{";", SEMICOLON},
	{"*", STAR},
	{"/", SLASH},
	{":", DDOT}
};


/**
 * Slouzi k porovnani retezce s nejakym registrem
 * 
 * @param reg Registr ve kterem se ma porovnavat
 * @param reg_len Delak registru ve kterem se ma porovnavat
 * @param str_ptr Ukazatel na retezec ktery se ma porovnat
 * @param ttype_ptr Ukazatel do ktereho se zapise index nalezeneho retezce v registru
 * 
 * @returns 0 kdyz retezec nebylo nalezeno jinak delku nalezeneho retezce
 */
int token_compare(trecord_t* reg, int reg_len, char* str_ptr, int* ttype_ptr){
	
	for (int i = 0; i < reg_len; i++){
		if(cstring_compare(reg[i].match,str_ptr)){
			*ttype_ptr = reg[i].type;
			return cstring_get_length(reg[i].match); 
		}
	}
	return 0;	
}

/**
 * Slouzi k zjisteni jestli je retezec klicove slovo nebo ne
 * 
 * @param str_ptr Ukazatel na retezec ktery se ma porovnat
 * @param ttype_ptr Ukazatel do ktereho se zapise index nalezeneho klicoveho slova v registru
 * 
 * @returns 0 kdyz klicove slovo nebylo nalezeno jinak delku klicoveho slova
 */
int token_compare_keywords(char* str_ptr, int* ttype_ptr){
	return token_compare(keyword_register,KEYWORD_REGISTER_LENGTH,str_ptr,ttype_ptr);
}

/**
 * Slouzi k zjisteni jestli je retezec symbol nebo ne
 * 
 * @param str_ptr Ukazatel na retezec ktery se ma porovnat
 * @param ttype_ptr Ukazatel do ktereho se zapise index nalezeneho symbolu v registru
 * 
 * @returns 0 kdyz symbol nebyl nalezen jinak delku symbolu
 */
int token_compare_symbol(char* str_ptr, int* ttype_ptr){
	return token_compare(symbol_register,SYMBOL_REGISTER_LENGTH,str_ptr,ttype_ptr);
}


/**
 * Inicializuje nove pole tokenu
 * 
 * @param ta_ptr Ukazatel na prave vytvorene pole tokenu
 * 
 * @returns 1 kdyz se nepovede alokace jinak 0
 */
int token_array_ctor(token_array_t* ta_ptr){
	
	token_t* tokens = malloc(sizeof(token_t) * TOKEN_ARRAY_BASE_SIZE); 
	
	if(tokens == NULL){
		return 1;
	}
	
	ta_ptr->size = TOKEN_ARRAY_BASE_SIZE;
	ta_ptr->len = 0;
	ta_ptr->elems = tokens;
	return 0;	
}


/**
 * Pridava do pole tokenu novy token
 * 
 * @param sb_ptr Ukazatel na posuvny buffer ktery uchovava zbytek dalsi potrebne data
 * @param token_type Typ tokenu
 * @param str_ptr Ukazatel na obash tokenu je li potreba jinak null
 */
void token_array_add(token_array_t* ta_ptr, token_type type, int line, int column, cstring_t* str_ptr){
	
	if(ta_ptr->len >= ta_ptr->size){
		ta_ptr->elems = realloc(ta_ptr->elems, ta_ptr->size * 2 * sizeof(token_t));
		
		if (ta_ptr->elems == NULL){
			handle_program_error();
		}
		
		ta_ptr->size =  ta_ptr->size * 2;
	}
	
	token_t token = {
		.type = type,
		.line = line,
		.column = column,
		.content = str_ptr
	};
	
	ta_ptr->elems[ta_ptr->len] = token;
	ta_ptr->len++;
	
}


/**
 * Uvolnuje pamet zabranou polem tokenu
 * 
 * @param ta_ptr Ukazatel na pole tokenu
 */
void token_array_dtor(token_array_t* ta_ptr){
	for (int i = 0; i < ta_ptr->len; i++){
		//TODO: call dtor on ta_ptr->elems[i].content
		if(ta_ptr->elems[i].content != NULL){
			cstring_dtor(ta_ptr->elems[i].content);
		}
	
	}
	free(ta_ptr->elems);
}


char* token_debug_get_string(token_type type){
	static char *TOKEN_ENUM_STRINGS[] = {
		//Data types
		"VOID",
		"INT",
		"FLOAT",
		"STRING",
		
		//Terminals
		"IDENTIFIER",
		"VARIABLE",
		"NUMBER",
		"FRACTION",
		"TEXT",
		"NIL",
		
		//Non-Terminals
		"NEG",
		//Matika
		"MINUS",
		"PLUS",
		"STAR",
		"SLASH",
		"DOT",
		//Porovnavace
		"EQUAL",
		"NOT_EQUAL",
		"GREATER_EQUAL",
		"LESS_EQUAL",
		"GREATER",
		"LESS",
		"AND",
		"OR",
		
		//Specialni znaky
		"COMMA",
		"ASSIGN",
		"LEFT_PAREN",
		"RIGHT_PAREN",
		"LEFT_BRACE",
		"RIGHT_BRACE",
		"SEMICOLON",
		"DDOT",
		
		//Klicove slova
		"IF",
		"ELSE",
		"WHILE",
		"FUNC",
		"RETURN",
		
		//Zbytek
		"HEADER",
		"FOOTER"
	};
	return TOKEN_ENUM_STRINGS[type];
}

void token_array_debug_print(token_array_t ta_ptr){
	
	printf("Vypisuju tokeny ve formatu [type,obsah,sloupec,radek]\n");
	int line = 0;
	for (int i = 0; i < ta_ptr.len; i++){
		
		if(line != ta_ptr.elems[i].line){
			printf("\n");
			line = ta_ptr.elems[i].line;
			printf("%d\t|",line);
		}
		
		if (ta_ptr.elems[i].content == NULL){
			printf("[%s,-,%d,%d]", token_debug_get_string(ta_ptr.elems[i].type),ta_ptr.elems[i].column,ta_ptr.elems[i].line);
		} else {
			printf("[%s,%s,%d,%d]", token_debug_get_string(ta_ptr.elems[i].type), ta_ptr.elems[i].content->content,ta_ptr.elems[i].column,ta_ptr.elems[i].line);
		}
		
	}
	printf("\n");
}