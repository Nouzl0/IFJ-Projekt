#pragma once

#include "error_handler.h"
#include "token.h"

typedef struct ptree_item_t ptree_item_t;

struct ptree_item_t {
	int is_terminal;
	int precedence;
	char* data;
	ptree_item_t* left;
	ptree_item_t* right;
};


typedef struct {
	ptree_item_t* root;
	ptree_item_t* active;
	
} ptree_t;

int get_precedence_by_type(token_type type);

void ptree_ctor(ptree_t* pt_ptr);

void ptree_add_branch(ptree_t* pt_ptr, token_type, char* data);

void ptree_add_branch_prec(ptree_t* pt_ptr, int cmp_prec, int save_prec, char* data);

void ptree_extend(ptree_t* pt_ptr, int cmp_prec, ptree_item_t* pt_item_ptr);

void prec_tree_add_leaf(ptree_t* pt_ptr, char* data);

void ptree_debug_to_json(ptree_item_t* pt_ptr);