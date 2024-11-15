#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>


typedef enum symtable_type{
  sym_int_type,
  sym_float_type,
  sym_string_type,
  sym_func_type
}symtable_type_t;


typedef struct ht_item {
  char *name;
  symtable_type_t type;
  bool used;
  int input_parameters;
  struct ht_item *next;
} ht_item_t;

typedef struct ht_table {
    ht_item_t **items;
    int size;
    int item_count;
} ht_table_t;

// Initialize table
// Use only prime number for size, recommended size is 101
void ht_init(ht_table_t *table, int table_size);

// Search for item in table
ht_item_t *ht_search(ht_table_t *table, char *name);

// Insert new item into table
// Also checking redefinition sematic error
void ht_insert(ht_table_t *table, char *name, symtable_type_t type, bool used, int input_parameters);

// Copy table into new one
void ht_copy(ht_table_t *old_table, ht_table_t *new_table);

// Delete item from table
void ht_delete(ht_table_t *table, char *name);

// Delete all items from table
// Also checking unused variable semantic error
void ht_delete_all(ht_table_t *table);

#endif