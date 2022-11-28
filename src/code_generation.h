#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "error_handler.h"
#include "syntax_tree.h"
#include "tokens_lib.h"

// checks if AP_Tree token is number terminal, may be changed
#define is_assignstmt_terminal(AP_Tree) ((AP_Tree->token.type == NUMBER) || (AP_Tree->token.type == VARIABLE) || (AP_Tree->token.type == FRACTION))

void generate_code(stx_node_t *AS_Tree);
void do_assignstmt(stx_node_t* AS_Tree);
void assignstmt_print(expr_node_t* AP_Tree , char* assigned_var, bool reset);