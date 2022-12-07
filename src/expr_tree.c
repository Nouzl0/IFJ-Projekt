#include "expr_tree.h"


/**
 * Returns precedence value by given token type of operator
 *
 * @param type Token type of operator
 * @returns 
 */
#define PRECEDENCE_TABLE_SIZE 12
int get_precedence_by_type(token_type type){
	token_type arr1[PRECEDENCE_TABLE_SIZE] = {
		STAR, SLASH,
		PLUS, MINUS, DOT,
		GREATER, LESS, EQUAL, GREATER_EQUAL, LESS_EQUAL,
		TYPE_EQUAL, TYPE_NOT_EQUAL
		};
	
	int arr2[PRECEDENCE_TABLE_SIZE] = {
		2, 2,
		3, 3, 3,
		4, 4, 4, 4, 4,
		5, 5
	};
	
	for (unsigned int i = 0; i < PRECEDENCE_TABLE_SIZE; i++){
		if(type == arr1[i]){
			return arr2[i];
		}
	}
	return 0;
	
}


/**
 * Expression tree constructor 
 *
 * @param et_ptr Pointer to expression tree
 */
void expr_tree_ctor(expr_tree_t* et_ptr){
	et_ptr->root = NULL;
	et_ptr->active = NULL;
}


/**
 * Inserts new expression node to expression tree
 * Node should be operator or sub tree
 *
 * @param et_ptr Pointer to expression tree that is inserted into
 * @param cmp_prec Value of precedence which is used
 * for finding place for new node
 * @param new_node_ptr Pointer to node that is inserted
 */
void expr_tree_extend(expr_tree_t* et_ptr, int cmp_prec, expr_node_t* new_node_ptr){
	// First node in tree
	if (et_ptr->active == NULL){
		if(et_ptr->root != NULL){
			new_node_ptr->left = et_ptr->root;
		}
		et_ptr->root = new_node_ptr;
		et_ptr->active = new_node_ptr;
		return;
	}
	
	// Node is inserted first because there is no other node with lower precedence
	if(cmp_prec >= et_ptr->root->precedence){
		new_node_ptr->left = et_ptr->root;
		et_ptr->active = new_node_ptr;
		et_ptr->root = new_node_ptr;
		return;
	}

	// Goes deeper through tree
	expr_node_t* curr_ptr = et_ptr->root;
	while(1){
		/*
		  Found node that has lower precedence exactly by 1
		  New node is inserted above this node	
		*/
		if (curr_ptr->precedence - new_node_ptr->precedence == 1){
			// Rest of the tree is instered to left in new node
			new_node_ptr->left = curr_ptr->right;
			curr_ptr->right = new_node_ptr;
			et_ptr->active = new_node_ptr;
			return;
		}
		
		// Continues right because we read from left to right 
		if(curr_ptr->right != NULL && !curr_ptr->right->is_terminal){
			curr_ptr = curr_ptr->right;
			continue;
		}
		
		/*
			There isn't node with lower precedence than new node
			because of that new node is inserted at end of right branch
		*/
		if (curr_ptr->right != NULL){
			new_node_ptr->left = curr_ptr->right;
		}
		curr_ptr->right = new_node_ptr;
		et_ptr->active = new_node_ptr;
		
		break;
	}
}

 
/**
 * Inits new node that stores operator
 * Inserts new node to expression tree
 *
 * @param et_ptr Pointer to expression tree
 * @param cmp_prec Value of precedence that is used for comparing
 * @param save_prec Value of precedence that is saved in tree
 * @param tok Token that stores operator
 */
void expr_tree_add_branch_prec(expr_tree_t* et_ptr, int cmp_prec, int save_prec, token_t tok){
	expr_node_t* new_node_ptr = malloc(sizeof(expr_node_t));
	new_node_ptr->is_terminal = 0;
	new_node_ptr->precedence = save_prec;
	new_node_ptr->token = tok;
	new_node_ptr->params_len = 0;
	new_node_ptr->params = NULL;
	new_node_ptr->right = NULL;
	new_node_ptr->left = NULL;
	
	if(new_node_ptr == NULL){
		alloc_error();
		return;
	}
	
	expr_tree_extend(et_ptr, cmp_prec, new_node_ptr);
	
}


