/* Fill in your Name and GNumber in the following two comment fields
 * Name:
 * GNumber:
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "hash.h"

/* (IMPLEMENT THIS FUNCTION)
 * In this function, you will create a new Symtab struct.
 * Follow the Directions in the Project Documentation for this Function
 * Return the pointer to the new symtab.
 * On any memory errors, return NULL
 */
Symtab *hash_initialize() {
  /* Implement this function */
  Symtab* symtab = malloc(sizeof(Symtab));
  if(symtab == NULL){
    return NULL;
  }
  symtab->table = malloc(sizeof(Symbol) * HASH_TABLE_INITIAL);
  symtab->capacity = HASH_TABLE_INITIAL;
  for (int i = 0; i < HASH_TABLE_INITIAL; i++)
  {
    symtab->table[i] = NULL;
  }
  symtab->size = 0;
  return symtab;
}
    // printf("%d\n", symtab->table[i]);

/* (IMPLEMENT THIS FUNCTION)
 * Destroy your Symbol Table.
 * Follow the Directions in the Project Documentation for this Function
 * Return on any memory errors.
 */
void hash_destroy(Symtab *symtab) {
  if(symtab == NULL){
    return;
  }
  Symbol* current = NULL;
  Symbol* killer = NULL;
  for (int i = 0; i < hash_get_capacity(symtab); i++)
  {
    current = symtab->table[i];
    while(current!=NULL){
      killer = current;
      current = current->next;
      symbol_free(killer);
    }
  }

  free(symtab->table);
  symtab->table = NULL;
  free(symtab);
  symtab = NULL;
  /* Implement this function */
  return;
}

/* (IMPLEMENT THIS FUNCTION)
 * Return the capacity of the table inside of symtab.
 * If symtab is NULL, return -1;
 */
int hash_get_capacity(Symtab *symtab) {
  /* Implement this function */
  if(symtab == NULL){
    return -1;
  }
  return symtab->capacity;
}

/* (IMPLEMENT THIS FUNCTION)
 * Return the number of used indexes in the table (size) inside of symtab.
 * If symtab is NULL, return -1;
 */
int hash_get_size(Symtab *symtab) {
  if(symtab == NULL){
    return -1;
  }
  /* Implement this function */
  return symtab->size;
}

/* (IMPLEMENT THIS FUNCTION)
 * Adds a new Symbol to the symtab via Hashing.
 * Follow the Directions in the Project Documentation for this Function
 * If symtab is NULL, there are any malloc errors, or if any rehash fails, return -1;
 * Otherwise, return 0;
 */
int hash_put(Symtab *symtab, char *var, int val) {
  if(symtab == NULL){
    return -1;
  }
  float load = (float)hash_get_size(symtab) / (float)hash_get_capacity(symtab);
  if(load>=2.0){
    hash_rehash(symtab,hash_get_capacity(symtab) * 2);
  }
  Symbol* newSymbol = symbol_create(var, val);
  if(newSymbol == NULL){
    return -1;
  }
  int index = hash_code(var) % symtab->capacity;
  // Check if a Symbol is present at this index;
  if(symtab->table[index] == NULL){
    // Start a linked list here
    symtab->size +=1;
    symtab->table[index] = newSymbol;
  }
  else{
    // Loop through the entire LL to search if Variable is already present
    Symbol* current = symtab->table[index];
    while(current != NULL){
      if(strncmp(current->variable,var, strlen(var)) == 0){
        // Varable is already present
        current->val = val;
        break;
      }
      if(current->next == NULL){
        current -> next = newSymbol;
        symtab->size +=1;
        break;
      }
      current = current->next;
    }
  }
  // Check to rehash
  
  /* Implement this function */
  return 0;
}

/* (IMPLEMENT THIS FUNCTION)
 * Gets the Symbol for a variable in the Hash Table.
 * Follow the Directions in the Project Documentation for this Function
 * On any NULL symtab or memory errors, return NULL
 */
Symbol *hash_get(Symtab *symtab, char *var) {
  if(symtab == NULL){
    return NULL;
  }
  int index = hash_code(var) % symtab->capacity;
  if(symtab->table[index] == NULL){
    // Not found
    return NULL;
  }
  else{
    // Need to search at this index
    Symbol* current = symtab->table[index];
    while(current != NULL){
      if(strncmp(current->variable,var, strlen(var)) == 0){
        // Varable is FOUND
        Symbol* symbolToReturn = symbol_copy(current);
        if(symbolToReturn == NULL){
          return NULL;
        }
        else{
          return symbolToReturn;
        }
      }
    current = current->next;
    }
  }
  /* Implement this function */
  return NULL;
}

/* (IMPLEMENT THIS FUNCTION)
 * Doubles the size of the Array in symtab and rehashes.
 * Follow the Directions in the Project Documentation for this Function
 * If there were any memory errors, set symtab->array to NULL
 * If symtab is NULL, return immediately.
 */
/*
 1) Create a single linked list of every Symbol
    Delete the entire table
    Insert all the elements in the previous linked list to the original
 2)   
*/
void hash_rehash(Symtab *symtab, int new_capacity) {
  if(symtab == NULL){
    return;
  }
  Symbol* current = NULL;
  Symbol* head = NULL;
  Symbol* walker = NULL;
  Symbol* killer = NULL;
  for (int i = 0; i < new_capacity/2; i++)
  {
    printf("%d\n", i);
    walker = symtab->table[i];
    while(walker!=NULL){
      if(head == NULL){
        current = symbol_copy(walker);
        head = current;
      }
      else{
        current->next = symbol_copy(walker);
        current = current->next;
        killer = walker;
      }
      killer = walker;
      walker = walker->next;
      symbol_free(killer);
    }
  }
  // Linked list at head
  free(symtab->table);
  symtab->table = NULL;
  symtab->table = malloc(sizeof(Symbol) * new_capacity);
  symtab->size = 0;
  symtab->capacity = new_capacity;
  for (int i = 0; i < new_capacity; i++)
  {
    symtab->table[i] =NULL;
  }
  current = head;
  while(current!=NULL){
    hash_put(symtab, current->variable, current->val);
    killer = current;
    current = current->next;
    symbol_free(killer);
  }
  symbol_free(walker);

  // free(originalTable);
  /* Implement this function */
  return;
}

/* Implemented for you.
 * Provided function to print the symbol table */
void hash_print_symtab(Symtab *symtab) {
  if(symtab == NULL) {
    return;
  }
  printf("|-----Symbol Table [%d size/%d cap]\n", symtab->size, symtab->capacity);

  int i = 0;
  Symbol *walker = NULL;

  /* Iterate every index, looking for symbols to print */
  for(i = 0; i < symtab->capacity; i++) {
    walker = symtab->table[i];
    /* For each found linked list, print every symbol therein */
    while(walker != NULL) {
      printf("| %10s: %d \n", walker->variable, walker->val);
      walker = walker->next;
    }
  }
  return;
}

/* This function is written for you.
 * This computes the hash function for a String
 */
long hash_code(char *var) {
  long code = 0;
  int i;
  int size = strlen(var);

  for(i = 0; i < size; i++) {
    code = (code + var[i]);
    if(size == 1 || i < (size - 1)) {
      code *= 128;
    }
  }

  return code;
}
