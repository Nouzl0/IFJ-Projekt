#include "binary_tree.h"

/*
TODO:

	Pridat Uvolneni pameti

	Prejmenovat na prec_tree

	Mozna prejmenovat nektere funkce

	Mozna zkusit vytvorit lepsi algoritmus na zjisteni
	urovne precedene podle tokenu

*/


int get_precedence_by_type(token_type type){
	token_type arr1[] = {
		NEG,
		STAR, SLASH,
		PLUS, MINUS, DOT,
		GREATER, LESS, EQUAL, GREATER_EQUAL, LESS_EQUAL,
		AND, OR
		};
	
	int arr2[] = {
		1,
		2, 2,
		3, 3, 3,
		4, 4, 4, 4, 4,
		5, 5
	};
	
	for (unsigned int i = 0; i < sizeof(arr1) / sizeof(int); i++){
		if(type == arr1[i]){
			return arr2[i];
		}
	}
	return 0;
	
}

binary_tree_t* binary_tree_init(){
	
	binary_tree_t* btr_ptr = malloc(sizeof(binary_tree_t));
	
	if (btr_ptr == NULL){
		handle_program_error();
	}
	
	btr_ptr->root = NULL;
	btr_ptr->active = NULL;
	
	return btr_ptr;
}

/*
btree_item_t* get_item_by_level(binary_tree_t* bt_ptr){
	btree_item_t* curr_ptr = bt_ptr->root;
	for (int i = 0; i < bt_ptr->level; i++){
		curr_ptr = curr_ptr->right;
	}
	return curr_ptr;
}
*/

void binary_tree_extend(binary_tree_t* bt_ptr, int cmp_prec, btree_item_t* bt_item_ptr){
	//Uplne prvni operand ve stromu
	if (bt_ptr->active == NULL){
		if(bt_ptr->root != NULL){
			bt_item_ptr->left = bt_ptr->root;
		}
		bt_ptr->root = bt_item_ptr;
		bt_ptr->active = bt_item_ptr;
		return;
	}
	
	//Operand je pridan uplne na zacatek jelikoz ma nejvetsi precedenci
	if(cmp_prec >= bt_ptr->root->precedence){
		bt_item_ptr->left = bt_ptr->root;
		bt_ptr->active = bt_item_ptr;
		bt_ptr->root = bt_item_ptr;
		return;
	}
	
	
	btree_item_t* curr_ptr = bt_ptr->root;
	while(1){
		
		if (curr_ptr->precedence - bt_item_ptr->precedence == 1){
			bt_item_ptr->left = curr_ptr->right;
			curr_ptr->right = bt_item_ptr;
			bt_ptr->active = bt_item_ptr;
			//Pushnout mezi
			//Zbytek stromu vlozit vlevo
			return;
		}
		
		//Pokracuje dolu smerem vpravo jelikoz ctu z leva do prava
		if(curr_ptr->right != NULL && !curr_ptr->right->is_leaf){
			curr_ptr = curr_ptr->right;
			continue;
		}

		//Uplne posledni mozny branch za ktery to jde vlozit
		bt_item_ptr->left = curr_ptr->right;
		curr_ptr->right = bt_item_ptr;
		bt_ptr->active = bt_item_ptr;
		
		break;
	}
}


void binary_tree_fork_prec(binary_tree_t* bt_ptr, int cmp_prec, int save_prec, char* data){
	btree_item_t* bt_item_ptr = malloc(sizeof(btree_item_t));
	bt_item_ptr->is_leaf = 0;
	bt_item_ptr->precedence = save_prec;
	bt_item_ptr->data = data;
	bt_item_ptr->right = NULL;
	bt_item_ptr->left = NULL;
	
	binary_tree_extend(bt_ptr, cmp_prec, bt_item_ptr);
	
}

void binary_tree_fork(binary_tree_t* bt_ptr, token_type type, char* data){
	int prec = get_precedence_by_type(type);
	binary_tree_fork_prec(bt_ptr, prec, prec, data);
}

void binary_tree_add_leaf(binary_tree_t* bt_ptr, char* data){
	btree_item_t* bt_item_ptr = malloc(sizeof(btree_item_t));
	bt_item_ptr->is_leaf = 1;
	bt_item_ptr->data = data;
	
	if(bt_item_ptr == NULL){
		handle_program_error();
	}
	
	//Uplne prvni identifikator ve stromu;
	if (bt_ptr->active == NULL){
		bt_ptr->root = bt_item_ptr;
		
		return;
	}
	
	//Pridava identifikator k operatoru
	if(bt_ptr->active->left == NULL){
		bt_ptr->active->left = bt_item_ptr;
	} else {
		bt_ptr->active->right = bt_item_ptr;
	}
	
}



void recursive_print(btree_item_t* bi){
	if (bi == NULL){
		printf("null");
		return;
	}
	printf("{");
	printf("\"data\": \"%s\",",bi->data);
	
	if(bi->is_leaf){
		printf("\"is_leaf\": true,");
	} else {
		printf("\"is_leaf\": false,");
	}
	
	printf("\"left\": ");
	recursive_print(bi->left);
	printf(",");
	printf("\"right\": ");
	recursive_print(bi->right);
	printf("}");
}

void binary_tree_to_json(binary_tree_t* bt_ptr){
	printf("--------------------------------\n");
	recursive_print(bt_ptr->root);
	printf("\n--------------------------------\n");
}