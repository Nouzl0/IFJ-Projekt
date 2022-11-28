#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "error_handler.h"
#include "precedence_tree.h"

ptree_item_t* expr_parse(tok_arr_t* tok_arr, token_type expr_end_type);