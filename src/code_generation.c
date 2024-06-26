/**
 * Project: Implementace překladače imperativního jazyka IFJ22
 * 
 * @file code_generation.c
 * @brief End code generation
 * 
 * @author Nikolas Nosál <xnosal01>
 * @author Adam Mrkva <xmrkva04>
 * @author Rostislav Navrátil <xnavra72>
 * @author David Nevrlka <xnevrl00>
 */

#include "code_generation.h"

#define sym_table_start_size 1000
#define buffer_size 1000

/* - - - - - - - - - - - - */
/*     GLOBAL VARIABLES    */
/* - - - - - - - - - - - - */

// symbol table
//STList *symbol_table;

// used in arithmetics
StackPtr left_stack = NULL, right_stack = NULL, float_stack = NULL;

// TEMPORARY - used get info if AP_Tree has float
bool is_float = false;



/* - - - - - - - - - - */
/*    MAIN FUNCTION    */
/* - - - - - - - - - - */

// working on this library 
void generate_code(stx_node_t *AS_Tree) {
    
    // init
    STList *symbol_table = ST_Init(sym_table_start_size);
    
    // create code
    do_block(AS_Tree, symbol_table, false);

    // end
    ST_Dispose(&symbol_table);
}



/* - - - - - - - - - - - -*/
/*   ITEM TYPE FUNCTIONS  */
/* - - - - - - - - - - - -*/

/**
 * @brief Generates instruction code from AST
 * 
 * @param AS_Tree Syntax tree
 * @param symbol_table Symbol table
 */
void do_block(stx_node_t *AS_Tree, STList *symbol_table, bool is_func) // a.k.a do_block
{
    // #0 - check if tree is not empty
	if (AS_Tree == NULL) {
		return;
	}

    // #1 - variables
    static bool code_generation_start = true;
    bool main_branch = false;

    // #2 - generate code init
    if (code_generation_start) {
        code_generation_start = false;
        main_branch = true;

        // init functions
        left_stack = stack_init();
        right_stack = stack_init();
        float_stack = stack_init();

        // init print
        printf("# - HEADER - #\n");
        printf(".IFJcode22\n");
        printf("CREATEFRAME\n");
        printf("PUSHFRAME\n");

        printf("# - ARITHMETIC GLOBAL VARIABLES - #\n");
        printf("DEFVAR GF@%%tmp0\n");
        printf("DEFVAR GF@%%tmp1\n");
        printf("DEFVAR GF@%%tmp2\n");

        printf("# - BOOL GLOBAL VARIABLES - #\n");
        printf("DEFVAR GF@%%bool0\n");
        printf("DEFVAR GF@%%bool1\n");
        printf("DEFVAR GF@%%bvar0\n");
        printf("DEFVAR GF@%%bvar1\n");

        printf("# - FUNCTION GLOBAL VARIABLES - #\n");
        printf("DEFVAR GF@%%freturn\n");
        printf("DEFVAR GF@%%fparam0\n");
        printf("DEFVAR GF@%%fparam1\n");
        printf("DEFVAR GF@%%fparam2\n");
        printf("DEFVAR GF@%%fparam3\n");
        printf("DEFVAR GF@%%fparam4\n");
        printf("# =============================== #\n");
        
        // print langauge defined functions
        func_substring();
        func_intval();
        func_floatval();
    }

    // #3 - go through all nodes in tree recursively
    if (AS_Tree->items_len != 0) {
        for (int i = 0; i < AS_Tree->items_len; i++) {
            do_assignexpr(AS_Tree->items[i], symbol_table);
            do_expr(AS_Tree->items[i]);
            do_retexpr(AS_Tree->items[i]);
            do_ifelse(AS_Tree->items[i], symbol_table);
            do_whileblock(AS_Tree->items[i], symbol_table);
            do_funcblock(AS_Tree->items[i]);
        }
    }

    // #4 - if function print return
    if (is_func == true) {
        
        // dispose of function symtable
        ST_Dispose(&symbol_table);

        // print function end
        printf("POPFRAME\n");
        printf("RETURN\n");
    }

    // #5 - generate code end
    if (main_branch == true) {
        // free memory
        stack_dispose(&left_stack);
        stack_dispose(&right_stack);
        stack_dispose(&float_stack);

        // end print
        printf("POPFRAME\n");
    }
}


/**
 *  - Assignes value to variable
 * 
 * @param AS_Tree - Abstract syntax tree
 * @param symbol_table - Symbol table
 */
