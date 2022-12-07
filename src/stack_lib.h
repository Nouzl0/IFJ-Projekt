/**
 * Project: Implementace překladače imperativního jazyka IFJ22
 * 
 * @file stack_lib.h
 * @brief Header file for code generation helper library 
 * 
 * @author Nikolas Nosál <xnosal01>
 * @author Adam Mrkva <xmrkva04>
 * @author Rostislav Navrátil <xnavra72>
 * @author David Nevrlka <xnevrl00>
 */
#pragma once

// implementation of lifo stack

/* - - - - - - - - */
/*    LIBRARIES    */
/* - - - - - - - - */

// libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* - - - - - - - - - - - -*/
/*    TYPE DEFINITIONS    */
/* - - - - - - - - - - - -*/

#define START_STACK_DATA 10

// stack data
typedef struct stack_data_t {
    char** string_arr;
    int size;
    int capacity;
} *StackDataPtr;

// item of stack
typedef struct stack_item_t{
    struct stack_item_t *next;
    char *data_ptr;
} *StackItemPtr;

// stacks
typedef struct stack_t {
    struct stack_data_t* data;
    struct stack_item_t *top;
    int size;
} *StackPtr;


/* - - - - - - - - */
/*    FUNCTIONS    */
/* - - - - - - - - */

// creates new stack
StackPtr stack_init(void);

// pushes char pointer to the stack
void stack_push_ptr(StackPtr *stack, char* data);

// pushes string to the stack
int stack_push_string(StackPtr *stack, char *data);

// pops item from the stack
char* stack_pop(StackPtr *stack);

// disposes stack
void stack_dispose(StackPtr* stack);

// prints stack
void stack_print(StackPtr stack);