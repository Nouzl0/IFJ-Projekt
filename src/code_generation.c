#include "code_generation.h"

/* - - - - - - - - - - */
/*    MAIN FUNCTION    */
/* - - - - - - - - - - */

// working on this library 

// generate_code (printf) from syntactic tree
void generate_code(stx_node_t *AS_Tree) 
{
    printf("\n");

    // check if tree is not empty
	if (AS_Tree == NULL) {
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

    // prints assignstmt items
    do_assignstmt(AS_Tree);
}


/* - - - - - - - - - - - -*/
/*  ASSIGNSTMT FUNCTIONS  */
/* - - - - - - - - - - - -*/

/**
 * Print arithmetic expression from assignstmt
 * 
 * @param AS_Tree 
 */
void do_assignstmt(stx_node_t* AS_Tree) {
    
    // variables
    static bool tmp_var_active = false; 

    // check if tree is not empty
    if ((AS_Tree == NULL) || (AS_Tree->type != ASSIGNEXPR)) {
        return;
    }


    // defines variable
    printf("DEFVAR LF@%s\n", AS_Tree->token->content);

    // defines tmp variables
    if (tmp_var_active == false) {
        printf("DEFVAR LF@%s\nDEFVAR LF@%s\nDEFVAR LF@%s\n", "#t0", "#t1", "#t2");
        tmp_var_active = true;
    }

    // print rest of the tree
    assignstmt_print(AS_Tree->expr, AS_Tree->token->content, true);
}


/**
 * Recursively prints arithmetic expression from Precedence Tree
 *  - used in function (do_assignstmt,)
 * 
 * @param AP_Tree Precedence tree, from which instruction will be created
 * @param assigned_var Variable, for which are the instructions created
 * @param reset Used to indicate if the funtion is started recursively or not. - (true = start, false = recursive)
 */
void assignstmt_print(expr_node_t* AP_Tree , char* assigned_var, bool reset) {

    // #0 - variable initialization
    const char instruction_set[6][7] = { "SUB", "ADD", "MUL", "DIV", "CONCAT", };   // instruction set
    const char tmp_variables[10][10] = { "#t0", "#t1", "#t2", };                    // temporary variables
    char *var_left = NULL, *var_right = NULL;                                       // variables for left and right side of the tree
    static int var_offset = 0;                                                      // offset for temporary variables
    static int side = 0;                                                            // decides on which side of the tree is the function
    static char *left_term = NULL, *right_term = NULL;                              // rembers left and right terminal of the tree

    // #1 - function check and reset
    if (AP_Tree == NULL) {
        return;
    }  

    if (reset == true) {
        var_offset = 0;
    }

    // #2 - assigning temporary variables
    if ((AP_Tree->left != NULL) && AP_Tree->right != NULL) {

        // assign to both sub-trees
        if ((AP_Tree->left->is_terminal == false) && (AP_Tree->right->is_terminal == false)) {
            var_left = tmp_variables[0 + var_offset];
            var_right = tmp_variables[1 + var_offset];
        }
        // assign to left sub-tree
        if ((AP_Tree->left->is_terminal == false) && (AP_Tree->right->is_terminal == true)) {
            var_left = tmp_variables[0 + var_offset];
        }
        // assign to left sub-tree
        if ((AP_Tree->left->is_terminal == true) && (AP_Tree->right->is_terminal == false)) {
            var_right = tmp_variables[0 + var_offset];
        }
    }


    // #3 - post order tree traversal
    assignstmt_print(AP_Tree->left, var_left, false);
    assignstmt_print(AP_Tree->right, var_right, false);
    
    // #4 - printing instructions in post order traversal
    if ((AP_Tree->left != NULL) && AP_Tree->right != NULL) {
        
        // both are not-terminals
        if ((AP_Tree->left->is_terminal == false) && (AP_Tree->right->is_terminal == false)) {
            printf("%s %s %s %s\n", instruction_set[AP_Tree->token.type - 13], assigned_var, var_left, var_right);
            side = 0;   // reseting side

            // chagning the var_offset
            if (var_offset == 0) {
                var_offset = 1;
            } else {
                var_offset = 0;
            }
        }

        // both are terminals
        if ((AP_Tree->left->is_terminal == true) && (AP_Tree->right->is_terminal == true)) {
            printf("%s %s %s %s\n", instruction_set[AP_Tree->token.type - 13], assigned_var, left_term, right_term);
            side = 0;   // reseting side
        }

        // left is terminal
        if ((AP_Tree->left->is_terminal == true) && (AP_Tree->right->is_terminal == false)) {
            printf("%s %s %s %s\n", instruction_set[AP_Tree->token.type - 13], assigned_var, left_term, var_right);
            side = 0;   // reseting side      
        }

        // right is terminal
        if ((AP_Tree->left->is_terminal == false) && (AP_Tree->right->is_terminal == true)) {
            printf("%s %s %s %s\n", instruction_set[AP_Tree->token.type - 13], assigned_var, left_term, left_term);
            side = 0;   // reseting side       
        }
    }

    // #5 - saving terminal values on the bottom of the tree
    if (AP_Tree->is_terminal == true) {
        if (side == 0) {
            left_term = AP_Tree->token.content;
            side = 1;
        } else {
            right_term = AP_Tree->token.content;
            side = 0;
        }
    }
}

