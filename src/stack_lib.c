/**
 * Project: Implementace překladače imperativního jazyka IFJ22
 * 
 * @file stack_lib.c
 * @brief Code generation helper library 
 * 
 * @author Nikolas Nosál <xnosal01>
 * @author Adam Mrkva <xmrkva04>
 * @author Rostislav Navrátil <xnavra72>
 * @author David Nevrlka <xnevrl00>
 */

#include "stack_lib.h"

/* - - - - - - - - - - - - - */
/*    LIFO STACK FUNCTIONS   */
/* - - - - - - - - - - - - - */


// creates new stack
StackPtr stack_init(void)
{   
    // create stack
    StackPtr stack = malloc(sizeof(struct stack_t));
    stack->top = NULL;
    stack->size = 0;

    // create data
    stack->data = malloc(sizeof(struct stack_data_t));
    stack->data->size = 0;
    stack->data->capacity = START_STACK_DATA;
    stack->data->string_arr = malloc(sizeof(char *) * START_STACK_DATA);


    return stack;
}


// pushes char pointer to the stack
void stack_push_ptr(StackPtr *stack, char* data_ptr) 
{   
    // creating new stack if one doesn't exist's
    if (*stack == NULL) {

        // create stack
        *stack = malloc(sizeof(struct stack_t));
        (*stack)->top = NULL;
        (*stack)->size = 0;

        // create data
        (*stack)->data = malloc(sizeof(struct stack_data_t));
        (*stack)->data->size = 0;
        (*stack)->data->capacity = START_STACK_DATA;
        (*stack)->data->string_arr = malloc(sizeof(char *) * START_STACK_DATA);
    }

    // creating new item
    StackItemPtr new_item = malloc(sizeof(struct stack_item_t));
    new_item->data_ptr = data_ptr;

    // adding new item to the lifo stack
    new_item->next = (*stack)->top;
    (*stack)->top = new_item;
    (*stack)->size++;

    return;
}

int stack_push_string(StackPtr *stack, char* data)
{
    // creating new stack if one doesn't exist's
    if (*stack == NULL) {

        // create stack
        *stack = malloc(sizeof(struct stack_t));
        (*stack)->top = NULL;
        (*stack)->size = 0;

        // create data
        (*stack)->data = malloc(sizeof(struct stack_data_t));
        (*stack)->data->size = 0;
        (*stack)->data->capacity = START_STACK_DATA;
        (*stack)->data->string_arr = malloc(sizeof(char *) * START_STACK_DATA);
    }

    // changing the capacity of the string array
    if ((*stack)->data->size == (*stack)->data->capacity) {
        (*stack)->data->capacity *= 2;
        (*stack)->data->string_arr = realloc((*stack)->data->string_arr, sizeof(char *) * (*stack)->data->capacity);
    }

    // saving the string to array
    int index = (*stack)->data->size;
    (*stack)->data->string_arr[index] = calloc((int) strlen(data) + 1, sizeof(char));
    strcpy((*stack)->data->string_arr[index], data);
    (*stack)->data->size++;

    // creating new item
    StackItemPtr new_item = malloc(sizeof(struct stack_item_t));
    new_item->data_ptr = (*stack)->data->string_arr[index];

    // adding new item to the lifo stack
    new_item->next = (*stack)->top;
    (*stack)->top = new_item;
    (*stack)->size++;

    return index;
}

// stack pop
char* stack_pop(StackPtr *stack)
{
    // checking if stack is empty
    if ((*stack) == NULL || (*stack)->size == 0) {
        return NULL;
    }

    // getting top item
    StackItemPtr top_item = (*stack)->top; 
    char* return_string = (*stack)->top->data_ptr;

    // removing top item
    (*stack)->top = top_item->next;
    (*stack)->size--;

    // freeing memory
    free(top_item);

    return return_string;
}


// stack dispose
void stack_dispose(StackPtr *stack)
{
    // checking if stack is empty
    if ((*stack) == NULL) {
        *stack = NULL;
        return;
    }
    
    // disposing of data
    for (int i = 0; i < (*stack)->data->size; i++) {
        free((*stack)->data->string_arr[i]);
    }

    free((*stack)->data->string_arr);
    free((*stack)->data);


    // getting top item
    StackItemPtr delete_item = (*stack)->top;
    StackItemPtr next_item = NULL;

    // deleting items
    for (int i = 0; i < (*stack)->size; i++) {
        next_item = delete_item->next;
        free(delete_item);

        // setting new delete item
        delete_item = next_item;
    }

    // setting stack to null
    free(*stack);
    *stack = NULL;

    return;
}

// stack print
void stack_print(StackPtr stack)
{
    // checking if stack is empty
    if (stack == NULL || stack->size == 0) {
        return;
    }

    // getting top item
    StackItemPtr top_item = stack->top;

    // printing items
    printf("\n");
    
    for (int i = 0; i < stack->size; i++) {
        printf("[%d] - %s\n", i, top_item->data_ptr);
        top_item = top_item->next;
    }

    return;
}