#pragma once

#include "error_handler.h"
#include "token.h"

typedef struct btree_item_t btree_item_t;

struct btree_item_t {
	int is_leaf;
	int precedence;
	char* data;
	btree_item_t* left;
	btree_item_t* right;
};


typedef struct {
	btree_item_t* root;
	btree_item_t* active;
	
} binary_tree_t;

int get_precedence_by_type(token_type type);

binary_tree_t* binary_tree_init();

//void binary_tree_add(binary_tree_t* bt_ptr, token_type operand, char id1 , char id2);

void binary_tree_fork(binary_tree_t* bt_ptr, token_type, char* data);

void binary_tree_fork_prec(binary_tree_t* bt_ptr, int cmp_prec, int save_prec, char* data);

void binary_tree_add_leaf(binary_tree_t* bt_ptr, char* data);

void binary_tree_to_json(binary_tree_t* bt_ptr);