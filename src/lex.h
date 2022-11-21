#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "error_handler.h"
#include "strings_lib.h"
#include "tokens_lib.h"

void lex_tokenize(tok_arr_t* ta_ptr, FILE* source);

void lex_tokenize_file(tok_arr_t* ta_ptr, char* file_name);
