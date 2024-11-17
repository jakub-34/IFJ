#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "symtable.h"
#include "hashtable.h"
#include "symtable_stack.h"
#include "symtable.h"

int main(){
    ht_table_t table;
    ht_init(&table, 101);
    sym_stack_t stack;
    sym_stack_init(&stack);

    ht_insert(&table, "ahoj", sym_int_type, false, 0);
    new_scope(&stack, &table);

    ht_item_t *item = get_item(&stack, &table, "ahoj");
    if(item != NULL){
        item->used = true;
    }

    leave_scope(&stack, &table);
    item = get_item(&stack, &table, "ahoj");
    if(item != NULL){
        if(item->used == true){
            printf("Item was used\n");
        } else {
            printf("Item was not used\n");
        }
    }

    ht_delete_all(&table);
}