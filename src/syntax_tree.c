#include "syntax_tree.h"

void stree_ctor(stree_item_t* stree_item_ptr){
	stree_item_ptr->type = BLOCK;
	stree_item_ptr->level = 0;
	stree_item_ptr->items_len = 0;
	stree_item_ptr->items = NULL;
	stree_item_ptr->stmt = NULL;
	
}

void stree_add_item(stree_item_t* stree_item_ptr, int count){
	
	
	
	//stree_item_ptr
}

stree_item_t* stree_add_block(){
	stree_item_t* stree_item_ptr = malloc(sizeof(stree_item_t));
	
	if (stree_item_ptr == NULL){
		return NULL;
	}
	
	stree_item_ptr->type = BLOCK;
	stree_item_ptr->level = 0;
	stree_item_ptr->items_size = 5;
	stree_item_ptr->items_len = 0;
	stree_item_ptr->stmt = NULL;
	stree_item_ptr->items = malloc(sizeof(stree_item_t*) * stree_item_ptr->items_size);
	
	if (stree_item_ptr->items == NULL){
		return NULL;
	}
	
	return stree_item_ptr;
}

void stree_inset_to_block(stree_item_t* stree_item_ptr, item_type type, int count){
	
}