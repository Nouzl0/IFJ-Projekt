#include "code_generation.h"

/* - - - - - - - - - - */
/*    MAIN FUNCTION    */
/* - - - - - - - - - - */

// working on this library 

// generate_code (printf) from syntactic tree
/*
void generate_code(stree_item_t *AS_Tree) 
{
    printf("\n");

    // check if tree is not empty
	if (AS_Tree == NULL) {
		printf("NULL");
		return;
	}
    
    // printing what is in the tree debug
    printf("Type = %d\n", AS_Tree->type);

    // go through all nodes in tree recursively
    if (AS_Tree->items_len != 0) {
        for (int i = 0; i < AS_Tree->items_len; i++) {
            generate_code(AS_Tree->items[i]);
        }
    }

    //print p_tree
    do_assignstmt(AS_Tree);
}
*/

/* - - - - - - - - - - - -*/
/*  ASSIGNSTMT FUNCTIONS  */
/* - - - - - - - - - - - -*/

/**
 * Print arithmetic expression from assignstmt
 * 
 * @param AS_Tree 
 */
/*
void do_assignstmt(stree_item_t* AS_Tree) {
    if ((AS_Tree == NULL) || (AS_Tree->type != ASSIGNSTMT)) {
        return;
    }

    // print token
    printf("%s =\n", AS_Tree->token->content);

    // print rest of the tree
    assignstmt_print(AS_Tree->stmt, AS_Tree->token->content);
}
*/


/**
 * Helps function assignstmt_print to print the rest of the tree
 * 
 * @param AP_Tree 
 * @return * used 
 */
/*
void assignstmt_print(ptree_item_t* AP_Tree, char* assigned_var) {

    // tmp variables for string variables
    char tmp_var[6] = "tmp";
    static int tmp_var_num = 0;

    // print num/variable/fraction
    if (is_assignstmt_terminal(AP_Tree)) {
        printf("%s ", AP_Tree->token.content);
        return;
    
    // else print arithmetic operator with assigned variable
    } else {
        printf("\n%s %s ", assignstmt_set[(AP_Tree->token.type) - 13], assigned_var);  // 13 is the first arithmetic operator
    }

    // creating tmp variables for completing the arithmetic expression
    if (AP_Tree->left != NULL) {
        printf("%s%d ", tmp_var, tmp_var_num);
        tmp_var_num++;
    }

    // recursively going through tree
    if (AP_Tree->left != NULL) {
        if (is_assignstmt_terminal(AP_Tree->left)) {
        
        }
        assignstmt_print(AP_Tree->left, "bruh");
    }

    if (AP_Tree->right != NULL) {
        if (is_assignstmt_terminal(AP_Tree->right)) {
            
        }

        assignstmt_print(AP_Tree->right, "momement");
    }
}
*/
