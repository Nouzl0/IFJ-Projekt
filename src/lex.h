#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "error_handler.h"
#include "strings_lib.h"
#include "token.h"

extern errors_t* global_err_ptr;

void lex_tokenize(tok_arr_t* ta_ptr, FILE* source);

void lex_tokenize_file(tok_arr_t* ta_ptr, char* file_name);
