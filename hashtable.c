#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "hashtable.h"


void ht_resize(ht_table_t *table);


// Hash function
int get_hash(char *key, int table_size) {
  int result = 1;
  int length = strlen(key);
  for (int i = 0; i < length; i++) {
    result += key[i];
  }
  return (result % table_size);
}

// Initialize table
void ht_init(ht_table_t *table, int table_size) {
  table->items = (ht_item_t **)malloc(table_size * sizeof(ht_item_t *));
  if(table->items == NULL){
    fprintf(stderr, "Error: Allocation failed\n");
    exit(99);
  }

  table->size = table_size;
  table->item_count = 0;

  for (int i = 0; i < table_size; i++){
    table->items[i] = NULL;
  }
}

// Search for item in table
ht_item_t *ht_search(ht_table_t *table, char *name) {
  int hash = get_hash(name, table->size);
  ht_item_t *item = table->items[hash];

  while (item != NULL){
    if (strcmp(item->name, name) == 0){
      return item;
    }
    item = item->next;
  }
  return NULL;
}

// Insert new item into table
void ht_insert(ht_table_t *table, char *name, symtable_type_t type, symtable_var_type_t var_type, bool used, bool modified, int input_parameters, symtable_type_t *params, symtable_type_t return_type) {
    // Resize table if needed
    if (table->item_count >= table->size){
        ht_resize(table);
    }

  // Existing item
  ht_item_t *existing_item = ht_search(table, name);
  if (existing_item != NULL){
    fprintf(stderr, "Redefinition of variable\n");
    exit(5);
  }

  // New item
  ht_item_t *new_item = (ht_item_t *)malloc(sizeof(struct ht_item));
  if (new_item == NULL){
    fprintf(stderr, "Error: Allocation failed\n");
    exit(99);
  }
  new_item->name = name;
  new_item->type = type;
  new_item->var_type = var_type;
  new_item->input_parameters = input_parameters;
  new_item->used = used;
  new_item->modified = modified;
  new_item->return_type = return_type;
  new_item->params = params;
  int hash = get_hash(name, table->size);
  
  // Add new value into the list
  if (table->items[hash] != NULL){
    new_item->next = table->items[hash];
    table->items[hash] = new_item;
    return;
  }


  // Add new value at the beginning
  new_item->next = NULL;
  table->items[hash] = new_item;
  table->item_count++;
}


// Delete item from table
void ht_delete(ht_table_t *table, char *name) {
  int hash = get_hash(name, table->size);
  ht_item_t *item = table->items[hash];
  if (item == NULL){
    return;
  }

  if (item->name == name){
    if (item->next != NULL){
      table->items[hash] = item->next;
      free(item);
      item = NULL;
      return;
    }
    table->items[hash] = NULL;
    free(item);
    item = NULL;
    return;
  }

  while (item->next != NULL){
    if (item->next->name == name){
      if (item->next->next != NULL){
        ht_item_t *temp = item->next;
        item->next = temp->next;
        free(temp);
        temp = NULL;
        return;
      }
      free(item->next);
      item->next = NULL;
      return;
    }
    item = item->next;
  }
}


// Delete all items from table
void ht_delete_all(ht_table_t *table) {
  for (int i = 0; i < table->size; i++){
    while (table->items[i] != NULL){
      ht_item_t *item = table->items[i];
      if((item->type != sym_func_type) && (item->used == false)){
          fprintf(stderr, "Semantic error 9: Unused variable\n");
          exit(9);
      }
      // Added modified check
      if((item->var_type == sym_var) && (item->modified == false)){
          fprintf(stderr, "Semantic error 9: Variable declared but not modified\n");
          exit(9);
      }
      ht_item_t *temp = item;
      item = item->next;
      table->items[i] = item;
      free(temp->params);
      free(temp);
      temp = NULL;
      continue;
      }
      table->items[i] = NULL;
    }
    table = NULL;
}


// Check if number is prime
int is_prime(int number){
    if (number <= 1){
        return 0;
    }
    if (number == 2){
        return 1;
    }
    if (number % 2 == 0){
        return 0;
    }

    for (int i = 3; i * i <= number; i += 2){
        if (number % i == 0){
            return 0;
        }
    }
    return 1;
}


// Find next prime number
int next_prime(int number){
    while(is_prime(number) == 0){
        number++;
    }
    return number;
}


// Resize table
void ht_resize(ht_table_t *table){
    // Create new table
    int new_size = next_prime(table->size * 2);
    ht_table_t new_table;
    new_table.size = new_size;
    new_table.item_count = 0;
    new_table.items = (ht_item_t **)malloc(new_size * sizeof(ht_item_t *));

    if (new_table.items == NULL){
        fprintf(stderr, "Error: Allocation failed\n");
        exit(99);
    }

    for(int i = 0; i < new_size; i++){
        new_table.items[i] = NULL;
    }

    // Copy items from old table to new table
    for(int i = 0; i < table->size; i++){
        ht_item_t *item = table->items[i];
        while(item != NULL){
            ht_insert(&new_table, item->name, item->type, item->var_type, item->used, item->modified, item->input_parameters, item->params, item->return_type);
            item = item->next;
        }
    }

    // Delete old table
    ht_delete_all(table);
    free(table->items);

    // Set new table
    table->items = new_table.items;
    table->size = new_size;
    table->item_count = new_table.item_count;
}


// Copy table item
ht_item_t *ht_copy_item(ht_item_t *item){
  if(item == NULL){
    return NULL;
  }

  ht_item_t *new_item = (ht_item_t *)malloc(sizeof(struct ht_item));
  if (new_item == NULL){
    fprintf(stderr, "Error: Allocation failed\n");
    exit(99);
  }

  new_item->name = strdup(item->name);
  if(new_item->name == NULL){
    free(new_item);
    fprintf(stderr, "Error: Allocation failed\n");
    exit(99);
  }

  new_item->type = item->type;
  new_item->var_type = item->var_type;
  new_item->used = item->used;
  new_item->input_parameters = item->input_parameters;
  new_item->modified = item->modified;
  new_item->params = item->params;
  new_item->return_type = item->return_type;
  

  new_item->next = ht_copy_item(item->next);

  return new_item;
}


// Copy table
void ht_copy(ht_table_t *old_table, ht_table_t *new_table){
  new_table->size = old_table->size;
  new_table->item_count = old_table->item_count;
  new_table->items = (ht_item_t **)malloc(new_table->size * sizeof(ht_item_t *));
  if(new_table->items == NULL){
    fprintf(stderr, "Error: Allocation failed\n");
    exit(99);
  }

  for(int i = 0; i < new_table->size; i++){
    if(old_table->items[i] != NULL){
      new_table->items[i] = ht_copy_item(old_table->items[i]);
    }
    else{
      new_table->items[i] = NULL;
    }
  }
}
