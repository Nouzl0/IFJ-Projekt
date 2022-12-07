/**
 * Project: Implementace překladače imperativního jazyka IFJ22
 * 
 * @file code_generation.h
 * @brief Header file for end code generation
 * 
 * @author Nikolas Nosál <xnosal01>
 * @author Adam Mrkva <xmrkva04>
 * @author Rostislav Navrátil <xnavra72>
 * @author David Nevrlka <xnevrl00>
 */

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error_handler.h"
#include "syntax_tree.h"
#include "symtable.h"
#include "tokens_lib.h"
#include "stack_lib.h"

// main function
void generate_code(stx_node_t *AS_Tree);

// item type functions
void do_block(stx_node_t *AS_Tree, STList *symbol_table, bool is_func);
void do_assignexpr(stx_node_t* AS_Tree, STList *symbol_table);
void do_expr(stx_node_t* AS_Tree);
void do_retexpr(stx_node_t* AS_Tree);
void do_ifelse(stx_node_t* AS_Tree, STList *symbol_table);
void do_whileblock(stx_node_t* AS_Tree, STList *symbol_table);
void do_funcblock(stx_node_t* AS_Tree);

// AP_Tree functions
void arithmetic_print(expr_node_t* AP_Tree , char* assigned_var, bool reset);
void logic_print(expr_node_t* AP_Tree, STList *symbol_table, char* label, int label_num);
void func_print(expr_node_t* AP_Tree, char *token_content);

// stack functions
void print_stack(expr_node_t* AP_Tree, bool left_side);
void format_string_print(char *buffer);

// special while function
void while_check_def(stx_node_t* stx_while, STList *symbol_table);

// float retyping
void arithmetic_print_floatcheck(expr_node_t* AP_Tree, STList *symbol_table, bool reset);
void arithmetic_print_float_retype(expr_node_t* AP_Tree, STList *symbol_table);
void arithmetic_print_float_untype(STList *symbol_table);

// func call functions
int is_defined_func(expr_node_t* AP_Tree);
void func_reads(char *token_content);
void func_readi(char *token_content);
void func_readf(char *token_content);
void func_write(expr_node_t* AP_Tree);
void func_strlen(expr_node_t* AP_Tree, char *token_content);
void func_ord(expr_node_t* AP_Tree, char * token_content);
void func_chr(expr_node_t* AP_Tree, char* token_content);
void func_strval(expr_node_t* AP_Tree, char* token_content);
void func_substring(void);
void func_intval(void);
void func_floatval(void);