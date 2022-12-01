#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error_handler.h"
#include "syntax_tree.h"
#include "tokens_lib.h"

// checks if AP_Tree token is number terminal, may be changed
#define both_terminal(AP_Tree) (((AP_Tree->left->is_terminal == true) && (AP_Tree->right->is_terminal == true)) && ((AP_Tree->left->token.type != IDENTIFIER) && (AP_Tree->right->token.type != IDENTIFIER)))
#define both_nonterminal(AP_Tree) (((AP_Tree->left->is_terminal == false) && (AP_Tree->right->is_terminal == false)) || ((AP_Tree->left->token.type == IDENTIFIER) && (AP_Tree->right->token.type == IDENTIFIER)))
#define left_terminal(AP_Tree) (((AP_Tree->left->is_terminal == true) && (AP_Tree->right->is_terminal == false)) && (AP_Tree->left->token.type != IDENTIFIER))
#define right_terminal(AP_Tree) (((AP_Tree->left->is_terminal == false) && (AP_Tree->right->is_terminal == true)) && (AP_Tree->right->token.type != IDENTIFIER))

// main function
void generate_code(stx_node_t *AS_Tree);

// item type functions
void do_assignstmt(stx_node_t* AS_Tree);
void do_expr(stx_node_t* AS_Tree);
void do_retexpr(stx_node_t* AS_Tree);
void do_ifelse(stx_node_t* AS_Tree);
void do_whileblock(stx_node_t* AS_Tree);
void do_funcblock(stx_node_t* AS_Tree);

// AP_Tree functions
void arithmetic_print(expr_node_t* AP_Tree , char* assigned_var, bool reset);
void logic_print(expr_node_t* AP_Tree, char* label, int label_counter);
void func_print(expr_node_t* AP_Tree);

// func call functions