# IFJ-Projekt
 * Creating compiler, for php-clone programing language, defined in ifj2022_final.pdf


# Library symtable.h - List of all symtable.h libary functions:

ST_Hash(char *name, size_t table_size);
 * Creates hash from inserted name, (converts string into unique int with consistency).
 * The converted int can have size <(0),(table_size)>. 
 * The created int will be then used in a hash table with size of (hash_table) variable.
 * 
 * @param name String used for hash
 * @param table_size The size of the hash_table
 * @return (unsigned int) - hash created from int

- example: size_t num = ST_Hash("text", 100);


STList* ST_Init(size_t size);  
 * Function creates (STList), dynamically alocated array of (linked lists), of type StElement
 * 
 * @param list Pointer to STlist
 * @param size Number of (linked lists) which will be created.
 * @return Pointer to STList if successful or NULL pointer if not
 * 
 * example: STList list = ST_Init(10);


STList* ST_Resize(STList *list, size_t size);
 * Function changes size of array of (linked lists), of type StElement
 * 
 * @param list Pointer to STlist
 * @param size Number of new (linked lists).
 * @return Pointer to STList if successful or NULL pointer if not
 * 
 * example: list = ST_Resize(list, 20);


void ST_Dispose(STList **list);    
 * Deletes symtable and deallocates it's memory.
 * Uses function [ ST_DeleteAll ].
 * 
 * @param list Pointer to STlist, which will be deallocated
 * 
 * example: ST_Dispose(&list);


unsigned int ST_ListEmpty(STList *list); 
 * Checks if list is empty
 * 
 * @param list Pointer to STlist
 * @return Bool int value (0, 1), if list is empty return 1, if not 0
 * 
 * example: if (ST_ListEmpty(list)) {}


size_t ST_ListSizeArr(STList *list);
 * Function returns symbol_table's list_size
 * 
 * @param list Pointer to STlist
 * 
 * example: size_t num = ST_ListSizeArr(list);


size_t ST_ListSizeEle(STList *list);  
 * Function returns symbol_table's element_size
 * 
 * @param list Pointer to STlist
 * 
 * example: size_t num = ST_ListSizeEle(list);


STElementPtr ST_ElementSearch(STList *list, char *string);
 * Function searches (ST_Element) in the symtable and returns pointer to it
 * 
 * @param list Pointer to STlist
 * @param string Key of the element which is searched
 * @return Pointer to STElement or NULL if unsuccessful
 * 
 * example: STElementPtr element_ptr = ST_ElementSearch(list, "hello");


int ST_ElementExists(STList *list, char *string);
 * Checks if ST_Element element exists in ST_List
 * Uses function, [ ST_ElementSearch ]
 * 
 * @param list Pointer to STlist
 * @param string Key of the element which is searched
 * @return Bool int value (0, 1), if element exists return 1, if not 0
 * 
 * example: if (ElementExists(list, "hello")) {}


void ST_Create(STList *list, char *string);
 * Creates in a symbol table element with no data
 * String as an input is not checked, [ use alphanumerical symbols ], will be fixed
 * 
 * @param list Pointer to STlist
 * @param string Name and key of the new element 
 * 
 * example: ST_Create(list, "world");


void ST_CreateResize(STList **list, char *string);
 * Creates symbol table and element with no data,
 * Resizes symbol table if there is more elements than it's size
 * Uses function [ ST_Create, ST_Resize ] 
 * 
 * @param list Pointer to STlist
 * @param string Name and key of the new element 
 * 
 * example: ST_CreateResize(list, "world")


void ST_Delete(STList *list, char *string);  
 * Deletes an element in symbol 
 * Uses function [ ST_ElementExists() ]
 * 
 * @param list Pointer to STlist
 * @param string Name and key of the element
 * 
 * example: ST_Delete(list, "world");


void ST_DeleteAll(STList *list);   
 * Deletes every element in the symbol table
 * 
 * @param list Pointer to STlist
 * 
 * example: ST_DeleteAll(list);


STElementDataPtr ST_DataGet(STList *list, char *string); 
 * Function returns element's data pointer.
 * uses function [ ST_DataGet ].
 * 
 * @param list Pointer to STlist
 * @param string Name and key of the element 
 * @return STElementDataPtr Pointer to element's data in symtable.
 * 
 * example: STElementDataPtr data = ST_DataGet(list, "world");   data->type = 5;


void ST_DataClear(STList *list, char *string);     
 * Clears data in element, (STElementData)
 * 
 * @param list Pointer to STlist
 * @param string Name and key of the element 
 * 
 * example: ST_DataClear(list, "world"); 

