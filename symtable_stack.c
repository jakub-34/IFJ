#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "symtable_stack.h"


// Inicialize stack
void sym_stack_init(sym_stack_t *stack){
    stack->table = (ht_table_t **)malloc(sizeof(ht_table_t *) * 100);
    if(stack->table == NULL){
        fprintf(stderr, "Error: malloc failed\n");
        exit(99);
    }
    stack->top_index = -1;
}


// Check if stack is empty
bool sym_stack_empty(sym_stack_t *stack){
    return stack->top_index == -1;
}


// Check if stack is full
bool sym_stack_full(sym_stack_t *stack){
    return stack->top_index == 99;
}


// Get item from stack
ht_table_t *sym_stack_top(sym_stack_t *stack){
    if(sym_stack_empty(stack)){
        return NULL;
    }
    return stack->table[stack->top_index];
}


// Pop item from stack
void sym_stack_pop(sym_stack_t *stack){
    if(sym_stack_empty(stack)){
        return;
    }
    ht_delete_all(stack->table[stack->top_index]);
    stack->top_index--;
}


// Push item into stack
void sym_stack_push(sym_stack_t *stack, ht_table_t *table){
    if(sym_stack_full(stack)){
        return;
    }
    stack->top_index++;
    stack->table[stack->top_index] = table;
}


// Delete all items from stack and stack
void sym_stack_dispose(sym_stack_t *stack){
    while(!sym_stack_empty(stack)){
        sym_stack_pop(stack);
    }
    free(stack->table);
}