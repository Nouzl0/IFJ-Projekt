#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "error_handler.h"
#include "lex_string_helper.h"
#include "string.h"
#include "token.h"

extern errors_t* global_err_ptr;

void lex_tokenize(token_array_t* ta_ptr, FILE* source);

void lex_tokenize_file(token_array_t* ta_ptr, char* file_name);
