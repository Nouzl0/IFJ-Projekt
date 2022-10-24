#pragma once

#include <stdlib.h>

#include "error_handler.h"
#include "precedence_tree.h"

int get_stmt_end_index(token_array_t tok_arr, int start_index, token_type ending);

ptree_item_t* parse_statement(token_array_t tok_arr, int start_index, int end_index);

