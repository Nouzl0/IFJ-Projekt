#include "code_generation.h"

/* - - - - - - - - - - */
/*    MAIN FUNCTION    */
/* - - - - - - - - - - */

// working on this library 

/**
 * @brief Generates instruction code from AST
 * 
 * @param AS_Tree Syntax tree
 */
void generate_code(stx_node_t *AS_Tree) // a.k.a do_block
{
    // check if tree is not empty
	if (AS_Tree == NULL) {
		return;
	}
    
    // go through all nodes in tree recursively
    if (AS_Tree->items_len != 0) {
        for (int i = 0; i < AS_Tree->items_len; i++) {
            do_assignstmt(AS_Tree->items[i]);
            do_whileblock(AS_Tree->items[i]);
            do_ifelse(AS_Tree->items[i]);
        }
    }
}



/* - - - - - - - - - - - -*/
/*   ITEM TYPE FUNCTIONS  */
/* - - - - - - - - - - - -*/

/**
 * Assignes value to variable
 *  - TODO: make sure the format of assigned variable is correct
 * 
 * @param AS_Tree - Abstract syntax tree
 */
void do_assignstmt(stx_node_t* AS_Tree) {
    
    // variables
    static bool tmp_var_active = false; 

    // check if tree is not empty
    if ((AS_Tree == NULL) || (AS_Tree->type != ASSIGNEXPR)) {
        return;
    }

    // defines variable - TODO: check if this is the right format
    printf("DEFVAR LF@%s\n", AS_Tree->token->content);

    // defines tmp variables - TODO: check if this is the right format and make it smarter
    if (tmp_var_active == false) {
        printf("DEFVAR LF@%s\nDEFVAR LF@%s\nDEFVAR LF@%s\n", "#t0", "#t1", "#t2");
        tmp_var_active = true;
    }

    // print rest of the tree
    arithmetic_print(AS_Tree->expr, AS_Tree->token->content, true);
}

/**
 * @brief 
 * 
 */
void do_ifelse(stx_node_t* AS_Tree) 
{   
    // variables
    static int if_label_cnt = 0;    int if_label = if_label_cnt;
    static int else_label_cnt = 0;  int else_label = else_label_cnt;
    if_label_cnt++; else_label_cnt++;

    // #0 - check if tree is not empty
    if ((AS_Tree == NULL) || (AS_Tree->type != IFELSE)) {
        return;
    }

    // if with else condition
    if (AS_Tree->items_len > 0) {
        // if condition
        logic_print(AS_Tree->expr, "#else", else_label);
        generate_code(AS_Tree->items[0]);                   // fix block_item problems
        printf("JUMP #if%d\n", if_label);
        
        // else condition
        printf("LABEL #else%d\n", else_label);
        generate_code(AS_Tree->items[1]);
        printf("LABEL #if%d\n", if_label);                  // fix block_item problems

    // if without else condition
    } else {
        // if condition
        logic_print(AS_Tree->expr, "#else", else_label);
        generate_code(AS_Tree->items[0]);                   // fix block_item problems
        printf("LABEL #else%d\n", else_label);
    }
}


/**
 * @brief 
 * 
 */
void do_whileblock(stx_node_t* AS_Tree)
{
    // variables
    static int label_counter = 0;
    int func_label = label_counter;
    label_counter++;

    // #0 - check if tree is not empty
    if ((AS_Tree == NULL) || (AS_Tree->type != WHILEBLOCK)) {
        return;
    }

    // #1 - Create label
    printf("LABEL %s%d\n", "#while", func_label);

    // #2 - print JUMPIF
    logic_print(AS_Tree->expr, "#endwhile", func_label);
    generate_code(AS_Tree);                                 // fix block_item problems

    // #3 - end label
    printf("JUMP %s%d\n", "#while", func_label);
    printf("LABEL %s%d\n", "#endwhile", func_label);
}


/* - - - - - - - - - - - - - */
/*  AP_TREE PRINT FUNCTIONS  */
/* - - - - - - - - - - - - - */

/**
 * Recursively prints arithmetic expression from Precedence Tree
 *  - used in function (do_assignstmt,)
 * 
 * @param AP_Tree Precedence tree, from which instruction will be created
 * @param assigned_var Variable, for which are the instructions created
 * @param reset Used to indicate if the funtion is started recursively or not. - (true = start, false = recursive)
 */
void arithmetic_print(expr_node_t* AP_Tree , char* assigned_var, bool reset) {

    // #0 - variable initialization
    const char instruction_set[6][7] = { "SUB", "ADD", "MUL", "DIV", "CONCAT", };   // instruction set
    char tmp_variables[10][10] = { "#t0", "#t1", "#t2", };                          // temporary variables
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
    arithmetic_print(AP_Tree->left, var_left, false);
    arithmetic_print(AP_Tree->right, var_right, false);
    
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

void logic_print(expr_node_t* AP_Tree, char* label, int label_num) 
{
    // #0 - variables
    char *left_term = NULL, *right_term = NULL;
    static bool function_start = true;

    // #1 - function check and reset
    if (AP_Tree == NULL) {
        return;
    }
    // printing tmp helper variables
    if (function_start == true) {
        printf("DEFVAR LF@%s\n", "#bool0");
        printf("DEFVAR LF@%s\n", "#bool1");
        function_start = false;
    }

    // #2 - getting sub-trees values
    if ((AP_Tree->left != NULL) && AP_Tree->right != NULL) {
        left_term = AP_Tree->left->token.content;
        right_term = AP_Tree->right->token.content;
    } else {
        return;
    }

    // #3 - printing 
    switch (AP_Tree->token.type)
    {
    case EQUAL:
        printf("JUMPIFNEQ %s%d %s %s\n", label, label_num, left_term, right_term);
        break;
    
    case NOT_EQUAL:
        printf("JUMPIFEQ %s%d %s %s\n", label, label_num, left_term, right_term);
        break;
    
    case GREATER_EQUAL:
        printf("GT %s %s %s\n", "#bool1", left_term, right_term);
        printf("EQ %s %s %s\n", "#bool1", left_term, right_term);
        printf("OR %s %s %s\n", "#bool0", "#bool0", "#bool1");
        printf("JUMPIFGNEQ %s%d %s %s\n", label, label_num, "#bool0", "bool=true");
        break;

    case LESS_EQUAL:
        printf("LT %s %s %s\n", "#bool0", left_term, right_term);
        printf("EQ %s %s %s\n", "#bool1", left_term, right_term);
        printf("OR %s %s %s\n", "#bool0", "#bool0", "#bool1");
        printf("JUMPIFGNEQ %s%d %s %s\n", label, label_num, "#bool0", "bool=true");
        break;

    case GREATER:
        printf("GT %s %s %s\n", "#bool0", left_term, right_term);
        printf("JUMPIFGNEQ %s%d %s %s\n", label, label_num, "#bool0", "bool=true");
        break;

    case LESS:
        printf("LT %s %s %s\n", "#bool0", left_term, right_term);
        printf("JUMPIFGNEQ %s%d %s %s\n", label, label_num, "#bool0", "bool=true");
        break;

    default:
        break;
    }

}
