#include "symtable.h"

/* - - - - - - - - - - - - - */
/*   COMPUTATION FUNCTIONS   */
/* - - - - - - - - - - - - - */

/**
 * Creates hash from inserted name, (converts string into unique int with consistency).
 * The converted int can have size <(0),(table_size)>. 
 * The created int will be then used in a hash table with size of (hash_table) variable.
 * 
 * @param name String used for hash
 * @param table_size The size of the hash_table
 * @return (unsigned int) - hash created from int
 */
extern unsigned int ST_Hash(char *name, size_t table_size) {
    
    register int length = strnlen(name, BC_STRING_MAX);
    register unsigned int hash_value = 0;

    for (register int i = 0; i < length; i++) {
        hash_value += name[i];
        hash_value = (hash_value * name[i]) % table_size;
    }

    return hash_value;
}


/* - - - - - - - - - - - */
/*   ST_LIST FUNCTIONS   */
/* - - - - - - - - - - - */

/**
 * Function creates (STList), dynamically alocated array of (linked lists), of type StElement
 * 
 * @param list Pointer to STlist
 * @param size Number of (linked lists) which will be created.
 * @return Pointer to STList if successful or NULL pointer if not
 */
extern STList* ST_Init(size_t size) 
{      
    // checking the size of st_init
    if (size == 0) {
        fprintf(stderr, "ERROR - ST_Init, can't initilize symtable with size 0\n");  
        return NULL;
    }

    // creating list struct
    STList* list = calloc(1, sizeof(STList));

    if (list == NULL) {
        fprintf(stderr, "ERROR - ST_Init, malloc failed (list)\n");  
        return NULL;
    }

    // creating the hash array
    list->array = calloc(size, sizeof(STElementPtr));

    if (list->array == NULL) {
        fprintf(stderr, "ERROR - ST_Init, malloc failed (list->array)\n");
        return NULL;                                                 
    }

    // writing the size of the array
    list->list_size = size;
    list->element_size = 0;

    return list;
}

/**
 * Function changes size of array of (linked lists), of type StElement
 * 
 * @param list Pointer to STlist
 * @param size Number of new (linked lists).
 * @return Pointer to STList if successful or NULL pointer if not
 */
extern STList* ST_Resize(STList *list, size_t size)
{   
    // if list is size = 0
    if (size == 0) {
        fprintf(stderr, "ERROR - ST_Resize, can't resize symbtable to 0\n");
        return NULL;
    }

    // if list is empty
    if (list == NULL) {
        fprintf(stderr, "ERROR - ST_Resize, list is empty\n");
        return NULL;
    }
    
    // createing new list
    STList *new_list = calloc(1, sizeof(STList));

    if (new_list == NULL) {
        fprintf(stderr, "ERROR - ST_Resize, malloc (new_list)\n"); 
        return NULL;                                                 
    }

    // creating the array
    new_list->array = calloc(size, sizeof(STElementPtr));
    
    if (new_list->array == NULL) {
        fprintf(stderr, "ERROR - ST_Init, malloc failed (new_list->array)\n"); 
        return NULL;                                                 
    }    

    // remebering the size
    new_list->list_size = size;
    new_list->element_size = list->element_size;

    // tmp STElement pointer
    STElementPtr element_p, element_f;
    int hash_value;

    // going through every array pointer of STlist
    for (int i = 0; i < (list->list_size); i++) {
        element_p = list->array[i];

        // re-pointing the old elements from old to new list
    	while (element_p != NULL) {

            hash_value = ST_Hash(element_p->name, new_list->list_size);
            
            // setting the data in STList
            element_f = new_list->array[hash_value];
            new_list->array[hash_value] = element_p;
            element_p = element_p->nextElement;
            new_list->array[hash_value]->nextElement = element_f;
    	}
    }

    // disposing the old list
    free(list->array);
    free(list);
    
    // return the new list
    return new_list;
}

/**
 * Deletes symtable and deallocates it's memory.
 * - uses function [ ST_DeleteAll ].
 * 
 * @param list Pointer to STlist, which will be deallocated
 */
void ST_Dispose(STList **list)
{
    // checking if list is empty
    if ((*list) == NULL) {
        fprintf(stderr ,"ERROR - ST_Dispose, symtable is already empty\n");
        return;
    }

    // deleting symtable
    ST_DeleteAll(*list);
    free((*list)->array);
    (*list)->array = NULL;

    // setting list pointer to NULL
    free(*list);
    *list = NULL;
}

