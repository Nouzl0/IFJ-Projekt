#pragma once

#include <stdlib.h>

#include "error_handler.h"
#include "token.h"
#include "precedence_tree.h"
#include "statement_parser.h"

void parse_token_array(error_handler_t* eh_ptr, token_array_t tok_arr);

