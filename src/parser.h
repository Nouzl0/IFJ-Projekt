#pragma once

#include <stdlib.h>

#include "error_handler.h"
#include "token.h"
#include "statement_parser.h"
#include "syntax_tree.h"

stree_item_t* parse_token_array(error_handler_t* eh_ptr, token_array_t tok_arr);