/**
 * Checks if list is empty
 * 
 * @param list Pointer to STlist
 * @return Bool int value (0, 1), if list is empty return 1, if not 0
 */
extern unsigned int ST_ListEmpty(STList *list)
{
    if (list == NULL) {
        return 0; 
    } else {
        return 1;
    }
}

/**
 * Function returns symbol_table's list_size
 * 
 * @param list Pointer to STlist
 */
extern size_t ST_ListSizeArr(STList *list)
{
    if (list == NULL) {
        fprintf(stderr ,"ERROR - ListArrSize, symtable is already empty\n");
        return 0;
    }

    return list->list_size;
}

/**
 * Function returns symbol_table's element_size
 * 
 * @param list Pointer to STlist
 */
extern size_t ST_ListSizeEle(STList *list)
{
    if (list == NULL) {
        fprintf(stderr ,"ERROR - ListEleSize, symtable is already empty\n");
        return 0;
    }

    return list->element_size;
}


/* - - - - - - - - - - - - */
/*   ST_ELEMENT FUNCTIONS  */
/* - - - - - - - - - - - - */

/**
 * Function searches (ST_Element) in the symtable and returns pointer to it
 * 
 * @param list Pointer to STlist
 * @param string Key of the element which is searched
 * @return Pointer to STElement or NULL if unsuccessful
 */
extern STElementPtr ST_ElementSearch(STList *list, char *string) 
{
    // if list is empty
    if (list == NULL) {
        fprintf(stderr, "ERROR - ST_Search, list is empty\n");
        return NULL;
    }

    int hash_value = ST_Hash(string, list->list_size);
    STElementPtr element_p = list->array[hash_value];

    while (element_p != NULL) {
        if ((strcmp(element_p->name, string)) == 0) {
            return element_p;
        }

        element_p = element_p->nextElement;
    }
    
    fprintf(stderr, "ERROR - ST_Search, %s doesnt't exist\n", string);
    return element_p;
}

/**
 * Checks if ST_Element element exists in ST_List
 * - uses function, [ ST_ElementSearch ]
 * 
 * @param list Pointer to STlist
 * @param string Key of the element which is searched
 * @return Bool int value (0, 1), if element exists return 1, if not 0
 */
