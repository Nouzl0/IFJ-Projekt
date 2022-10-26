#pragma once

#include <stdlib.h>

#include "error_handler.h"
#include "precedence_tree.h"

typedef enum {
	BLOCK,       // Blok kodu
	FUNCBLOCK,   // Deklarace funkce
	DECLINIT,    // Deklarace a zaroven inicializace promene
	DECL,        // Jenom deklarace promene
	ASSIGNSTMT,  // Prirazeni vyrazu do promene
	STMT,        // Samotny vyraz
	RETSTMT,     // Return s vyrazem
	IFELSE,      // Vetveni pomoci IF a ELSE ktere je brano jako celek
	WHILEBLOCK,  // Cyklus s podminkou a blokem
} item_type;


typedef struct stree_item_t stree_item_t;

struct stree_item_t {
	item_type type;
	token_t token;
	int level;
	int items_size;
	int items_len;
	stree_item_t** items;
	ptree_item_t* stmt;
};
