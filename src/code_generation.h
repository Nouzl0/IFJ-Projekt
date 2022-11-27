#include <stdio.h>
#include <stdlib.h>

#include "error_handler.h"
#include "syntax_tree.h"
#include "tokens_lib.h"

// checks if AP_Tree token is number terminal, may be changed
#define is_assignstmt_terminal(AP_Tree) ((AP_Tree->token.type == NUMBER) || (AP_Tree->token.type == VARIABLE) || (AP_Tree->token.type == FRACTION))

void generate_code(stree_item_t *AS_Tree);
void do_assignstmt(stree_item_t* AS_Tree);
void assignstmt_print(ptree_item_t* AP_Tree, char* assigned_var);

const char assignstmt_set[5][5] = { "SUB", "ADD", "MUL", "DIV", };