#include <stdlib.h>
#include <stdbool.h>
#include "hashtable.h"
#include "symtable_stack.h"


// Enter new scope
void new_scope(sym_stack_t *stack, ht_table_t *table){
    sym_stack_push(stack, table);
    ht_table_t new_table;
    ht_init(&new_table, 101);
    table = &new_table;
}


// Leave current scope
void leave_scope(sym_stack_t *stack, ht_table_t *table){
    ht_table_t *temp = sym_stack_top(stack);
    if(temp != NULL){
        table = temp;
    }
    sym_stack_pop(stack);
}


// Get item from symtable
ht_item_t *get_item(sym_stack_t *stack, ht_table_t *table, char *name){
    sym_stack_t stack2;
    sym_stack_init(&stack2);
    ht_item_t *item;
    while(true){
        item = ht_search(table, name);

        if((item == NULL) && (sym_stack_empty(stack))){
            while(sym_stack_empty(&stack2) == false){
                sym_stack_push(stack, sym_stack_top(&stack2));
                sym_stack_pop(&stack2);
            }
            sym_stack_dispose(&stack2);
            return NULL;
        }

        if(item != NULL){
            while(sym_stack_empty(&stack2) == false){
                sym_stack_push(stack, sym_stack_top(&stack2));
                sym_stack_pop(&stack2);
            }
            sym_stack_dispose(&stack2);
            return item;
        }

        sym_stack_push(&stack2, table);
        table = sym_stack_top(stack);
        sym_stack_pop(stack);
    }
}