/**
 * Gets precedence value of operator and then 
 * inserts it in to the expression tree 
 *
 * @param et_ptr Pointer to expression tree
 * @param tok Token that stores operator
 */
void expr_tree_add_branch(expr_tree_t* et_ptr, token_t tok){
	int prec = get_precedence_by_type(tok.type);
	expr_tree_add_branch_prec(et_ptr, prec, prec, tok);
}


/**
 * Inits and insert new leaf expression node to tree
 * This node should only store terminals 
 *
 * @param et_ptr Pointer to expression tree
 * @param tok Token that stores constant, variable or function name
 * @returns NULL if there is an allocation error otherwise pointer to new node
 */
expr_node_t* expr_tree_add_leaf(expr_tree_t* et_ptr, token_t tok){
	expr_node_t* new_node_ptr = malloc(sizeof(expr_node_t));
	new_node_ptr->is_terminal = 1;
	new_node_ptr->token = tok;
	new_node_ptr->precedence = 0;
	new_node_ptr->params_len = 0;
	new_node_ptr->params = NULL;
	new_node_ptr->left = NULL;
	new_node_ptr->right = NULL;
	
	if(new_node_ptr == NULL){
		alloc_error();
		return NULL;
	}
	
	// First operand in tree
	if (et_ptr->active == NULL){
		et_ptr->root = new_node_ptr;
		return new_node_ptr;
	}
		
	// Adding operand to operator
	if(et_ptr->active->left == NULL){
		et_ptr->active->left = new_node_ptr;
	} else {
		et_ptr->active->right = new_node_ptr;
	}
	
	return new_node_ptr;
	
}


/**
 * Recursively calls itself on expression nodes and frees memory
 *
 * @param en_ptr Pointer to root expression node
 */
void expr_node_dtor(expr_node_t* en_ptr){
	if (!en_ptr){
		return;
	}
	
	if(en_ptr->params_len){
		for (int i = 0; i < en_ptr->params_len; i++){
			expr_node_dtor(en_ptr->params[i]);
		}
		free(en_ptr->params);
		en_ptr->params = NULL;
	}
	
	expr_node_dtor(en_ptr->left);
	expr_node_dtor(en_ptr->right);
	
	free(en_ptr);
	
}

void tok_content_esc_print(char * str){
    int i = 0;
    while(str[i] != 0){
        if(str[i] > ' ' && str[i] != '\\'){
            printf("%c",str[i]);
        } else {
            printf("|%.3d",str[i]);
        }
        i++;
    }
}

/**
 * Recursively calls itself on expression nodes and prints out 
 * node info in json format
 * 
 * @param en_ptr Pointer to root expression node
 */
void expr_node_print(expr_node_t* en_ptr){
	if (en_ptr == NULL){
		printf("null");
		return;
	}
	printf("{");
	
	printf("\"type\": \"%s\",",token_enum_to_string(en_ptr->token.type));
	if(en_ptr->token.content != NULL){
		//printf("\"content\": \"%s\",",en_ptr->token.content);
		printf("\"content\": \"");
		tok_content_esc_print(en_ptr->token.content);
		printf("\",");
	}
	printf("\"precedence\": \"%d\",",en_ptr->precedence);
	if(en_ptr->is_terminal){
		printf("\"is_terminal\": true,");
	} else {
		printf("\"is_terminal\": false,");
	}
	printf("\"line\": %d,",en_ptr->token.line);
	printf("\"column\": %d,",en_ptr->token.column);
	printf("\"params_len\": %d,",en_ptr->params_len);
	
	if(en_ptr->params_len){
		printf("\"params\":[");
		for (int i = 0; i < en_ptr->params_len; i++){
			expr_node_print(en_ptr->params[i]);
			if(i != en_ptr->params_len-1){
				printf(",");
			}
		}
		printf("],");
	}
	
	printf("\"left\": ");
	expr_node_print(en_ptr->left);
	printf(",");
	printf("\"right\": ");
	expr_node_print(en_ptr->right);
	printf("}");
}