void do_assignexpr(stx_node_t* AS_Tree, STList *symbol_table) {
    

    // #0 - check if tree is not empty
    if ((AS_Tree == NULL) || (AS_Tree->type != ASSIGNEXPR)) {
        return;
    }

    // #2 check if there are floats
    arithmetic_print_floatcheck(AS_Tree->expr, symbol_table, true);

    // #2 - defines variable
    if ((ST_ElementExists(symbol_table, AS_Tree->token->content)) == false) {
        
        // adding variable to symbol table
        ST_CreateResize(&symbol_table, AS_Tree->token->content);
        STElementDataPtr data = ST_DataGet(symbol_table, AS_Tree->token->content);
        data->type = (is_float == true) ? FRACTION : NUMBER;
        
        // defining new variable
        printf("DEFVAR LF@%s\n", AS_Tree->token->content);
    
    // #2 change type of variable
    } else {
        // get variable
        STElementDataPtr data = ST_DataGet(symbol_table, AS_Tree->token->content);
        
        // if number and (float = true), change type
        if ((data->type == NUMBER) && (is_float == true)) {
            data->type = FRACTION;
        }

        // if float and number = true, change type
        if ((data->type == FRACTION) && (is_float == false)) {
            data->type = NUMBER;
        }
    }

    // #3 - retype variable if needed
    if (is_float == true) {
        arithmetic_print_float_retype(AS_Tree->expr, symbol_table);
    }

    // #4 - printing move instruction
    if ((AS_Tree->expr->left == NULL) && (AS_Tree->expr->right == NULL)) {
        
        // adding to stack
        print_stack(AS_Tree->expr, false);

        // is function
        if (AS_Tree->expr->token.type == IDENTIFIER) {
            
            // function is undefined
            if (is_defined_func(AS_Tree->expr) == -1) {
                func_print(AS_Tree->expr, NULL);
                printf("MOVE LF@%s GF@%%freturn\n", AS_Tree->token->content);
            
            // function is defined
            } else {
                func_print(AS_Tree->expr, AS_Tree->token->content);
            }

        // is assignment
        } else {
            printf("MOVE LF@%s %s\n", AS_Tree->token->content, stack_pop(&right_stack));
        }
    
    // #5 - else solve arithmetic expression
    } else {

        // setup correct format
        char buffer[buffer_size]; 
        strcpy(buffer, "LF@");
        strcat(buffer, AS_Tree->token->content);

        // solve arithmetic expression
        arithmetic_print(AS_Tree->expr, buffer, true);
    }

    // #6 - clean memory - dispose of stacks
    stack_dispose(&left_stack);
    stack_dispose(&right_stack);
}


/**
 * @brief - Prints function/expression
 * 
 * @param AS_Tree - syntax tree
 */
void do_expr(stx_node_t* AS_Tree) 
{   
    // check if tree is not empty
    if ((AS_Tree == NULL) || (AS_Tree->type != EXPR)) {
        return;
    }

    // call function
    if (AS_Tree->expr->token.type == IDENTIFIER) {
        func_print(AS_Tree->expr, NULL);
    }

}

void do_retexpr(stx_node_t* AS_Tree) 
{   
    // check if tree is not empty
    if ((AS_Tree == NULL) || (AS_Tree->type != RETEXPR)) {
        return;
    }

    if (AS_Tree->expr != NULL) {
        // print rest of the tree
        if (AS_Tree->expr->left == NULL && AS_Tree->expr->right == NULL) { 
            // print move
            print_stack(AS_Tree->expr, false);
            printf("MOVE GF@%%freturn %s\n", stack_pop(&right_stack));
        } else {
            // print arithmetic
            arithmetic_print(AS_Tree->expr, "GF@\%freturn", true);
        }
    }

    // print return
    printf("POPFRAME\n");
    printf("RETURN\n");
}


/**
 * @brief - Prints if-else statement
 * 
 * @param AS_Tree - syntax tree
 * @param symbol_table - symbol table
 */
void do_ifelse(stx_node_t* AS_Tree, STList *symbol_table) 
{   
    // #0 - check if tree is not empty
    if ((AS_Tree == NULL) || (AS_Tree->type != IFELSE)) {
        return;
    }

    // check copies
    check_if_defined(AS_Tree, symbol_table); 

    // variables
    static int if_label_cnt = 0, else_label_cnt = 0;  
    int if_label = if_label_cnt, else_label = else_label_cnt;
    if_label_cnt++; else_label_cnt++;


    // if with else condition
    if (AS_Tree->items_len == 2) {
        // if condition
        logic_print(AS_Tree->expr, symbol_table, "\%else", else_label);
        do_block(AS_Tree->items[0], symbol_table, false);            
        printf("JUMP %%if%d\n", if_label);
        
        // else condition
        printf("LABEL %%else%d\n", else_label);
        do_block(AS_Tree->items[1], symbol_table, false);
        printf("LABEL %%if%d\n", if_label);                
    }

    // if without else condition
    if (AS_Tree->items_len == 1) {
        // if condition
        logic_print(AS_Tree->expr, symbol_table, "\%else", else_label);
        do_block(AS_Tree->items[0], symbol_table, false);                  
        printf("LABEL %%else%d\n", else_label);
    }
}


/**
 * @brief - Prints while statement
 * 
 * @param AS_Tree - syntax tree
 * @param symbol_table - symbol table
 */
void do_whileblock(stx_node_t* AS_Tree, STList *symbol_table)
{
    // #0 - check if tree is not empty
    if ((AS_Tree == NULL) || (AS_Tree->type != WHILEBLOCK)) {
        return;
    }

    // variables
    static int label_counter = 0;
    int func_label = label_counter;
    label_counter++;

    // #1 - check copies
    check_if_defined(AS_Tree, symbol_table);

    // #2 - Create label
    printf("LABEL %s%d\n", "\%while", func_label);

    // #3 - print JUMPIF
    logic_print(AS_Tree->expr, symbol_table,"\%endwhile", func_label);
    do_block(AS_Tree, symbol_table, false);                               

    // #4 - end label
    printf("JUMP %s%d\n", "\%while", func_label);
    printf("LABEL %s%d\n", "\%endwhile", func_label);
}

/**
 * @brief - Prints function declaration
 * 
 * @param AS_Tree - syntax tree
 */
