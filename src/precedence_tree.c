#include "precedence_tree.h"

/*
TODO:

	Pridat Uvolneni pameti

	Zkusit vytvorit lepsi algoritmus na zjisteni
	urovne precedene podle tokenu

*/


/**
 *
 *   Mozna predelat
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


/**
 * Konstroktor pro strom precedence
 *
 * @param pt_ptr Ukazatel na strom precedence
 */
void ptree_ctor(ptree_t* pt_ptr){
	pt_ptr->root = NULL;
	pt_ptr->active = NULL;
}


/**
 * Pripojuje novy prvek do stromu precedence
 * jedna se o prvek ktery neni listovy
 * Takovy prvek je budto operator nebo jiny strom
 * strom precedence
 *
 * @param pt_ptr Ukazatel na strom precedence
 * @param cmp_prec Hodnota ktera urcuje precedenci podle
 * ktere se hleda ve stromu pro prvek nove misto
 * @param pt_item_ptr Ukazatel na pridavany prvek
 */
void ptree_extend(ptree_t* pt_ptr, int cmp_prec, ptree_item_t* pt_item_ptr){
	//Uplne prvni operand ve stromu
	if (pt_ptr->active == NULL){
		if(pt_ptr->root != NULL){
			pt_item_ptr->left = pt_ptr->root;
		}
		pt_ptr->root = pt_item_ptr;
		pt_ptr->active = pt_item_ptr;
		return;
	}
	
	//Operand je pridan uplne na zacatek jelikoz ma nejvetsi precedenci
	if(cmp_prec >= pt_ptr->root->precedence){
		pt_item_ptr->left = pt_ptr->root;
		pt_ptr->active = pt_item_ptr;
		pt_ptr->root = pt_item_ptr;
		return;
	}
	
	//Postupuju stromem dolu smerem doprava
	ptree_item_t* curr_ptr = pt_ptr->root;
	while(1){
		
		if (curr_ptr->precedence - pt_item_ptr->precedence == 1){
			pt_item_ptr->left = curr_ptr->right;
			curr_ptr->right = pt_item_ptr;
			pt_ptr->active = pt_item_ptr;
			//Pushnout mezi
			//Zbytek stromu vlozit vlevo
			return;
		}
		
		//Pokracuje dolu smerem vpravo jelikoz ctu z leva do prava
		if(curr_ptr->right != NULL && !curr_ptr->right->is_terminal){
			curr_ptr = curr_ptr->right;
			continue;
		}

		//Uplne posledni mozny branch za ktery to jde vlozit
		if (curr_ptr->right != NULL){
			pt_item_ptr->left = curr_ptr->right;
		}
		curr_ptr->right = pt_item_ptr;
		pt_ptr->active = pt_item_ptr;
		
		break;
	}
}

 
 /**
 * Inicializuje novy prvek stromu ktery bude obsahovat operator
 *
 * @param pt_ptr Ukazatel na strom precedence
 * @param cmp_prec Hodnota precedence podle ktere se porovnava
 * @param save_prec Hodna precedence ktera se vklada do prvku
 * @param data Ukazatel na data operatoru
 */
void ptree_add_branch_prec(ptree_t* pt_ptr, int cmp_prec, int save_prec, char* data){
	ptree_item_t* pt_item_ptr = malloc(sizeof(ptree_item_t));
	pt_item_ptr->is_terminal = 0;
	pt_item_ptr->precedence = save_prec;
	pt_item_ptr->data = data;
	pt_item_ptr->right = NULL;
	pt_item_ptr->left = NULL;
	
	ptree_extend(pt_ptr, cmp_prec, pt_item_ptr);
	
}


 /**
 * Ziskava precedenci operatoru a pote ho pridava do stromu
 *
 * @param pt_ptr Ukazatel na strom precedence
 * @param type Typ jakeho je vkladany operator
 * @param data Ukazatel na data operatoru
 */
void ptree_add_branch(ptree_t* pt_ptr, token_type type, char* data){
	int prec = get_precedence_by_type(type);
	ptree_add_branch_prec(pt_ptr, prec, prec, data);
}


/**
 * Pridava listovy prvek ktery je budto promena nebo konstanta
 *
 * @param pt_ptr Ukazatel na strom precedence
 * @param data Ukazatel na data hodnoty ci promene
 */
void prec_tree_add_leaf(ptree_t* pt_ptr, char* data){
	ptree_item_t* pt_item_ptr = malloc(sizeof(ptree_item_t));
	pt_item_ptr->is_terminal = 1;
	pt_item_ptr->data = data;
	
	if(pt_item_ptr == NULL){
		handle_program_error();
	}
	
	//Uplne prvni identifikator ve stromu;
	if (pt_ptr->active == NULL){
		pt_ptr->root = pt_item_ptr;
		
		return;
	}
	
	//Pridava identifikator k operatoru
	if(pt_ptr->active->left == NULL){
		pt_ptr->active->left = pt_item_ptr;
	} else {
		pt_ptr->active->right = pt_item_ptr;
	}
	
}


/**
 * Pridava listovy prvek ktery je budto promena nebo konstanta
 *
 * @param pt_item_ptr Ukazatel na strom precedence
 */
void recursive_print(ptree_item_t* pt_item_ptr){
	if (pt_item_ptr == NULL){
		printf("null");
		return;
	}
	printf("{");
	printf("\"data\": \"%s\",",pt_item_ptr->data);
	
	if(pt_item_ptr->is_terminal){
		printf("\"is_terminal\": true,");
	} else {
		printf("\"is_terminal\": false,");
	}
	
	printf("\"left\": ");
	recursive_print(pt_item_ptr->left);
	printf(",");
	printf("\"right\": ");
	recursive_print(pt_item_ptr->right);
	printf("}");
}

void ptree_debug_to_json(ptree_item_t* bi_ptr){	
	printf("--------------------------------\n");
	recursive_print(bi_ptr);
	printf("\n--------------------------------\n");
}