#include <stdio.h>
#include <stdlib.h>

#include "shift_buffer.h"
#include "string.h"
#include "token.h"

void lex_tokenize(token_array_t* ta_ptr, FILE* source);

void lex_tokenize_file(token_array_t* ta_ptr, char* file_name);