void do_funcblock(stx_node_t* AS_Tree)
{
    // #0 - check if tree is not empty
    if ((AS_Tree == NULL) || (AS_Tree->type != FUNCBLOCK)) {
        return;
    }

    // #1 variables
    static int func_def = 0;  

    // #2 - Print label
    printf("# - %s - #\n", AS_Tree->items[0]->token->content);
    printf("JUMP %%fjump%d\n", func_def);
    printf("LABEL func%s\n", AS_Tree->items[0]->token->content);
    printf("PUSHFRAME\n");


    // #3 - create symbol table for function
    STList *func_symbol_table = ST_Init(sym_table_start_size);

    // #4 - assign variables
    for (int i = 0; i < AS_Tree->items[1]->items_len; i++) {

        // add variables to symbol table
        ST_CreateResize(&func_symbol_table, AS_Tree->items[1]->items[i]->items[0]->token->content);
        STElementDataPtr data = ST_DataGet(func_symbol_table, AS_Tree->items[1]->items[i]->items[0]->token->content);
        token_type param_type = AS_Tree->items[1]->items[i]->token->type;
        
        switch (param_type) {
            // number
            case INT:
                data->type = NUMBER;
                break;
            // fraction
            case FLOAT:
                data->type = FRACTION;
                break;
            // string
            case STRING:
                data->type = TEXT;
                break;
            // something else
            default:
                break;
        }

        printf("DEFVAR LF@%s\n", AS_Tree->items[1]->items[i]->items[0]->token->content);
        printf("MOVE LF@%s LF@%%fvar%d\n",AS_Tree->items[1]->items[i]->items[0]->token->content, i);
    }
    

    // #5 - now do block code
    do_block(AS_Tree->items[2], func_symbol_table, true);
    printf("LABEL %%fjump%d\n", func_def);
    printf("# ======================== #\n");
    func_def++;
}


/* - - - - - - - - - - - - - */
/*  AP_TREE PRINT FUNCTIONS  */
/* - - - - - - - - - - - - - */

/**
 * @brief - Recursively prints arithmetic expression from Precedence Tree
 * 
 * @param AP_Tree Precedence tree, from which instruction will be created
 * @param assigned_var Variable, for which are the instructions created
 * @param reset Used to indicate if the funtion is started recursively or not. - (true = start, false = recursive)
 */
void arithmetic_print(expr_node_t* AP_Tree , char* assigned_var, bool reset) {

    // #0 - variable initialization
    const char instruction_set[6][7] = { "SUB", "ADD", "MUL", "DIV", "CONCAT", };   // instruction set
    char tmp_variables[10][10] = { "GF@\%tmp0", "GF@\%tmp1", "GF@\%tmp2", };        // temporary variables
    char *var_left, *var_right;                                                     // variables for left and right side of the tree
    static int var_offset = 0;                                                      // offset for temporary variables
    

    // #1 - function check and reset
    if (AP_Tree == NULL) {
        return;
    } 

    // #2 - function reset
    if (reset == true) {
        
        // reset offset
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
        
        // is function call
        if ((AP_Tree->left->token.type == IDENTIFIER) || (AP_Tree->right->token.type == IDENTIFIER)) {

            //save pre-recursion state
            char *tmp_left_term = stack_pop(&left_stack), *tmp_right_term = stack_pop(&right_stack);
            int tmp_var_offset = var_offset;

            // left terminal is the function
            if ((AP_Tree->left->token.type == IDENTIFIER) && (AP_Tree->right->token.type != IDENTIFIER)) {
                func_print(AP_Tree->left, NULL);
                
                // setting up functions
                var_offset = tmp_var_offset;
                var_left = tmp_variables[0 + var_offset];
                stack_push_ptr(&right_stack, tmp_right_term);

                // printing
                printf("MOVE %s GF@%%freturn\n", var_left);
                printf("%s %s %s %s\n", instruction_set[AP_Tree->token.type - 13], assigned_var, var_left, stack_pop(&right_stack));
            }
            
            // right terminal is the function
            if ((AP_Tree->right->token.type == IDENTIFIER) && (AP_Tree->left->token.type != IDENTIFIER)) {
                func_print(AP_Tree->right, NULL);

                // setting up variables for printing
                var_offset = tmp_var_offset;
                var_right = tmp_variables[0 + var_offset];
                stack_push_ptr(&left_stack, tmp_left_term);

                // printing
                printf("MOVE %s GF@%%freturn\n", var_right);
                printf("%s %s %s %s\n", instruction_set[AP_Tree->token.type - 13], assigned_var, stack_pop(&left_stack), var_right);
            }

            // both terminals are functions
            if ((AP_Tree->right->token.type == IDENTIFIER) && (AP_Tree->left->token.type == IDENTIFIER)) {
                func_print(AP_Tree->left, NULL);

                // left side -> setting up variables and printing 
                var_offset = tmp_var_offset;
                var_left = tmp_variables[0 + var_offset];
                printf("MOVE %s GF@%%freturn\n", var_left);

                // right side -> setting up variables and printing 
                func_print(AP_Tree->right, NULL);
                var_offset = tmp_var_offset;
                var_right = tmp_variables[1 + var_offset];
                printf("MOVE %s GF@%%freturn\n", var_right);

                // printing the result of tmp variables
                printf("%s %s %s %s\n", instruction_set[AP_Tree->token.type - 13], assigned_var, var_left, var_right);
            }

        // is arithmetic expression
        } else {

            // both are not-terminals
            if ((AP_Tree->left->is_terminal == false) && (AP_Tree->right->is_terminal == false)) {
                
                printf("%s %s %s %s\n", instruction_set[AP_Tree->token.type - 13], assigned_var, var_left, var_right);

                // setting the offset for temporary variables
                (var_offset == 0) ? (var_offset = 1) : (var_offset = 0);
            }

            // both are terminals
            if ((AP_Tree->left->is_terminal == true) && (AP_Tree->right->is_terminal == true)) {
                
                // adding to stack
                print_stack(AP_Tree->left, true);
                print_stack(AP_Tree->right, false);

                // printing
                printf("%s %s %s %s\n", instruction_set[AP_Tree->token.type - 13], assigned_var, stack_pop(&left_stack), stack_pop(&right_stack));
            }

            // left is terminal
            if ((AP_Tree->left->is_terminal == true) && (AP_Tree->right->is_terminal == false)) {
                
                // adding to stack
                print_stack(AP_Tree->left, true);
                
                // printing
                printf("%s %s %s %s\n", instruction_set[AP_Tree->token.type - 13], assigned_var, stack_pop(&left_stack), var_right);    
            }

            // right is terminal
            if ((AP_Tree->left->is_terminal == false) && (AP_Tree->right->is_terminal == true)) {
                
                // adding to stack
                print_stack(AP_Tree->right, false);

                // printing
                printf("%s %s %s %s\n", instruction_set[AP_Tree->token.type - 13], assigned_var, var_left, stack_pop(&right_stack));  
            }
        }
    }
}


