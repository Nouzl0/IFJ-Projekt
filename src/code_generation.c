#include "code_generation.h"




/* - - - - - - - - - - - - */
/*     GLOBAL VARIABLES    */
/* - - - - - - - - - - - - */

// symbol table
STList *symbol_table;

// used in arithmetics
StackPtr left_stack = NULL, right_stack = NULL, float_stack = NULL;

// TEMPORARY - used get info if AP_Tree has float
bool is_float = false;



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
        symbol_table = ST_Init(10);
        left_stack = stack_init();
        right_stack = stack_init();
        float_stack = stack_init();

        // init print
        printf(".IFJcode22\n");
        printf("CREATEFRAME\n");
        printf("PUSHFRAME\n");
    }

    // go through all nodes in tree recursively
    if (AS_Tree->items_len != 0) {
        for (int i = 0; i < AS_Tree->items_len; i++) {
            do_assignexpr(AS_Tree->items[i]);
            do_expr(AS_Tree->items[i]);
            do_retexpr(AS_Tree->items[i]);
            do_ifelse(AS_Tree->items[i]);
            do_whileblock(AS_Tree->items[i]);
            do_funcblock(AS_Tree->items[i]);
        }
    }

    // #3 - generate code end
    if (main_branch == true) {
        // free memory
        ST_Dispose(&symbol_table);
        stack_dispose(&left_stack);
        stack_dispose(&right_stack);

        // end print
        printf("POPFRAME\n");
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
void do_assignexpr(stx_node_t* AS_Tree) {
    

    // #0 - check if tree is not empty
    if ((AS_Tree == NULL) || (AS_Tree->type != ASSIGNEXPR)) {
        return;
    }

    // #1 - variables
    static bool tmp_var_active = false;

    // #2 check if there are floats
    arithmetic_print_floatcheck(AS_Tree->expr, true);

    // #2 - defines variable - TODO: check if this is the right format
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
        arithmetic_print_float_retype(AS_Tree->expr);
    }

    // #3 - defines tmp variables - TODO: check if this is the right format and make it smarter
    if (tmp_var_active == false) {
        for(int i = 0; i < 3; i++) {
            printf("DEFVAR GF@\%%tmp%d\n", i);
        }
        tmp_var_active = true;
    }

    // #4 - printing move instruction
    if ((AS_Tree->expr->left == NULL) && (AS_Tree->expr->right == NULL)) {
        
        // adding to stack
        print_stack(AS_Tree->expr, false);

        // printing the move instruction
        printf("MOVE LF@%s %s\n", AS_Tree->token->content, stack_pop(&right_stack));
    
    // #5 - else solve arithmetic expression
    } else {

        // setup correct format
        char buffer[1000]; 
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
 * @brief 
 * 
 * @param AS_Tree 
 */
void do_expr(stx_node_t* AS_Tree) 
{   
    // check if tree is not empty
    if ((AS_Tree == NULL) || (AS_Tree->type != EXPR)) {
        return;
    }

    // call function
    if (AS_Tree->expr->token.type == IDENTIFIER) {
        func_print(AS_Tree->expr);
    }

}

void do_retexpr(stx_node_t* AS_Tree) 
{   
    // check if tree is not empty
    if ((AS_Tree == NULL) || (AS_Tree->type != RETEXPR)) {
        return;
    }

    // variables
    static bool tmp_var_active = false; 

    // defines tmp variables - TODO: check if this is the right format and make it smarter
    if (tmp_var_active == false) {
        printf("DEFVAR LF@#freturn\n");
        tmp_var_active = true;
    }

    // print rest of the tree
    arithmetic_print(AS_Tree->expr, "#freturn", true);

    // print return
    printf("RETURN\n");
}


/**
 * @brief 
 * 
 */
void do_ifelse(stx_node_t* AS_Tree) 
{   
    // #0 - check if tree is not empty
    if ((AS_Tree == NULL) || (AS_Tree->type != IFELSE)) {
        return;
    }

    // variables
    static int if_label_cnt = 0, else_label_cnt = 0;  
    int if_label = if_label_cnt, else_label = else_label_cnt;
    if_label_cnt++; else_label_cnt++;


    // if with else condition
    if (AS_Tree->items_len == 2) {
        // if condition
        logic_print(AS_Tree->expr, "\%else", else_label);
        generate_code(AS_Tree->items[0]);            
        printf("JUMP %%if%d\n", if_label);
        
        // else condition
        printf("LABEL %%else%d\n", else_label);
        generate_code(AS_Tree->items[1]);
        printf("LABEL %%if%d\n", if_label);                
    }

    // if without else condition
    if (AS_Tree->items_len == 1) {
        // if condition
        logic_print(AS_Tree->expr, "\%else", else_label);
        generate_code(AS_Tree->items[0]);                  
        printf("LABEL %%else%d\n", else_label);
    }
}


/**
 * @brief 
 * 
 */
void do_whileblock(stx_node_t* AS_Tree)
{
    // #0 - check if tree is not empty
    if ((AS_Tree == NULL) || (AS_Tree->type != WHILEBLOCK)) {
        return;
    }

    // variables
    static int label_counter = 0;
    int func_label = label_counter;
    label_counter++;


    // #1 - Create label
    printf("LABEL %s%d\n", "#while", func_label);

    // #2 - print JUMPIF
    logic_print(AS_Tree->expr, "#endwhile", func_label);
    generate_code(AS_Tree);                               

    // #3 - end label
    printf("JUMP %s%d\n", "#while", func_label);
    printf("LABEL %s%d\n", "#endwhile", func_label);
}

void do_funcblock(stx_node_t* AS_Tree)
{
    // #0 - check if tree is not empty
    if ((AS_Tree == NULL) || (AS_Tree->type != FUNCBLOCK)) {
        return;
    }  

    // #1 - Print label
    printf("LABEL %s%s\n", "#func", AS_Tree->items[0]->token->content);

    // #2 - assign variables
    for (int i = 0; i < AS_Tree->items[1]->items_len; i++) {
        printf("CREATEFRAME\n");
        printf("DEFVAR LF@%s\n", AS_Tree->items[1]->items[i]->items[0]->token->content);
        printf("MOVE %s #fvar%d\n",AS_Tree->items[1]->items[i]->items[0]->token->content, i);
    }
    
    // #2 - now do block code
    generate_code(AS_Tree->items[2]);
}


/* - - - - - - - - - - - - - */
/*  AP_TREE PRINT FUNCTIONS  */
/* - - - - - - - - - - - - - */

/**
 * Recursively prints arithmetic expression from Precedence Tree
 *  - used in function (do_assignexpr, func_call)
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
                func_print(AP_Tree->left);
                
                // setting up functions
                var_offset = tmp_var_offset;
                var_left = tmp_variables[0 + var_offset];
                stack_push_ptr(&right_stack, tmp_right_term);

                // printing
                printf("MOVE %s \%freturn\n", var_left);
                printf("%s %s %s %s\n", instruction_set[AP_Tree->token.type - 13], assigned_var, var_left, stack_pop(&right_stack));
            }
            
            // right terminal is the function
            if ((AP_Tree->right->token.type == IDENTIFIER) && (AP_Tree->left->token.type != IDENTIFIER)) {
                func_print(AP_Tree->right);

                // setting up variables for printing
                var_offset = tmp_var_offset;
                var_right = tmp_variables[0 + var_offset];
                stack_push_ptr(&left_stack, tmp_left_term);

                // printing
                printf("MOVE LF%s LF\%freturn\n", var_right);
                printf("%s %s %s %s\n", instruction_set[AP_Tree->token.type - 13], assigned_var, stack_pop(&left_stack), var_right);
            }

            // both terminals are functions
            if ((AP_Tree->right->token.type == IDENTIFIER) && (AP_Tree->left->token.type == IDENTIFIER)) {
                func_print(AP_Tree->left);

                // left side -> setting up variables and printing 
                var_offset = tmp_var_offset;
                var_left = tmp_variables[0 + var_offset];
                printf("MOVE LF%s LF\%freturn\n", var_left);

                // right side -> setting up variables and printing 
                func_print(AP_Tree->right);
                var_offset = tmp_var_offset;
                var_right = tmp_variables[1 + var_offset];
                printf("MOVE LF%s LF\%freturn\n", var_right);

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


// extension of the arithmetic_print
void print_stack(expr_node_t* AP_Tree, bool left_side)
{   
    
    char buffer[1000], float_buffer[1000];


    // TEMPORATY - quick fix will probably be deleted
    if ((is_float == true) && (AP_Tree->token.type == NUMBER || AP_Tree->token.type == FRACTION)) {
        if (left_side == true) {
            strcpy(buffer, "float@");
            sprintf(float_buffer, "%a", atof(AP_Tree->token.content));
            strcat(buffer, float_buffer);
            stack_push_string(&left_stack, buffer);
        } else {
            strcpy(buffer, "float@");
            sprintf(float_buffer, "%a", atof(AP_Tree->token.content));
            strcat(buffer, float_buffer);
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
                sprintf(float_buffer, "%a", atof(AP_Tree->token.content));
                strcat(buffer, float_buffer);
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
                strcat(buffer, AP_Tree->token.content);
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
                sprintf(float_buffer, "%a", atof(AP_Tree->token.content));
                strcat(buffer, float_buffer);
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
                strcat(buffer, AP_Tree->token.content);
                stack_push_string(&right_stack, buffer);
                break;
            
            default:
                break;
        }
    }
}

// float checker
void arithmetic_print_floatcheck(expr_node_t* AP_Tree, bool reset)
{   
    // reseting the function call
    if (reset == true) {
        is_float = false;
    }

    // recursive call left
    if (AP_Tree->left != NULL) {
        arithmetic_print_floatcheck(AP_Tree->left, false);
    }

    // recursive call right
    if (AP_Tree->right != NULL) {
        arithmetic_print_floatcheck(AP_Tree->right, false);
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

void arithmetic_print_float_retype(expr_node_t* AP_Tree)
{
    // recursive call left
    if (AP_Tree->left != NULL) {
        arithmetic_print_float_retype(AP_Tree->left);
    }

    // recursive call right
    if (AP_Tree->right != NULL) {
        arithmetic_print_float_retype(AP_Tree->right);
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


void arithmetic_print_float_untype(void)
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

void aritmetic_print_floatclean(void)
{
    while(float_stack->size > 0) {
        stack_pop(&float_stack);
    }
}




void logic_print(expr_node_t* AP_Tree, char* label, int label_num) 
{
    // #0 - variables
    //char *left_term = NULL, *right_term = NULL;
    static bool function_start = true;
    bool left_expr_float = false, right_expr_float = false;

    // #1 - function check and reset
    if (AP_Tree == NULL) {
        return;
    }
    // printing tmp helper variables
    if (function_start == true) {
        printf("DEFVAR GF@%s\n", "\%bool0");
        printf("DEFVAR GF@%s\n", "\%bool1");
        printf("DEFVAR GF@%s\n", "\%bvar0");
        printf("DEFVAR GF@%s\n", "\%bvar1");
        function_start = false;
    }


    // #2 - getting sub-trees values
    if ((AP_Tree->left != NULL) && AP_Tree->right != NULL) {
        

        // checking left float
        arithmetic_print_floatcheck(AP_Tree->left, true);
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
                arithmetic_print_float_retype(AP_Tree->left);
            }

            // arithemetic expression
            arithmetic_print(AP_Tree->left, "GF@\%bvar0", true);
        }


        // checking right float
        arithmetic_print_floatcheck(AP_Tree->right, true);
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
                arithmetic_print_float_retype(AP_Tree->right);
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
    case EQUAL:
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
    arithmetic_print_float_untype();
    }
    printf("JUMPIFNEQ %s%d GF@%%bool0 bool@true\n", label, label_num);
}

void func_print(expr_node_t* AP_Tree) {
    
    // variables
    static int func_def_var = 0;
    const char def_func[20][10] = { "write", };
    const int def_func_num = 1;
    int func_num = 0;

    // #1 checking if the function is language-defined 
    do {
        if ((strcmp(def_func[func_num], AP_Tree->token.content)) == 0 ) {
            break;
        }
        func_num++;
    } while(func_num < def_func_num);


    // going back to function or printing
    switch (func_num) {
        
        // print
        case 0: func_test_print(AP_Tree);
            break;

        // user defined
        default:
            // check if there is a need to create function variable
            if (func_def_var < AP_Tree->params_len) {
                while (func_def_var < AP_Tree->params_len) {
                    printf("DEFVAR LF@#fvar%d\n", func_def_var);
                    func_def_var++;
                }
            }

            // init function variables
            for (int i = 0; i < AP_Tree->params_len; i++) {
            
                // printing move instruction
                if ((AP_Tree->params[i]->left == NULL) && (AP_Tree->params[i]->right == NULL)) {
                    printf("MOVE #fvar%d %s\n", i, AP_Tree->params[i]->token.content);

                // else solve arithmetic expression
                } else {
                    // appending the number to the variable name
                    char fvar[16];
                    sprintf(fvar, "fvar%d", i);

                    // recurive call
                    arithmetic_print(AP_Tree->params[i], fvar, false);
                }
            }

            // print function call
            printf("CALL #func%s\n", AP_Tree->token.content);
            break;
        }
}

/* - - - - - - - - - - - */
/*  FUNC CALL FUNCTIONS  */
/* - - - - - - - - - - - */

// simple print used for debugging
void func_test_print(expr_node_t* AP_Tree) 
{
    printf("WRITE LF@%s\n", AP_Tree->params[0]->token.content);
}