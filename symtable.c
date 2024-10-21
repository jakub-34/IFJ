#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "hashtable.h"
#include "symtable_stack.h"


// Enter new scope
void new_scope(sym_stack_t *stack, ht_table_t *table){
    ht_table_t *temp = malloc(sizeof(ht_table_t));
    if(temp == NULL){
        fprintf(stderr, "Error: malloc failed\n");
        exit(99);
    }
    ht_copy(table, temp);
    sym_stack_push(stack, temp);

    ht_table_t new_table;
    ht_init(&new_table, 101);
    *table = new_table;
}


// Leave current scope
void leave_scope(sym_stack_t *stack, ht_table_t *table){
    ht_table_t *temp = sym_stack_top(stack);
    if(temp != NULL){
        ht_delete_all(table);
        *table = *temp;
    }
    sym_stack_pop(stack);
}


// Get item from symtable
ht_item_t *get_item(sym_stack_t *stack, ht_table_t *table, char *name){
    ht_item_t *item = ht_search(table, name);
    if(item == NULL){
        for(int i = stack->top_index; i >= 0; i--){
            item = ht_search(stack->table[i], name);
            if(item != NULL){
                break;
            }
        }
    }
    return item;
}