/**
 * @brief Sends string to the stack -> stack changes it to desired format
 * 
 * @param AP_Tree - expression tree
 * @param left_side - true if left side of the expression, false otherwise
 */
void print_stack(expr_node_t* AP_Tree, bool left_side)
{   
    char buffer[buffer_size] = { 0 }, sec_buffer[buffer_size] = { 0 };

    // TEMPORATY - quick fix will probably be deleted
    if ((is_float == true) && (AP_Tree->token.type == NUMBER || AP_Tree->token.type == FRACTION)) {
        if (left_side == true) {
            strcpy(buffer, "float@");
            sprintf(sec_buffer, "%a", atof(AP_Tree->token.content));
            strcat(buffer, sec_buffer);
            stack_push_string(&left_stack, buffer);
        } else {
            strcpy(buffer, "float@");
            sprintf(sec_buffer, "%a", atof(AP_Tree->token.content));
            strcat(buffer, sec_buffer);
            stack_push_string(&right_stack, buffer);
        }

        return;
    }


    // left side of the tree token
    if (left_side == true) {
        switch (AP_Tree->token.type) {
            // number
            case NUMBER:
                strcpy(buffer, "int@");
                strcat(buffer, AP_Tree->token.content);
                stack_push_string(&left_stack, buffer);
                break;
            // fraction
            case FRACTION:
                strcpy(buffer, "float@");
                sprintf(sec_buffer, "%a", atof(AP_Tree->token.content));
                strcat(buffer, sec_buffer);
                stack_push_string(&left_stack, buffer);
                break;
            // variable
            case VARIABLE:
                strcpy(buffer, "LF@");
                strcat(buffer, AP_Tree->token.content);
                stack_push_string(&left_stack, buffer);
                break;
            // string
            case TEXT:
                strcpy(buffer, "string@");
                strcpy(sec_buffer, AP_Tree->token.content);
                format_string_print(sec_buffer);
                strcat(buffer, sec_buffer);
                stack_push_string(&left_stack, buffer);
                break;
            case NIL:
                strcpy(buffer, "nil@nil");
                stack_push_string(&left_stack, buffer);
                break;

            default:
                break;
        }
    

    // right side of the tree
    } else {
        switch (AP_Tree->token.type) {
            // number
            case NUMBER:
                strcpy(buffer, "int@");
                strcat(buffer, AP_Tree->token.content);
                stack_push_string(&right_stack, buffer);
                break;
            // fraction
            case FRACTION:
                strcpy(buffer, "float@");
                sprintf(sec_buffer, "%a", atof(AP_Tree->token.content));
                strcat(buffer, sec_buffer);
                stack_push_string(&right_stack, buffer);
                break;
            // variable
            case VARIABLE:
                strcpy(buffer, "LF@");
                strcat(buffer, AP_Tree->token.content);
                stack_push_string(&right_stack, buffer);
                break;
            // string
            case TEXT:
                strcpy(buffer, "string@");
                strcpy(sec_buffer, AP_Tree->token.content);
                format_string_print(sec_buffer);
                strcat(buffer, sec_buffer);
                stack_push_string(&right_stack, buffer);
                break;
            case NIL:
                strcpy(buffer, "nil@nil");
                stack_push_string(&right_stack, buffer);
                break;
            
            default:
                break;
        }
    }
}

/**
 * @brief Formats string to desired string format -> used by print_stack
 * 
 * @param buffer - string to be formatted
 */
void format_string_print(char *buffer) {

    // variables
    char replace_buffer[buffer_size] = { 0 }, sym_buffer[buffer_size] = { 0 };
    int i = 0;

    // going through the string
    while(buffer[i] != 0){

        // getting the symbol
        if(buffer[i] > ' ' && buffer[i] != '\\'){
            sprintf(sym_buffer, "%c",buffer[i]);
        } else {
            sprintf(sym_buffer, "\\%.3d",buffer[i]);
        }
        i++;

        // adding to the buffer
        strcat(replace_buffer, sym_buffer);
    }

    // copying the buffer
    strcpy(buffer, replace_buffer);
}



