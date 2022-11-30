#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "error_handler.h"
#include "syntax_tree.h"
#include "tokens_lib.h"

// checks if AP_Tree token is number terminal, may be changed
#define is_assignstmt_terminal(AP_Tree) ((AP_Tree->token.type == NUMBER) || (AP_Tree->token.type == VARIABLE) || (AP_Tree->token.type == FRACTION))

// main function
void generate_code(stx_node_t *AS_Tree);

// item type functions
void do_assignstmt(stx_node_t* AS_Tree);
void do_ifelse(stx_node_t* AS_Tree);
void do_whileblock(stx_node_t* AS_Tree);

// AP_Tree functions
void arithmetic_print(expr_node_t* AP_Tree , char* assigned_var, bool reset);
void logic_print(expr_node_t* AP_Tree, char* label, int label_counter);