#pragma once

#include <stdlib.h>

#include "error_handler.h"
#include "precedence_tree.h"

int get_stmt_end_index(error_handler_t* eh_ptr, token_array_t tok_arr, int start_index, token_type ending, int delimiter);

ptree_item_t* parse_statement(error_handler_t* eh_ptr, token_array_t tok_arr, int start_index, int end_index);

