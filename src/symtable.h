#pragma once
#define _POSIX_C_SOURCE 200809L

/* - - - - - - - - */
/*    LIBRARIES    */
/* - - - - - - - - */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


/* - - - - - - - - - - - -*/
/*    TYPE DEFINITIONS    */
/* - - - - - - - - - - - -*/

/**
 * Maximal size of a string/name/key
 */
#define MAX_STRING_SIZE 1000

/**
 * Enum definition od element's (data slots) in symbol table.
 * These enums are used in function which access or insert data
 * 
 *  - new data-slots may be added
 *  - not used in any functions
 */
typedef enum {
    // type of var
    TYPE = 0,
    // size of the var in bytes
    SIZE = 1,
    // declaration line of var
    DECLA_LINE = 2,
    // line of usage of var
    USAGE_LINE = 3,
    // adress of variable
    ADRESS = 4,
} STDataSlot;

/**
 * Definition of element's data in symbol table, directly accessed by pointer STElementDataPtr.
 * 
 * - existing data-types may be replaced
 * - new data-types may be added
*/
typedef struct STElementData {
    // type of var
    int type;
    // size of the var in bytes
    int size;
    // declaration line of var - can be replaced by file-pointer
    int decla_line;   
    // line of usage of var - can be replaced with dynamic-array/static-array of file-pointers
    int usage_line; 
    // adress of variable - can be replaced with pointer
    int adress; 
} *STElementDataPtr;

/**
 * Definition of element in symbol table, directly accessed by pointer STElementPtr
 * 
 * - the key/name of element may be changed from static to dynamically alocated
 */
typedef struct STElement {
    // the key/name data of an element - can become dynamically alocated in the future
    char name[MAX_STRING_SIZE];
    // pointer to elements's data
    struct STElementData *data;
    // pointer pointing to next element, (cause of possible collision in symtable)
    struct STElement *nextElement;
} *STElementPtr;

/**
 * Definition of symbol_table/list, which is a dynamically alocated 1D array of linked lists.
 * Uses hash function for quicker search and insertion of elements.
 */
typedef struct {
    // dynamically alocated 1D array of STElement pointers
    STElementPtr *array;
    // the size of the array
    size_t list_size;
    // number of elements in the list
    size_t element_size;
} STList;


/* - - - - - - - - - - - - - */
/*   COMPUTATION FUNCTIONS   */
/* - - - - - - - - - - - - - */

/**
 *  Creates symbol table and return pointer to it */
extern unsigned int ST_Hash(char *name, size_t table_size);


/* - - - - - - - - - - - */
/*   ST_LIST FUNCTIONS   */
/* - - - - - - - - - - - */

/**
 *  Creates symbol table and return pointer to it */
extern STList* ST_Init(size_t size); 

/* Changes size of the symbol table */
extern STList* ST_Resize(STList *list, size_t size);

/* Deletes all elements and deletes the symbol table */
extern void ST_Dispose(STList **list);    

/** 
 * Checks if symbol table is not empty */
extern unsigned int ST_ListEmpty(STList *list); 

/* Returns the size of the symbol's table array */
extern size_t ST_ListSizeArr(STList *list);

/* Returns the num of elements in the symbol table */
extern size_t ST_ListSizeEle(STList *list);  


/* - - - - - - - - - - - - */
/*   ST_ELEMENT FUNCTIONS  */
/* - - - - - - - - - - - - */

/* Searches for element with key and returns pointer to it */
extern STElementPtr ST_ElementSearch(STList *list, char *string);

/* Returns true if if element with *string key exists */
extern int ST_ElementExists(STList *list, char *string);

/* Creates in a symbol table element with no data */
extern void ST_Create(STList *list, char *string);

/* Creates symbol table and element with no data,
 * Resizes symbol table if there is more elements than it's size */
extern void ST_CreateResize(STList **list, char *string);

/* Deletes in a symbol table an element */
extern void ST_Delete(STList *list, char *string);   

/* Deletes all elements in the symbol table */
extern void ST_DeleteAll(STList *list);   


/* - - - - - - - - - - - - - - - */
/*   ST_ELEMENT_DATA FUNCTIONS   */
/* - - - - - - - - - - - - - - - */

/* Returns STElementDataPtr pointer to element's ElementData struct */
extern STElementDataPtr ST_DataGet(STList *list, char *string); 

/* Cleans every data slot in an element */
extern void ST_DataClear(STList *list, char *string);     


/* - - - - - - - - - - */
/*   DEBUG FUNCTIONS   */
/* - - - - - - - - - - */

/* (Debug function) - Prints the symbol table into stdout */
extern void ST_PrintList(STList *list);
