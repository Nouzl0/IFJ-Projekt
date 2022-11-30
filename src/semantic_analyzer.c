#include "semantic_analyzer.h"

//void semantic_error(int error_code, token_t token, char* info);
/**
 * 
 * 
 * @param table 
 * @param func_item 
 */
void register_function(STList* table, stx_node_t* func_item){
	
	token_t* name_tok_ptr = func_item->items[0]->token;
	char* func_name = name_tok_ptr->content;
	int params_len = func_item->items[1]->items_len;
	token_type return_type = func_item->token->type;
	
	
	ST_CreateResize(&table, func_name);
	
	STElementDataPtr data = ST_DataGet(table, func_name);
	
	data->type = return_type;
	data->tok_ptr = name_tok_ptr;
	
	data->params = malloc(sizeof(STDataParam) * params_len);
	
	for (int i = 0; i < func_item->items[1]->items_len; i++){
		stx_node_t* type_item = func_item->items[1]->items[i];
		data->params[i].type = type_item->token->type;
		stx_node_t* name_item = type_item->items[0];
		data->params[i].param_name = name_item->token->content;
	}
	
	/*
	//Tyto udaje pridat do tabulky funkci
	printf("Nazev funkce: %s\n", func_item->items[0]->token->content);
	printf("Vraci: %s\n",token_enum_to_string(func_item->token->type));
	
	for (int i = 0; i < func_item->items[1]->items_len; i++){
		stx_node_t* type_item = func_item->items[1]->items[i];
		printf("Typ parametru: %s\n",token_enum_to_string(type_item->token->type));
		stx_node_t* name_item = type_item->items[0];
		printf("Jemno parametru: %s\n", name_item->token->content);
	}
	*/	
}
/*
//Rekurzivne volano potreba zjistovat errory
token_type rec_check_types(expr_node_t* expr){
	if(!expr){
		return NIL;
	}
	
	
	//Zakladni matematicke operace
	if(expr->token.type >= MINUS && expr->token.type <= SLASH){
		//Typy operadnu se musi schodovat
		//a kdyz se neschoduji je potreba pridat do stromu pretypovani
		//Jejich vysledekem je to co do nich leze a u SLASH je to vzdy float
		token_type left = rec_check_types(expr->left);
		token_type right = rec_check_types(expr->right);
		return VOID;
	}
	
	//Pouze pro scitani dvou retezcu
	if (expr->token.type == DOT){
		if(rec_check_types(expr->left) == STRING && rec_check_types(expr->left) == STRING){
			return STRING;
		} else {
			//Semanticka chyba spatny datovy typ
			//Jine datove typy jak STRING
		}
	}
	
	//Porovnavani datovych typu 
	if(expr->token.type == TYPE_EQUAL || expr->token.type == TYPE_NOT_EQUAL){
		//Muzou se porovnavat jakekoliv datove typy
		//Pri TYPE_EQUAL se musi shodovat i obsah
		rec_check_types(expr->right);
		rec_check_types(expr->left);
		return VOID;
	}
	
	//Porovnavaci operatory
	if(expr->token.type >= EQUAL || expr->token.type <= LESS){
		//Tady nevim jake datove typy budou povolene
		rec_check_types(expr->right);
		rec_check_types(expr->left);
		return VOID;
	}
	
	//Pretypovani konstant
	//Mozna by se hodilo provest zmeny i v ast 
	if(expr->token.type == NUMBER){
		return INT;
	}
	
	if(expr->token.type == FRACTION){
		return FLOAT;
	}
	
	if(expr->token.type == TEXT){
		return INT;
	}
	
	//Zjisteni typu promene
	if(expr->token.type == VARIABLE){
		//Ziskat data z tabulky symbolu
		return VOID;
	}
	
	//Zjisteni navratoveho typu funkce
	if(expr->token.type == IDENTIFIER){
		//Vraci data z tabulky funkci
			
		//Porovnat pocet a typ parametru
		//pri volani funkce
		for(int i = 0; i < expr->params_len; i++){
			//rec_get_type(params[i])
		}
		
		return VOID;
	}
	
	return VOID;
}

void analyze_assignexpr(stx_node_t* item){
	//Nazev promene do ktere se prirazuje: item->token->content;
	//Vyraz ktery se prirazuje: item->expr
}

void analyze_retexpr(stx_node_t* item){
	//Vyraz ktery se ma vratit: item->expr
}

void analyze_ifelse(stx_node_t* item){
	//Vyraz podminky: item->expr
	//if blok: item->items[0] 
	//else blok: item->items[1] tento blok tam byt nemusi
}

void analyze_whileblock(stx_node_t* item){
	//Vyraz podminky: item->expr
	//blok: item->items[0]
}
*/
void analyze_item(stx_node_t* item){
	
	//item->type
	switch(item->type){
		case EXPR:
			//rec_check_types(item->expr);
			break;
		
		case ASSIGNEXPR:
			//Vsechny prvky ktere jsou pridane do tabulky
			//jsou na konci bloku zase odebrany
			//tim zajistim aby promena nemohla byt pouzita z venku
			//analyze_assignexpr(item);
			break;
		

		case RETEXPR:
			//analyze_retexpr(item);
			break;
		
		case IFELSE:
			//analyze_ifelse(item);
			break;
		
		case WHILEBLOCK:
			//analyze_whileblock(item);
			break;
			
		default:
			//Nemelo by nastat
			return;
	}
	
	
}

/*
void analyze_function(stx_node_t* func_item){
	//bud vytvori novou tabulku nebo bude pouzivat suffix -{nazev_funkce}
	//analyze_item(func_item->items[2]);
	
}
*/

void analyze_ast(stx_node_t* ast_root){
	
	if(ast_root->items_len < 1){
		//Prazdy blok
		return;
	}
	
	STList* func_table = ST_Init(10);
	
	//Prochazi vsechny funkce a bere jenom hlavicku
	for (int i = 0; i < ast_root->items_len; i++){
		stx_node_t* item = ast_root->items[i];
		if (item->type == FUNCBLOCK){
			register_function(func_table,item);
		}
	}
	
	//Prochazi vsechny prvky a anylyzuje je
	for (int i = 0; i < ast_root->items_len; i++){	
		stx_node_t* item = ast_root->items[i];
		if (item->type == FUNCBLOCK){
			//analyze_function(item);
		} else {
			analyze_item(item);
		}
	}
	
	

	
	STElementDataPtr data = ST_DataGet(func_table, "getMax");
	
	if(data){
		printf("je: %s\n", data->params[0].param_name);
	} else {
		printf("neni\n");
	}
	
	ST_Dispose(&func_table);
*/
}