/**
 * @brief Checks if the variable is defined in the symbol table
 * 
 * @param stx_while - while/if loop node
 * @param symbol_table - symbol table
 */
void check_if_defined(stx_node_t* stx_while, STList *symbol_table)
{   
    // goind through all the items in the while loop
    for (int i = 0; i < stx_while->items_len; i++) {
        
        // getting the type of the item
        item_type item = stx_while->items[i]->type;

        // adds variable to stack and while stack
        if (item == ASSIGNEXPR) {
            if (stx_while->items[i]->token->content != NULL) {
                if ((ST_ElementExists(symbol_table, stx_while->items[i]->token->content)) == false) {

                    // adding variable to symbol table
                    ST_CreateResize(&symbol_table, stx_while->items[i]->token->content);
                    STElementDataPtr data = ST_DataGet(symbol_table, stx_while->items[i]->token->content);
                    data->type = NUMBER;

                    // defining new variable
                    printf("DEFVAR LF@%s\n", stx_while->items[i]->token->content);
                }
            }
        }
        
        // continue recursively to search for variables
        if (item == IFELSE || item == WHILEBLOCK || item == BLOCK) {

            // recursive call
            if (stx_while->items[i] != NULL) {
                check_if_defined(stx_while->items[i], symbol_table);
            }
        }
    }
}


/**
 * @brief Checks if the variable is float
 * 
 * @param AP_Tree - arithmetic expression tree
 * @param symbol_table - symbol table
 */
void arithmetic_print_floatcheck(expr_node_t* AP_Tree, STList *symbol_table, bool reset)
{   
    // reseting the function call
    if (reset == true) {
        is_float = false;
    }

    // recursive call left
    if (AP_Tree->left != NULL) {
        arithmetic_print_floatcheck(AP_Tree->left, symbol_table, false);
    }

    // recursive call right
    if (AP_Tree->right != NULL) {
        arithmetic_print_floatcheck(AP_Tree->right, symbol_table, false);
    }

    // float number -> float
    if (AP_Tree->token.type == FRACTION) {
        is_float = true;
        return;
    }

    // division -> float
    if (AP_Tree->token.type == SLASH) {
        is_float = true;
        return;
    }

    // float variable -> float
    if (AP_Tree->token.type == VARIABLE) {
        
        // get data type
        if (ST_ElementExists(symbol_table, AP_Tree->token.content)) {
            STElementDataPtr data = ST_DataGet(symbol_table, AP_Tree->token.content);

            // evaluate data type
            if (data->type == FRACTION) {
                is_float = true;
                return;
            }
        }
    }
}

/**
 * @brief Retypes variables to float
 * 
 * @param AP_Tree - arithmetic expression tree
 * @param symbol_table - symbol table
 */
void arithmetic_print_float_retype(expr_node_t* AP_Tree, STList *symbol_table)
{
    // recursive call left
    if (AP_Tree->left != NULL) {
        arithmetic_print_float_retype(AP_Tree->left, symbol_table);
    }

    // recursive call right
    if (AP_Tree->right != NULL) {
        arithmetic_print_float_retype(AP_Tree->right, symbol_table);
    }

    if (AP_Tree->token.type == VARIABLE) {
        
        // get data type
        if (ST_ElementExists(symbol_table, AP_Tree->token.content)) {
            STElementDataPtr data = ST_DataGet(symbol_table, AP_Tree->token.content);

            // change data type expression
            if (data->type == NUMBER) {
                printf("INT2FLOAT LF@%s LF@%s\n", AP_Tree->token.content, AP_Tree->token.content);
                stack_push_string(&float_stack, AP_Tree->token.content);
                data->type = FRACTION;
            }
        }
    } 
}

/**
 * @brief Retypes variables back to int
 * 
 * @param symbol_table - symbol table
 */
void arithmetic_print_float_untype(STList *symbol_table)
{
    // poping the stack
    while(float_stack->size > 0) {
        // get last retyped variables
        char* buffer = stack_pop(&float_stack);

        // change data type expression
        if (ST_ElementExists(symbol_table, buffer)) {
            STElementDataPtr data = ST_DataGet(symbol_table, buffer);
            data->type = NUMBER;
            printf("FLOAT2INT LF@%s LF@%s\n", buffer, buffer);
        }
    }
}

/**
 * @brief Prints logic expression
 * 
 * @param AP_Tree - logic expression tree
 * @param symbol_table - symbol table
 * @param label - label name string
 * @param label_num - number of variable
 */
