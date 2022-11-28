#pragma once

#include "error_handler.h"
#include "tokens_lib.h"

typedef struct expr_node_t expr_node_t;

struct expr_node_t {
	int is_terminal;
	int precedence;
	token_t token;
	int params_len;
	expr_node_t** params;
	expr_node_t* left;
	expr_node_t* right;
};

typedef struct {
	expr_node_t* root;
	expr_node_t* active;
	
} expr_tree_t;

int get_precedence_by_type(token_type type);

void expr_tree_ctor(expr_tree_t* et_ptr);

void expr_tree_add_branch(expr_tree_t* et_ptr, token_t tok);

void expr_tree_add_branch_prec(expr_tree_t* et_ptr, int cmp_prec, int save_prec, token_t tok);

void expr_tree_extend(expr_tree_t* et_ptr, int cmp_prec, expr_node_t* new_node_ptr);

expr_node_t* expr_tree_add_leaf(expr_tree_t* et_ptr, token_t tok);

void expr_node_dtor(expr_node_t* node_ptr);

void expr_node_print(expr_node_t* et_ptr);