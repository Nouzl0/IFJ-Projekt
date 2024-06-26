/**
 * Project: Implementace překladače imperativního jazyka IFJ22
 * 
 * @file syntax_tree.h
 * @brief Header file for syntax tree
 * 
 * @author Nikolas Nosál <xnosal01>
 * @author Adam Mrkva <xmrkva04>
 * @author Rostislav Navrátil <xnavra72>
 * @author David Nevrlka <xnevrl00>
 */

#pragma once

#include <stdlib.h>

#include "error_handler.h"
#include "expr_tree.h"
	

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


typedef struct stx_node_t stx_node_t;

struct stx_node_t {
	item_type type;
	token_t* token;
	int level;
	int items_size;
	int items_len;
	stx_node_t** items;
	expr_node_t* expr;
};

stx_node_t* stx_node_new(item_type type, int items_count);

stx_node_t* stx_node_new_block(int level);

void stx_node_insert_item(stx_node_t* sn_ptr, stx_node_t* new_sn_node);

void stx_node_insert_expr(stx_node_t* sn_ptr, expr_node_t* expr_root_ptr);

void stx_node_dtor(stx_node_t* stree);

char* stx_node_type_to_string(item_type type);

void stx_tree_to_json(stx_node_t* sn_ptr);