void logic_print(expr_node_t* AP_Tree, STList *symbol_table, char* label, int label_num) 
{
    // #0 - variables
    bool left_expr_float = false, right_expr_float = false;

    // #1 - function check and reset
    if (AP_Tree == NULL) {
        return;
    }

    // #2 - getting sub-trees values
    if ((AP_Tree->left != NULL) && AP_Tree->right != NULL) {
        

        // checking left float
        arithmetic_print_floatcheck(AP_Tree->left, symbol_table, true);
        if (is_float == true) {
            left_expr_float = true;
        }

        // assigment (left)
        if (AP_Tree->left->left == NULL && AP_Tree->left->right == NULL) {

            // adding to stack
            print_stack(AP_Tree->left, true);
            printf("MOVE GF@%%bvar0 %s\n", stack_pop(&left_stack));

        // arithmetic expression (left)
        } else {
            // retype variable if needed
            if (left_expr_float == true) {
                arithmetic_print_float_retype(AP_Tree->left, symbol_table);
            }

            // arithemetic expression
            arithmetic_print(AP_Tree->left, "GF@\%bvar0", true);
        }


        // checking right float
        arithmetic_print_floatcheck(AP_Tree->right, symbol_table, true);
        if (is_float == true) {
            right_expr_float = true;
        }   

        // assigment (right)
        if (AP_Tree->right->left == NULL && AP_Tree->right->right == NULL) {
            
            // adding to stack
            print_stack(AP_Tree->right, false);
            printf("MOVE GF@%%bvar1 %s\n", stack_pop(&right_stack));

        // arithmetic expression (right)   
        } else {

            // retype variable if needed
            if (right_expr_float == true) {
                arithmetic_print_float_retype(AP_Tree->right, symbol_table);
            }
            
            // arithemetic expression
            arithmetic_print(AP_Tree->right, "GF@\%bvar1", true);
        }
    } else {
        return;
    }

    // #3 - additional retype
    if (left_expr_float == true && right_expr_float == false) {
        printf("INT2FLOAT GF@%%bvar1 GF@%%bvar1\n");
    }

    if (left_expr_float == false && right_expr_float == true) {
        printf("INT2FLOAT GF@%%bvar0 GF@%%bvar0\n");
    }

    //printf("left: %d, right: %d\n", left_expr_float, right_expr_float);

    // #4 - solving the bool expresion - printing 
    switch (AP_Tree->token.type)
    {
    case TYPE_EQUAL:
        printf("EQ GF@%%bool0 GF@%%bvar0 GF@%%bvar1\n");
        break;
    
    case TYPE_NOT_EQUAL:
        printf("EQ GF@%%bool0 GF@%%bvar0 GF@%%bvar1\n");
        printf("EQ GF@%%bool0 GF@%%bool0 bool@false\n");
        break;

    case GREATER_EQUAL:
        printf("GT GF@%%bool1 GF@%%bvar0 GF@%%bvar1\n");
        printf("EQ GF@%%bool1 GF@%%bvar0 GF@%%bvar1\n");
        printf("OR GF@%%bool0 GF@%%bool0 GF@%%bool1\n");
        break;

    case LESS_EQUAL:
        printf("LT GF@%%bool0 GF@%%bvar0 GF@%%bvar1\n");
        printf("EQ GF@%%bool1 GF@%%bvar0 GF@%%bvar1\n");
        printf("OR GF@%%bool0 GF@%%bool0 GF@%%bool1\n");
        break;

    case GREATER:
        printf("GT GF@%%bool0 GF@%%bvar0 GF@%%bvar1\n");
        break;

    case LESS:
        printf("LT GF@%%bool0 GF@%%bvar0 GF@%%bvar1\n");
        break;

    default:
        break;
    }


    // #5 - untype and jumping to if - printing
    if (left_expr_float == true || right_expr_float == true) {
    arithmetic_print_float_untype(symbol_table);
    }
    printf("JUMPIFNEQ %s%d GF@%%bool0 bool@true\n", label, label_num);
}

/**
 * @brief Prints function call
 * 
 * @param AP_Tree - expression tree
 * @param token_content - token content for defined functions
 */
void func_print(expr_node_t* AP_Tree, char *token_content) {
    
    // #0 - variables
    static int func_call = 0;

    // #1 - checking if the function is language-defined
    int func_num = is_defined_func(AP_Tree);
    

    // #2 - if defined, does the function
    switch (func_num) {
        //function reads - reads string from stdin
        case 0: func_reads(token_content);
            break;
        //function readi - reads int from stdin
        case 1: func_readi(token_content);
            break;
        //function readf - reads float from stdin
        case 2: func_readf(token_content);
            break;
        //function write - writes to stdout
        case 3: func_write(AP_Tree);
            break;
        //function strlen - returns lenght of given string
        case 4: func_strlen(AP_Tree, token_content);
            break;
        //function ord - returns ASCII of first character of the given string
        case 5: func_ord(AP_Tree, token_content);
            break;
        //function chr - returns character represented by given ASCII value
        case 6: func_chr(AP_Tree, token_content);
            break;
        //functions stral - returns given variable as string
        case 7: func_strval(AP_Tree, token_content);
            break;     

        // user defined
        default:

            // init function variables
            for (int i = 0; i < AP_Tree->params_len; i++) {

                // function call which calls functions
                if (AP_Tree->token.type == IDENTIFIER && AP_Tree->params[i]->token.type == IDENTIFIER) {
                    
                    // recursive call
                    func_call++;
                    func_print(AP_Tree->params[i], token_content);

                    // printing
                    printf("DEFVAR LF@%%ffvar%d\n", func_call);
                    printf("MOVE LF@%%ffvar%d GF@%%freturn\n", func_call);

                    // saving to buffer
                    char buffer[buffer_size];
                    sprintf(buffer, "LF@%%ffvar%d", func_call);
                    stack_push_string(&right_stack, buffer);

                    printf("\n");

                    if (i == AP_Tree->params_len - 1) {
                        
                        // creating frame for last call
                        printf("CREATEFRAME\n");
                        for (int i = 0; i < AP_Tree->params_len; i++) {
                            char *ffvar = stack_pop(&right_stack);
                            printf("DEFVAR TF@%%fvar%d\n", i);
                            printf("MOVE TF@%%fvar%d %s\n", i, ffvar);
                        }

                        // printing call instruction
                        printf("CALL func%s\n", AP_Tree->token.content);
                    }

                // normal function call
                } else {

                    // printing createframe instruction
                    if (i == 0) {
                        printf("CREATEFRAME\n");
                    }

                    // printing move instruction
                    if ((AP_Tree->params[i]->left == NULL) && (AP_Tree->params[i]->right == NULL)) {

                        // define variables
                        printf("DEFVAR TF@%%fvar%d\n", i);

                        // adding to stack and printing from it
                        print_stack(AP_Tree->params[i], false);
                        printf("MOVE TF@%%fvar%d %s\n", i, stack_pop(&right_stack));

                    // else solve arithmetic expression
                    } else {
                            // define variables
                            printf("DEFVAR TF@%%fvar%d\n", i);
                            // appending the number to the variable name
                            char buffer[buffer_size];
                            sprintf(buffer, "TF@%%fvar%d", i);

                            // recurive call
                            arithmetic_print(AP_Tree->params[i], buffer, false);
                    }

                    // printing call instruction
                    if (i == AP_Tree->params_len - 1) {
                        printf("CALL func%s\n", AP_Tree->token.content);
                    }
                }
                
            }
            
            break;
        }
}



