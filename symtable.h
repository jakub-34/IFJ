/*
* Project: Implementacia prekladaca imperativneho jazyka IFJ2024
*
* @author: Jakub Hrdlicka <xhrdli18>
*
*/

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "hashtable.h"
#include "symtable_stack.h"


// Enter new scope
void new_scope(sym_stack_t *stack, ht_table_t *table);

// Leave current scope
void leave_scope(sym_stack_t *stack, ht_table_t *table);

// Get item from symtable
ht_item_t *get_item(sym_stack_t *stack, ht_table_t *table, char *name);


#endif