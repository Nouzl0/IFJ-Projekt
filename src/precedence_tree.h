#pragma once

#include "error_handler.h"
#include "token.h"

typedef struct ptree_item_t ptree_item_t;

struct ptree_item_t {
	int is_terminal;
	int precedence;
	token_t token;
	int params_len;
	ptree_item_t** params;
	ptree_item_t* left;
	ptree_item_t* right;
};

typedef struct {
	ptree_item_t* root;
	ptree_item_t* active;
	
} ptree_t;

int get_precedence_by_type(token_type type);

void ptree_ctor(ptree_t* pt_ptr);

void ptree_add_branch(ptree_t* pt_ptr, token_t tok);

void ptree_add_branch_prec(ptree_t* pt_ptr, int cmp_prec, int save_prec, token_t tok);

void ptree_extend(ptree_t* pt_ptr, int cmp_prec, ptree_item_t* pt_item_ptr);

ptree_item_t* prec_tree_add_leaf(ptree_t* pt_ptr, token_t tok);

void ptree_dtor(ptree_item_t* ptree_item_ptr);

void ptree_debug_to_json(ptree_item_t* pt_ptr);