/* - - - - - - - - - - - */
/*  FUNC CALL FUNCTIONS  */
/* - - - - - - - - - - - */

/**
 * @brief Checks if the function is defined
 * 
 * @param token_content - token content for defined functions
 */
int is_defined_func(expr_node_t* AP_Tree) 
{
    const char def_func[20][8] = { "reads", "readi", "readf", "write", "strlen", "ord", "chr", "strval"};
    const int def_func_num = 8;
    int func_num = 0;
    bool flag = false;

    //loop trough builtin functions
    do {
        if ((strcmp(def_func[func_num], AP_Tree->token.content)) == 0 ) {
            flag = true;
            break;
        }
        func_num++;
    } while(func_num < def_func_num);

    // if the function is defined, returns its number
    if(flag == true) {
        return func_num;
    } else {
        return -1;
    }
}


/**
 * @brief Function reads - reads string from stdin
 * 
 * @param token_content - token content for defined functions
 */
void func_reads(char *token_content) {
    printf("READ LF@%s string\n", token_content);
}

/**
 * @brief Function readi - reads int from stdin
 * 
 * @param token_content - token content for defined functions
 */
void func_readi(char *token_content) {
    printf("READ LF@%s int\n", token_content);
}

/**
 * @brief Function readf - reads float from stdin
 * 
 * @param token_content - token content for defined functions
 */
void func_readf(char *token_content) {
    printf("READ LF@%s float\n", token_content);
}

/**
 * @brief Function write - writes given variable to stdout
 * 
 * @param AP_Tree - expression tree
 */
void func_write(expr_node_t* AP_Tree) 
{
    for (int i = 0; i < AP_Tree->params_len; i++) {

        // if func call
        if (AP_Tree->params[i]->token.type == IDENTIFIER) {
            // go to bottom
            func_print(AP_Tree->params[i], NULL); 
            // define variables
            printf("WRITE GF@%%freturn\n");
        }

        // if arithmetic expression
        if (AP_Tree->params[i]->token.type != IDENTIFIER) {
            if (AP_Tree->params[i]->left == NULL && AP_Tree->params[i]->right == NULL) {
                print_stack(AP_Tree->params[i], false);
                printf("WRITE %s\n", stack_pop(&right_stack));
            } else {
                arithmetic_print(AP_Tree->params[i], "GF@\%freturn", true);
                printf("WRITE GF@%%freturn\n");
            }
        }
    }
}

/**
 * @brief Function strlen - returns length of given string
 * 
 * @param AP_Tree - expression tree
 * @param token_content - token content for defined functions
 */
void func_strlen(expr_node_t* AP_Tree, char *token_content) {
    print_stack(AP_Tree->params[0], false);
    printf("STRLEN LF@%s %s\n", token_content, stack_pop(&right_stack));
}

/**
 * @brief Function ord - int of given character
 * 
 * @param AP_Tree - expression tree
 * @param token_content - token content for defined functions
 */
void func_ord(expr_node_t* AP_Tree, char *token_content){
    if(strlen(AP_Tree->params[0]->token.content) == 0){
        printf("MOVE LF@%s int@0\n", token_content);
    } else {
        print_stack(AP_Tree->params[0], false);
        printf("STRI2INT LF@%s %s int@0\n", token_content, stack_pop(&right_stack));
    }
}

/**
 * @brief Function chr - returns character of given ASCII code
 * 
 * @param AP_Tree - expression tree
 * @param token_content - token content for defined functions
 */
void func_chr(expr_node_t* AP_Tree, char *token_content) {
    print_stack(AP_Tree->params[0], false);
    printf("INT2CHAR LF@%s %s\n", token_content, stack_pop(&right_stack));
}


/**
 * @brief Function strval - checks if given string is nil
 * 
 * @param AP_Tree - expression tree
 * @param token_content - token content for defined functions
 */
