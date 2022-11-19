#include "precedence_tree.h"

/*
TODO:

	Pridat Uvolneni pameti

	Zkusit vytvorit lepsi algoritmus na zjisteni
	urovne precedene podle tokenu


	Upravit komentare
	
*/


/**
 *
 *
 */
int get_precedence_by_type(token_type type){
	token_type arr1[] = {
		NEG,
		STAR, SLASH,
		PLUS, MINUS, DOT,
		GREATER, LESS, TYPE_EQUAL,TYPE_NOT_EQUAL, EQUAL, GREATER_EQUAL, LESS_EQUAL,
		AND, OR
		};
	
	int arr2[] = {
		1,
		2, 2,
		3, 3, 3,
		4, 4, 4, 4, 4, 4, 4,
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
		/*
			Nalezen operand ktery ma o jednu uroven mensi precedenci 
			tudiz je potreba novy operand vlozit nad neho
		*/
		if (curr_ptr->precedence - pt_item_ptr->precedence == 1){
			/*
				Zbytek stromu ktery se pod novym operandem nachazi
				je do noveho prvku vlozen doleva
			*/
			pt_item_ptr->left = curr_ptr->right;
			curr_ptr->right = pt_item_ptr;
			pt_ptr->active = pt_item_ptr;
			return;
		}
		
		//Pokracuje dolu smerem vpravo jelikoz ctu z leva do prava
		if(curr_ptr->right != NULL && !curr_ptr->right->is_terminal){
			curr_ptr = curr_ptr->right;
			continue;
		}
		
		/*
			Jelikoz nebyla nalezena nizsi precedence nez precedence
			vlozeneho operandu je operand vlozen az uplne na konec 
			v prave vetvi stromu
		*/
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
void ptree_add_branch_prec(ptree_t* pt_ptr, int cmp_prec, int save_prec, token_t tok){
	ptree_item_t* pt_item_ptr = malloc(sizeof(ptree_item_t));
	pt_item_ptr->is_terminal = 0;
	pt_item_ptr->precedence = save_prec;
	pt_item_ptr->token = tok;
	pt_item_ptr->params_len = 0;
	pt_item_ptr->params = NULL;
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
void ptree_add_branch(ptree_t* pt_ptr, token_t tok){
	int prec = get_precedence_by_type(tok.type);
	ptree_add_branch_prec(pt_ptr, prec, prec, tok);
}


/**
 * Pridava listovy prvek ktery je budto promena, konstanta nebo volani funkce
 *
 * @param pt_ptr Ukazatel na strom precedence
 * @param data Ukazatel na data hodnoty, promene ci funkce
 * @returns NULL kdyz se nepovede alokace jinak ukazatel na pridany terminal
 */
ptree_item_t* prec_tree_add_leaf(ptree_t* pt_ptr, token_t tok){
	ptree_item_t* pt_item_ptr = malloc(sizeof(ptree_item_t));
	pt_item_ptr->is_terminal = 1;
	pt_item_ptr->token = tok;
	pt_item_ptr->precedence = 0;
	pt_item_ptr->params_len = 0;
	pt_item_ptr->params = NULL;
	pt_item_ptr->left = NULL;
	pt_item_ptr->right = NULL;
	
	if(pt_item_ptr == NULL){
		handle_program_error();
		return NULL;
	}
	
	//Uplne prvni identifikator ve stromu;
	if (pt_ptr->active == NULL){
		pt_ptr->root = pt_item_ptr;
		return pt_item_ptr;
	}
	
	//Pridava identifikator k operatoru
	if(pt_ptr->active->left == NULL){
		pt_ptr->active->left = pt_item_ptr;
	} else {
		pt_ptr->active->right = pt_item_ptr;
	}
	
	return pt_item_ptr;
	
}


/**
 * Rekurzivnim volanim uvolnuje pamet zabiranou precedencnim stromem
 *
 * @param pt_item_ptr Ukazatel na korenovy prvek stromu precedence
 */
void ptree_dtor(ptree_item_t* pt_item_ptr){
	if (pt_item_ptr == NULL){
		return;
	}
	
	if(pt_item_ptr->params_len){
		for (int i = 0; i < pt_item_ptr->params_len; i++){
			ptree_dtor(pt_item_ptr->params[i]);
		}
		free(pt_item_ptr->params);
		pt_item_ptr->params = NULL;
	}
	
	ptree_dtor(pt_item_ptr->left);
	ptree_dtor(pt_item_ptr->right);
	
	free(pt_item_ptr);
	
}


/**
 * Vypisuje obsah precedenciho stromu jako JSON objekt
 *
 * @param pt_item_ptr Ukazatel na prvek stromu precedence
 */
void recursive_print(ptree_item_t* pt_item_ptr){
	if (pt_item_ptr == NULL){
		printf("null");
		return;
	}
	printf("{");
	
	printf("\"type\": \"%s\",",token_enum_to_string(pt_item_ptr->token.type));
	if(pt_item_ptr->token.content != NULL){
		printf("\"content\": \"%s\",",pt_item_ptr->token.content);
	}
	printf("\"precedence\": \"%d\",",pt_item_ptr->precedence);
	if(pt_item_ptr->is_terminal){
		printf("\"is_terminal\": true,");
	} else {
		printf("\"is_terminal\": false,");
	}
	printf("\"line\": %d,",pt_item_ptr->token.line);
	printf("\"column\": %d,",pt_item_ptr->token.column);
	printf("\"params_len\": %d,",pt_item_ptr->params_len);
	
	if(pt_item_ptr->params_len){
		printf("\"params\":[");
		for (int i = 0; i < pt_item_ptr->params_len; i++){
			recursive_print(pt_item_ptr->params[i]);
			if(i != pt_item_ptr->params_len-1){
				printf(",");
			}
		}
		printf("],");
	}
	
	printf("\"left\": ");
	recursive_print(pt_item_ptr->left);
	printf(",");
	printf("\"right\": ");
	recursive_print(pt_item_ptr->right);
	printf("}");
}

/**
 * Pomocna funkce pro vypis
 *
 * @param bi_ptr Ukazatel na prvek stromu precedence
 */
void ptree_debug_to_json(ptree_item_t* bi_ptr){	
	printf("--------------------------------\n");
	recursive_print(bi_ptr);
	printf("\n--------------------------------\n");
}