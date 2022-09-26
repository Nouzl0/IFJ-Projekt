#include <stdio.h>
#include <stdlib.h>

#include "string_builder.h"

int main() {
	
	
	string_builder_t* sb_ptr = string_builder_ctor();
	
	string_builder_add_char(sb_ptr,'h');
	string_builder_add_char(sb_ptr,'e');
	string_builder_add_char(sb_ptr,'l');
	string_builder_add_char(sb_ptr,'l');
	string_builder_add_char(sb_ptr,'o');
	string_builder_add_char(sb_ptr,' ');
	string_builder_add_char(sb_ptr,'w');
	string_builder_add_char(sb_ptr,'o');
	string_builder_add_char(sb_ptr,'r');
	string_builder_add_char(sb_ptr,'l');
	string_builder_add_char(sb_ptr,'d');
	
	string_builder_print(sb_ptr);
	
	string_builder_dtor(sb_ptr);
	
	
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