void func_strval(expr_node_t* AP_Tree, char *token_content){
    if(strlen(AP_Tree->params[0]->token.content) == 0){
        printf("MOVE LF@%s nil@nil\n", token_content);
    } else {
        print_stack(AP_Tree->params[0], false);
        printf("MOVE LF@%s %s\n", token_content, stack_pop(&right_stack));
    }
}

/**
 * @brief Function substr - returns substring of given string
 * - defines in header of ifjcode
 */
void func_substring(void) 
{
    printf("# - FUNCTION SUBSTRING - #\n");
    printf("JUMP %%substring\n");
    printf("LABEL funcsubstring\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@var1\n");
    printf("DEFVAR LF@var2\n");
    printf("DEFVAR LF@var3\n");
    printf("MOVE LF@var1 LF@%%fvar0\n");
    printf("MOVE LF@var2 LF@%%fvar1\n");
    printf("MOVE LF@var3 LF@%%fvar2\n");
    printf("LT GF@%%tmp0 LF@var2 int@0\n");
    printf("JUMPIFEQ %%substrerr GF@%%tmp0 bool@true\n");
    printf("LT GF@%%tmp0 LF@var3 int@0\n");
    printf("JUMPIFEQ %%substrerr GF@%%tmp0 bool@true\n");
    printf("GT GF@%%tmp0 LF@var2 LF@var3\n");
    printf("JUMPIFEQ %%substrerr GF@%%tmp0 bool@true\n");
    printf("STRLEN GF@%%tmp2 LF@var1\n");
    printf("JUMPIFEQ %%substrerr LF@var2 GF@%%tmp2 #i equals lenght of string\n");
    printf("LT GF@%%tmp0 GF@%%tmp2 LF@var2\n");
    printf("JUMPIFEQ %%substrerr GF@%%tmp0 bool@true\n");
    printf("LT GF@%%tmp0 GF@%%tmp2 LF@var3\n");
    printf("JUMPIFEQ %%substrerr GF@%%tmp0 bool@true\n");
    printf("MOVE GF@%%tmp0 int@0\n");
    printf("MOVE GF@%%tmp1 string@\n");
    printf("JUMP %%substring1\n");
    printf("LABEL %%substring1\n");
    printf("JUMPIFEQ %%strkeep GF@%%tmp0 LF@var2\n");
    printf("ADD GF@%%tmp0 GF@%%tmp0 int@1\n");
    printf("JUMP %%substring1\n");
    printf("LABEL %%strkeep\n");
    printf("JUMPIFEQ %%substrend GF@%%tmp0 LF@var3\n");
    printf("GETCHAR GF@%%tmp2 LF@var1 GF@%%tmp0\n");
    printf("CONCAT GF@%%tmp1 GF@%%tmp1 GF@%%tmp2\n");
    printf("ADD GF@%%tmp0 GF@%%tmp0 int@1\n");
    printf("JUMP %%strkeep\n");
    printf("LABEL %%substrend\n");
    printf("MOVE GF@%%freturn GF@%%tmp1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL %%substrerr\n");
    printf("MOVE GF@%%freturn nil@nil\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL %%substring\n");
    printf("# ======================== #\n");
}

/**
 * @brief Function intval - returns integer value of given variable
 * - defines in header of ifjcode
 */
void func_intval(void) {
    printf("# - FUNCTION intval - #\n");
    printf("JUMP %%intval\n");
    printf("LABEL funcintval\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@var1\n");
    printf("MOVE GF@%%tmp2 bool@false\n");
    printf("MOVE LF@var1 LF@%%fvar0\n");
    printf("TYPE GF@%%tmp0 LF@var1\n");
    printf("EQ GF@%%tmp1 GF@%%tmp0 string@int\n");
    printf("JUMPIFEQ %%intvalI bool@true GF@%%tmp1\n");
    printf("EQ GF@%%tmp1 GF@%%tmp0 string@float\n");
    printf("JUMPIFEQ %%intvalF bool@true GF@%%tmp1\n");
    printf("MOVE GF@%%freturn int@0\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("\n");
    printf("LABEL %%intvalI #input is int\n");
    printf("MOVE GF@%%freturn LF@var1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("\n");
    printf("LABEL %%intvalF #input is float\n");
    printf("FLOAT2INT GF@%%freturn LF@var1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL %%intval\n");
    printf("# =============================== #\n");
}

/**
 * @brief Function floatval - returns float value of given variable
 * - defines in header of ifjcode
 */
void func_floatval(void){
    printf("# - FUNCTION floatval - #\n");
    printf("JUMP %%float\n");
    printf("LABEL funcfloatval\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@var1\n");
    printf("MOVE GF@%%tmp2 bool@false\n");
    printf("MOVE LF@var1 LF@%%fvar0\n");
    printf("TYPE GF@%%tmp0 LF@var1\n");
    printf("EQ GF@%%tmp1 GF@%%tmp0 string@int\n");
    printf("JUMPIFEQ %%floatvalI bool@true GF@%%tmp1\n");
    printf("EQ GF@%%tmp1 GF@%%tmp0 string@float\n");
    printf("JUMPIFEQ %%floatvalF bool@true GF@%%tmp1\n");
    printf("MOVE GF@%%freturn float@0x0p+0 #else input is null\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("\n");
    printf("LABEL %%floatvalI #input is int\n");
    printf("INT2FLOAT GF@%%freturn LF@var1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("\n");
    printf("LABEL %%floatvalF #input is float\n");
    printf("MOVE GF@%%freturn LF@var1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL %%float\n");
    printf("# =============================== #\n");
}
