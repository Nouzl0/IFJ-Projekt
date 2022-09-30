#include <stdlib.h>

#include "lex.h"

int main(){
	
	token_array_t token_array;
	token_array_ctor(&token_array);
	
	lex_tokenize_file(&token_array,"./example.php");
	
	token_array_dtor(&token_array);
	
	return 0;
}