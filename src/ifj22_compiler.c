#include <stdlib.h>

#include "lex.h"

int main(){
	
	token_array_t token_array;
	token_array_ctor(&token_array);
	
	lex_tokenize_file(&token_array,"./example.php");
	
	
	
	int line = 0;
	for (int i = 0; i < token_array.len; i++){
		
		if(line != token_array.elems[i].line){
			printf("\n");
			line = token_array.elems[i].line;
		}
		
		if (token_array.elems[i].content == NULL){
			printf("[%s,-]", TOKEN_ENUM_STRINGS[token_array.elems[i].type]);
		} else {
			printf("[%s,%s]", TOKEN_ENUM_STRINGS[token_array.elems[i].type], token_array.elems[i].content->content);
		}
		
	}
	printf("\n");
	
	
	
	
	token_array_dtor(&token_array);
	
	return 0;
}