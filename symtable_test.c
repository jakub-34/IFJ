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

    ht_insert(&table, "ahoj", sym_int_type, true, 0);
    new_scope(&stack, &table);
    ht_insert(&table, "cau", sym_int_type, true, 0);

    ht_item_t *item = get_item(&stack, &table, "ahoj");
    if(item != NULL){
        printf("Ahojfound\n");
    }
    else{
        printf("Ahoj not found\n");
    }

    item = get_item(&stack, &table, "cau");
    if(item != NULL){
        printf("Cau found\n");
    }
    else{
        printf("Cau not found\n");
    }

    leave_scope(&stack, &table);
    
    item = get_item(&stack, &table, "ahoj");
    if(item != NULL){
        printf("Ahojfound\n");
    }
    else{
        printf("Ahoj not found\n");
    }

    item = get_item(&stack, &table, "cau");
    if(item != NULL){
        printf("Cau found\n");
    }
    else{
        printf("Cau not found\n");
    }
}