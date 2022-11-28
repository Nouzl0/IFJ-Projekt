#pragma once

#include <stdlib.h>

#include "error_handler.h"
#include "precedence_tree.h"

typedef enum {
	ASSIGNEXPR,
	EXPR,
	RETEXPR,
	IFELSE,
	WHILEBLOCK,
	BLOCK,
	FUNCBLOCK,
	FUNCNAME,
	FUNCPARAMS,
	PARAMTYPE,
	PARAM
} item_type;


typedef struct stree_item_t stree_item_t;

struct stree_item_t {
	item_type type;
	token_t* token;
	int level;
	int items_size;
	int items_len;
	stree_item_t** items;
	ptree_item_t* stmt;
};

stree_item_t* stree_new_item(item_type type, int items_count);

stree_item_t* stree_new_block(int level);

void stree_insert_to_block(stree_item_t* st_block, stree_item_t* new_st_item);

void stree_insert_stmt(stree_item_t* st_block, ptree_item_t* prec_tree);

void stree_dtor(stree_item_t** stree);

void stree_json_debug_print(stree_item_t*);