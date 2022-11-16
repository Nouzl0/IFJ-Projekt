#include "syntax_tree.h"

stree_item_t* stree_new_item(item_type type, int items_count){
	stree_item_t* new_item = malloc(sizeof(stree_item_t));
	
	
	if (new_item == NULL){
		return NULL;
	}
	
	new_item->type = type;
	new_item->token = NULL;
	new_item->level = 0;
	new_item->items_size = items_count;
	new_item->items_len = 0;
	new_item->stmt = NULL;
	new_item->items = NULL;
	if (items_count){
		new_item->items = malloc(sizeof(stree_item_t*) * new_item->items_size);
	}
	
	return new_item;
}

void stree_insert_to_block(stree_item_t* st_block, stree_item_t* new_st_item){
	if(st_block->items_len >= st_block->items_size){
		st_block->items_size = st_block->items_size * 2;
		st_block->items = realloc(st_block->items, sizeof(stree_item_t*) * st_block->items_size);
	}
	
	st_block->items[st_block->items_len] = new_st_item;
	st_block->items_len++;
}


stree_item_t* stree_new_block(int level){
	stree_item_t* new_block = stree_new_item(BLOCK, 5);
	new_block->level = level;
	return new_block;
}

void stree_insert_stmt(stree_item_t* st_block, ptree_item_t* prec_tree){
	stree_item_t* stmt_item = stree_new_item(STMT,0);
	stmt_item->level = st_block->level;
	stmt_item->stmt = prec_tree;
	stree_insert_to_block(st_block,stmt_item);
}


void stree_dtor(stree_item_t** stree){
	if(!(*stree)){
		return;
	}
	
	if((*stree)->stmt){
		ptree_dtor((*stree)->stmt);
	}
	if((*stree)->items){
		for (int i = 0; i < (*stree)->items_len; i++){
			stree_dtor(&(*stree)->items[i]);
		}
		free((*stree)->items);
		(*stree)->items = NULL;
	}
	free((*stree));
	(*stree) = NULL;
}


char* stree_item_type_to_string(item_type type){
	static char *ITEM_ENUM_STRINGS[] = {
		"BLOCK",       // Blok kodu
		"FUNCBLOCK",   // Deklarace funkce
		"FUNCNAME",    //
		"FUNCPARAMS",  // 
		"PARAMTYPE",   //
		"PARAM",       //
		"ASSIGNSTMT",  // Prirazeni vyrazu do promene
		"STMT",        // Samotny vyraz
		"RETSTMT",     // Return s vyrazem
		"IFELSE",      // Vetveni pomoci IF a ELSE ktere je brano jako celek
		"WHILEBLOCK"   // Obash while blocku i s podminkou
	};
	return ITEM_ENUM_STRINGS[type];
}

void stree_json_debug_print(stree_item_t* st_item){
	
	if(!st_item){
		printf("null");
		return;
	}
	
	printf("{");
	printf("\"type\":\"%s\",", stree_item_type_to_string(st_item->type));
	printf("\"level\":\"%d\"",st_item->level);
	
	if(st_item->token){
		printf(",\"token\":{");
		printf("\"type\": \"%s\"", token_debug_get_string(st_item->token->type));
		
		if(st_item->token->content){
			printf(",\"content\": \"%s\"", st_item->token->content->content);
		}
		
		printf("}");
	}
	
	
	printf(",\"items_len\":\"%d\"",st_item->items_len);
	if(st_item->items_len){
		printf(",\"items\": [");
		for (int i = 0; i < st_item->items_len; i++){
			//printf("\n --- %d --- \n",i);
			stree_json_debug_print(st_item->items[i]);
			if(i != st_item->items_len-1){
				printf(",");
			}
		}
		printf("]");
	}
	
	if(st_item->stmt){
		printf(",");
		printf("\"stmt\":");
		recursive_print(st_item->stmt);
	}
	
	printf("}");
	
	
}