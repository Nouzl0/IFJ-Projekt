#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "error_handler.h"
#include "shift_buffer.h"
#include "string.h"
#include "token.h"

void lex_tokenize(error_handler_t* eh_ptr, token_array_t* ta_ptr, FILE* source);

void lex_tokenize_file(error_handler_t* eh_ptr, token_array_t* ta_ptr, char* file_name);
