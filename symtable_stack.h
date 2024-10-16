#ifndef SYMTABLE_STACK_H
#define SYMTABLE_STACK_H

#include <stdbool.h>
#include "hashtable.h"

typedef struct sym_stack{
    ht_table_t **table;
    int top_index;
} sym_stack_t;

// Inicialize stack
void sym_stack_init(sym_stack_t *stack);

// Check if stack is empty
bool sym_stack_empty(sym_stack_t *stack);

// Get item from stack
ht_table_t *sym_stack_top(sym_stack_t *stack);

// Pop item from stack
void sym_stack_pop(sym_stack_t *stack);

// Push item into stack
void sym_stack_push(sym_stack_t *stack, ht_table_t *table);

// Delete all items from stack and stack
void sym_stack_dispose(sym_stack_t *stack);

#endif