#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "error_handler.h"
#include "expr_tree.h"

expr_node_t* expr_parse(tok_arr_t* tok_arr, token_type expr_end_type);