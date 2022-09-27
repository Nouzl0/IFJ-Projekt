#include <stdio.h>
#include <stdlib.h>

#include "cstring.h"

int main() {
	
	cstring_t* cs_ptr = cstring_ctor();
	
	cstring_add_char(cs_ptr,'h');
	cstring_add_char(cs_ptr,'e');
	cstring_add_char(cs_ptr,'l');
	cstring_add_char(cs_ptr,'l');
	cstring_add_char(cs_ptr,'o');
	cstring_add_char(cs_ptr,' ');
	cstring_add_char(cs_ptr,'w');
	cstring_add_char(cs_ptr,'o');
	cstring_add_char(cs_ptr,'r');
	cstring_add_char(cs_ptr,'l');
	cstring_add_char(cs_ptr,'d');
	
	cstring_print(cs_ptr);
	
	cstring_dtor(cs_ptr);
	
	
	/*

	FILE* f = fopen("./example.php","r");

	if (f == NULL){
		return 1;
	}


	while (1){
		char c = fgetc(f);
		//curr_char_ptr = 1;

		if(c == EOF){
		  break;
		}
		
	printf("%d\n",c);
		
	}
	
	free(f);


	*/
	
	
	return 0;
}