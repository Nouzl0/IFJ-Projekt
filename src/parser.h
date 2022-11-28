#pragma once

#include <stdlib.h>

#include "error_handler.h"
#include "tokens_lib.h"
#include "statement_parser.h"
#include "syntax_tree.h"

stree_item_t* parser_build_all(tok_arr_t* ta_ptr);