extern int ST_ElementExists(STList *list, char *string)
{
    STElementPtr element_p = ST_ElementSearch(list, string);

    if (element_p == NULL) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * Creates in a symbol table element with no data
 * - string as an input is not checked, [ use alphanumerical symbols ], will be fixed
 * 
 * @param list Pointer to STlist
 * @param string Name and key of the new element 
 */
extern void ST_Create(STList *list, char *string)
{   
    // checking if list is not empty
    if (list == NULL) {
        fprintf(stderr, "ERROR - ST_Create, list is empty\n");
        return;
    }
    
    // creating new_element
    STElementPtr new_element = calloc(1 ,sizeof(struct STElement));

	if (new_element == NULL) {
        fprintf(stderr, "ERROR - ST_Create, malloc failed (new_element)\n");
        return;
    }

    // allocating space for element's data
    new_element->data = calloc(1 ,sizeof(struct STElementData));

	if (new_element->data == NULL) {
        fprintf(stderr, "ERROR - ST_Create, malloc failed (new_element->data)\n");
        return;
    }

    // hash value
    int hash_value = ST_Hash(string, list->list_size);

	// pre-setting the new element with relevant data
    strcpy(new_element->name, string);
    new_element->nextElement = NULL;

    // setting the data in STList
	new_element->nextElement = list->array[hash_value];
	list->array[hash_value] = new_element; 
    list->element_size++;
}


/**
 * Creates symbol table and element with no data,
 * Resizes symbol table if there is more elements than it's size
 * - uses function [ ST_Create, ST_Resize ] 
 * 
 * @param list Pointer to STlist
 * @param string Name and key of the new element 
 */
extern void ST_CreateResize(STList **list, char *string)
{   
    // checking if list is not empty
    if (*list == NULL) {
        fprintf(stderr, "ERROR - ST_CreateResize, list is empty\n");
        return;
    }

    // creating new element
    ST_Create(*list, string);

    // resizing list if it's too small
    if ((*list)->list_size <= (*list)->element_size) {
        *list = ST_Resize(*list, ((*list)->list_size) * 2);
    }
}

/**
 * Deletes an element in symbol 
 * - uses function [ST_ElementExists()]
 * 
 * @param list Pointer to STlist
 * @param string Name and key of the element 
 */
extern void ST_Delete(STList *list, char *string )
{
    // linking pointers of element
    STElementPtr element_p, behind_p;

    // checks if element exists
    if (!ST_ElementExists(list, string) ) {
        fprintf(stderr,"ERROR - ST_Delete, %s doesn't exists or list is empty\n", string);
        return;
    }

    // hash of element
    int hash_value = ST_Hash(string, list->list_size);

    // searched element is the first one
    if ( (strcmp(list->array[hash_value]->name, string)) == 0) {
        
        element_p = list->array[hash_value]->nextElement;
        
        // deleting element
        free(list->array[hash_value]->data);
        free(list->array[hash_value]);

        // relinking element
        list->array[hash_value] = element_p;
    
    
    // searched element is in the linked list
    } else {
        element_p = list->array[hash_value];

        while (element_p != NULL) {
            if ( (strcmp(element_p->name, string)) == 0) {
               break;
            } else {
                behind_p = element_p;
                element_p = element_p->nextElement;
            }
        }  

        // relinking elements
        behind_p->nextElement = behind_p->nextElement->nextElement;

        // deleting the targeted element
        free(element_p->data);
        free(element_p);
    }
}

/**
 * Deletes every element in the symbol table
 * 
 * @param list Pointer to STlist
 */
extern void ST_DeleteAll(STList *list) 
{   

    if (list == NULL) {
        fprintf(stderr, "ERROR - ST_DeleteAll, list is empty\n");
        return;
    }

    // tmp STElement pointer
    STElementPtr element_p;

    // going through every array pointer of STlist
    for (int i = 0; i < (list->list_size); i++) {
        
        // deleting the elements in linked list
    	while (list->array[i] != NULL) {
    		element_p = list->array[i]->nextElement;
    		
            // deleting data
            free(list->array[i]->data);
            free(list->array[i]);
    		
            // moving to next element
            list->array[i] = element_p;
    	}

    	// list init
    	list->array[i] = NULL;
    }
}


/* - - - - - - - - - - - - - - - */
/*   ST_ELEMENT_DATA FUNCTIONS   */
/* - - - - - - - - - - - - - - - */

/**
 * Function returns element's data pointer.
 * - uses function [ ST_DataGet ].
 * 
 * @param list Pointer to STlist
 * @param string Name and key of the element 
 * @return STElementDataPtr Pointer to element's data in symtable.
 */
extern STElementDataPtr ST_DataGet(STList *list, char *string)
{
    STElementPtr element_p = ST_ElementSearch(list, string);
    
    if (element_p == NULL) {
        fprintf(stderr,"ERROR - ST_DataGet, %s doesn't exists or list is empty\n", string);
        return NULL;
    }

    return element_p->data;
}

/**
 * Clears data in element, (STElementData)
 * 
 * @param list Pointer to STlist
 * @param string Name and key of the element 
 */
extern void ST_DataClear(STList *list, char *string) 
{
    STElementPtr element_p = ST_ElementSearch(list, string);

    if (element_p == NULL) {
        fprintf(stderr, "ERROR - DataClear, %s doesn't exists or list is empty\n", string);
        return;
    }

    // clearing element's data
    free(element_p->data);
    element_p->data = calloc(1 ,sizeof(struct STElementData));

    if (element_p->data == NULL) {
        fprintf(stderr, "ERROR - DataClear, malloc failed (element_p->data)\n");
        return;
    }

}


/* - - - - - - - - - - */
/*   DEBUG FUNCTIONS   */
/* - - - - - - - - - - */

/**
 * Prints the list (debug)
 * - function will be removed in final build
 * - just be carefull using this thing, but from my experience it's pretty reliable
 */
extern void ST_PrintList(STList *list)
{   

    if (list == NULL) {
        return;
    }


    // tmp STElement pointer
    STElementPtr element_p;
    STElementPtr element_f;

    // going through every array pointer of STlist
    for (int i = 0; i < (list->list_size); i++) {
        
        printf("array[%d] -> ", i);

        element_p = list->array[i];

        // deleting the elements in linked list
    	while (element_p != NULL) {

            // setting pointer on emelement_free and moving pointer next to element_p
            element_f = element_p;
            element_p = element_p->nextElement;

            // printing the elemen
            printf("%s[", element_f->name);
            printf("%d, %d, %d, %d, %d", element_f->data->type, element_f->data->size, 
            element_f->data->decla_line, element_f->data->usage_line, element_f->data->adress);
            printf("] -> ");
    	}

        printf("NULL\n");
    }
    printf("- - - - \n\n");
}