#include "syntax_tree.h"


/**
 * Allocates memory for new syntax node and for child node pointers
 * if needed 
 * 
 * @param type Type of syntax new node
 * @param items_count Number of child nodes 
 * @returns Pointer to new syntax node if allocation was successful otherwise NULL
 */
stx_node_t* stx_node_new(item_type type, int items_count){
	stx_node_t* new_item = malloc(sizeof(stx_node_t));
	
	
	if (new_item == NULL){
		return NULL;
	}
	
	new_item->type = type;
	new_item->token = NULL;
	new_item->level = 0;
	new_item->items_size = items_count;
	new_item->items_len = 0;
	new_item->expr = NULL;
	new_item->items = NULL;
	if (items_count){
		new_item->items = malloc(sizeof(stx_node_t*) * new_item->items_size);
	}
	
	return new_item;
}


/**
 * Inserts syntax node to parent syntax node
 * Makes sure that there is space allocated for the pointers 
 * 
 * @param sn_ptr Pointer to parent node
 * @param new_sn_ptr Pointer to node for insert
 */
void stx_node_insert_item(stx_node_t* sn_ptr, stx_node_t* new_sn_ptr){
	if(sn_ptr->items_len >= sn_ptr->items_size){
		sn_ptr->items_size = sn_ptr->items_size * 2;
		sn_ptr->items = realloc(sn_ptr->items, sizeof(stx_node_t*) * sn_ptr->items_size);
	}
	
	sn_ptr->items[sn_ptr->items_len] = new_sn_ptr;
	sn_ptr->items_len++;
}


/**
 * Helper function for calling stx_node_new with predefined
 * values wich are suitable for code block node
 * 
 * @param level Value representing how many parent
 * code block nodes is above new node
 */
stx_node_t* stx_node_new_block(int level){
	stx_node_t* new_block = stx_node_new(BLOCK, 5);
	new_block->level = level;
	return new_block;
}


/**
 * Helper function for calling stx_node_new for creating
 * syntax node that only holds statement
 * At the end new syntax node is inserted in given parent node
 * 
 * @param sn_ptr Pointer to parent node
 * @param expr_root_node Pointer to expression node 
 */
void stx_node_insert_expr(stx_node_t* sn_ptr, expr_node_t* expr_root_node){
	stx_node_t* stmt_item = stx_node_new(EXPR,0);
	stmt_item->level = sn_ptr->level;
	stmt_item->expr = expr_root_node;
	stx_node_insert_item(sn_ptr,stmt_item);
}


/**
 * Frees memory allocated by syntax node
 * Calls itself recursively on node items
 * 
 * @param stree Pointer to root syntax node
 */
void stx_node_dtor(stx_node_t* stree){
	if(!stree){
		return;
	}
	
	if(stree->expr){
		expr_node_dtor(stree->expr);
	}
	if(stree->items){
		for (int i = 0; i < stree->items_len; i++){
			stx_node_dtor(stree->items[i]);
		}
		free(stree->items);
		stree->items = NULL;
	}
	free(stree);
}


/**
 * Frees memory allocated by syntax node
 * Calls itself recursively on node items
 * 
 * @param stree Pointer to root syntax node
 * @returns String reprezentation of enum of item type
 */
char* stx_node_type_to_string(item_type type){
	static char *NODE_TYPE_ENUM_STRINGS[] = {
		"ASSIGNEXPR",
		"EXPR",
		"RETEXPR",
		"IFELSE",
		"WHILEBLOCK",
		"BLOCK",
		"FUNCBLOCK",
		"FUNCNAME",
		"FUNCPARAMS",
		"PARAMTYPE",
		"PARAM"
	};
	return NODE_TYPE_ENUM_STRINGS[type];
}


/**
 * Prints out info about given syntax node
 * Calls itself recursively on node items
 *
 * @param sn_ptr Pointer to root syntax node
 */
void stx_node_print(stx_node_t* sn_ptr){
	
	if(!sn_ptr){
		printf("null");
		return;
	}
	
	printf("{");
	printf("\"type\":\"%s\",", stx_node_type_to_string(sn_ptr->type));
	printf("\"level\":\"%d\"",sn_ptr->level);
	
	if(sn_ptr->token){
		printf(",\"token\":{");
		printf("\"type\": \"%s\"", token_enum_to_string(sn_ptr->token->type));
		
		if(sn_ptr->token->content){
			printf(",\"content\": \"%s\"", sn_ptr->token->content);
		}
		
		printf("}");
	}
	
	
	printf(",\"items_len\":\"%d\"",sn_ptr->items_len);
	if(sn_ptr->items_len){
		printf(",\"items\": [");
		for (int i = 0; i < sn_ptr->items_len; i++){
			//printf("\n --- %d --- \n",i);
			stx_node_print(sn_ptr->items[i]);
			if(i != sn_ptr->items_len-1){
				printf(",");
			}
		}
		printf("]");
	}
	
	if(sn_ptr->expr){
		printf(",");
		printf("\"expr\":");
		expr_node_print(sn_ptr->expr);
	}
	
	printf("}");
	
	
}


/**
 * Helper function to call stx_node_print on given root node
 * Prints out newlines and spacers
 *
 * @param sn_ptr Pointer to root syntax node
 */
void stx_tree_to_json(stx_node_t* sn_ptr){
	printf("\n---------------------\n");
	stx_node_print(sn_ptr);
	printf("\n------------------\n");
}