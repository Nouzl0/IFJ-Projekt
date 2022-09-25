#include <stdio.h>
#include <stdlib.h>

#include "string_builder.h"

int main() {
	
	
	string_builder_t* sb_ptr = string_builder_ctor();
	
	string_builder_dtor(sb_ptr);
	
	
	printf("Joq\n");
	

	FILE* f = fopen("./test2.php","r");


	while (1){
		char c = fgetc(f);
		//*curr_char_ptr = 1;

		if(c == EOF){
		  break;
		}
		
	printf("%d\n",c);
		
	}
	
	free(f);

	return 0;
}