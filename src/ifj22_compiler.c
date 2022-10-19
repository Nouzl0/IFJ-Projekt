#include <stdlib.h>

#include "lex.h"

#include "binary_tree.h"

int main(){

	//Init
	error_handler_t error_handler;
	error_handler_ctor(&error_handler);
	
	token_array_t token_array;
	token_array_ctor(&token_array);
	
	//lex_tokenize_file(&error_handler, &token_array,"./example.php");
	
	handle_lex_error(error_handler);
	
	
	
	//token_array_print(token_array);
	
	
	binary_tree_t* bt_ptr = binary_tree_init();
	
	binary_tree_add_leaf(bt_ptr,"a");
	binary_tree_add_branch(bt_ptr,STAR,"*");
	binary_tree_add_leaf(bt_ptr,"b");
	binary_tree_add_branch(bt_ptr,PLUS,"+");
	binary_tree_add_leaf(bt_ptr,"c");
	binary_tree_add_branch(bt_ptr,STAR,"*");
	binary_tree_add_leaf(bt_ptr,"d");
	binary_tree_add_branch(bt_ptr,EQUAL,"==");
	binary_tree_add_leaf(bt_ptr,"e");
	binary_tree_add_branch(bt_ptr,STAR,"*");
	binary_tree_add_leaf(bt_ptr,"f");
	binary_tree_add_branch(bt_ptr,PLUS,"+");
	binary_tree_add_leaf(bt_ptr,"g");
	binary_tree_add_branch(bt_ptr,STAR,"*");
	binary_tree_add_leaf(bt_ptr,"h");
	binary_tree_add_branch(bt_ptr,MINUS,"-");
	binary_tree_add_leaf(bt_ptr,"i");
	binary_tree_add_branch(bt_ptr,STAR,"*");
	binary_tree_add_leaf(bt_ptr,"j");
	
	
	binary_tree_to_json(bt_ptr);
	
	
	
	int i = 1;
	while(i < token_array.len){
		
		
		//token_array.elems[i].type;
		
		//printf("Typec: %d\n", token_array.elems[i].type);
		i++;
	}
	

	
	
	
	
	token_array_dtor(&token_array);
	
	
	
	return 0;
}