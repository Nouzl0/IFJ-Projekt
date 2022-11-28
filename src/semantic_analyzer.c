#include "semantic_analyzer.h"

//void semantic_error(int error_code, token_t token, char* info);

void register_function(stree_item_t* func_item){
	//Tyto udaje pridat do tabulky funkci
	printf("Nazev funkce: %s\n", func_item->items[0]->token->content);
	printf("Vraci: %s\n",token_enum_to_string(func_item->token->type));
	
	for (int i = 0; i < func_item->items[1]->items_len; i++){
		stree_item_t* type_item = func_item->items[1]->items[i];
		printf("Typ parametru: %s\n",token_enum_to_string(type_item->token->type));
		stree_item_t* name_item = type_item->items[0];
		printf("Jemno parametru: %s\n", name_item->token->content);
	}	
}

//Rekurzivne volano potreba zjistovat errory
token_type rec_check_types(ptree_item_t* stmt){
	if(!stmt){
		return NIL;
	}
	
	
	//Zakladni matematicke operace
	if(stmt->token.type >= MINUS && stmt->token.type <= SLASH){
		//Typy operadnu se musi schodovat
		//a kdyz se neschoduji je potreba pridat do stromu pretypovani
		//Jejich vysledekem je to co do nich leze a u SLASH je to vzdy float
		token_type left = rec_check_types(stmt->left);
		token_type right = rec_check_types(stmt->right);
		return VOID;
	}
	
	//Pouze pro scitani dvou retezcu
	if (stmt->token.type == DOT){
		if(rec_check_types(stmt->left) == STRING && rec_check_types(stmt->left) == STRING){
			return STRING;
		} else {
			//Semanticka chyba spatny datovy typ
			//Jine datove typy jak STRING
		}
	}
	
	//Porovnavani datovych typu 
	if(stmt->token.type == TYPE_EQUAL || stmt->token.type == TYPE_NOT_EQUAL){
		//Muzou se porovnavat jakekoliv datove typy
		//Pri TYPE_EQUAL se musi shodovat i obsah
		rec_check_types(stmt->right);
		rec_check_types(stmt->left);
		return VOID;
	}
	
	//Porovnavaci operatory
	if(stmt->token.type >= EQUAL || stmt->token.type <= LESS){
		//Tady nevim jake datove typy budou povolene
		rec_check_types(stmt->right);
		rec_check_types(stmt->left);
		return VOID;
	}
	
	//Pretypovani konstant
	//Mozna by se hodilo provest zmeny i v ast 
	if(stmt->token.type == NUMBER){
		return INT;
	}
	
	if(stmt->token.type == FRACTION){
		return FLOAT;
	}
	
	if(stmt->token.type == TEXT){
		return INT;
	}
	
	//Zjisteni typu promene
	if(stmt->token.type == VARIABLE){
		//Ziskat data z tabulky symbolu
		return VOID;
	}
	
	//Zjisteni navratoveho typu funkce
	if(stmt->token.type == IDENTIFIER){
		//Vraci data z tabulky funkci
			
		//Porovnat pocet a typ parametru
		//pri volani funkce
		for(int i = 0; i < stmt->params_len; i++){
			//rec_get_type(params[i])
		}
		
		return VOID;
	}
	
	return VOID;
}

void analyze_assignstmt(stree_item_t* item){
	//Nazev promene do ktere se prirazuje: item->token->content;
	//Vyraz ktery se prirazuje: item->stmt
}

void analyze_retstmt(stree_item_t* item){
	//Vyraz ktery se ma vratit: item->stmt
}

void analyze_ifelse(stree_item_t* item){
	//Vyraz podminky: item->stmt
	//if blok: item->items[0] 
	//else blok: item->items[1] tento blok tam byt nemusi
}

void analyze_whileblock(stree_item_t* item){
	//Vyraz podminky: item->stmt
	//blok: item->items[0]
}

void analyze_item(stree_item_t* item){
	
	//item->type
	switch(item->type){
		case EXPR:
			rec_check_types(item->stmt);
			break;
		
		case ASSIGNEXPR:
			//Vsechny prvky ktere jsou pridane do tabulky
			//jsou na konci bloku zase odebrany
			//tim zajistim aby promena nemohla byt pouzita z venku
			analyze_assignstmt(item);
			break;
		
		case RETEXPR:
			analyze_retstmt(item);
			break;
		
		case IFELSE:
			analyze_ifelse(item);
			break;
		
		case WHILEBLOCK:
			analyze_whileblock(item);
			break;
			
		default:
			//Nemelo by nastat
			return;
	}
	
	
}

void analyze_function(stree_item_t* func_item){
	//bud vytvori novou tabulku nebo bude pouzivat suffix -{nazev_funkce}
	//analyze_item(func_item->items[2]);
	
}


void analyze_ast(stree_item_t* ast_root){
	
	if(ast_root->items_len < 1){
		//Prazdy blok
		return;
	}
	
	//Prochazi vsechny funkce a bere jenom hlavicku
	for (int i = 0; i < ast_root->items_len; i++){
		stree_item_t* item = ast_root->items[i];
		if (item->type == FUNCBLOCK){
			register_function(item);
		}
	}
	
	//Prochazi vsechny prvky a anylyzuje je
	for (int i = 0; i < ast_root->items_len; i++){	
		stree_item_t* item = ast_root->items[i];
		if (item->type == FUNCBLOCK){
			analyze_function(item);
		} else {
			analyze_item(item);
		}
	}
	
}