#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "error_handler.h"
#include "precedence_tree.h"
//OLD
int get_stmt_end_index(error_handler_t* eh_ptr, tok_arr_t tok_arr, int start_index, token_type ending, int delimiter);

ptree_item_t* parse_statement(error_handler_t* eh_ptr, tok_arr_t tok_arr, int start_index, int end_index);
//NEW
ptree_item_t* expr_parse(tok_arr_t* tok_arr, token_type expr_end_type);