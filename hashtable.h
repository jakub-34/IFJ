#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>

// For types of variables and other helpful types (changing order might break something, do with caution)
typedef enum symtable_type{
  sym_int_type,
  sym_nullable_int_type,
  sym_float_type,
  sym_nullable_float_type,
  sym_string_type,
  sym_nullable_string_type,
  sym_func_type,
  // until this are types used for type of identifier
  sym_null_type,  // type for null value
  sym_void_type,  // for type of return value
  sym_bool_type,   // for condition result
  sym_str_lit_type  // raw string
}symtable_type_t;

// Type for modifiable, constatnt variables and literals
typedef enum symtable_var_type{
  sym_var,
  sym_const,
  sym_literal
}symtable_var_type_t;

// Item in symtable
typedef struct ht_item {
  char *name;
  symtable_type_t type;
  symtable_var_type_t var_type;
  bool used;
  bool modified;
  int input_parameters;
  symtable_type_t *params;
  symtable_type_t return_type;
  struct ht_item *next;
} ht_item_t;

// Symtable itself
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
void ht_insert(ht_table_t *table, ht_item_t *item);

// Copy table into new one
void ht_copy(ht_table_t *old_table, ht_table_t *new_table);

// Delete item from table
void ht_delete(ht_table_t *table, char *name);

// Delete all items from table
// Also checking unused variable semantic error
void ht_delete_all(ht_table_t *table);

// Prints out all items of symtable of current scope
void ht_print(ht_table_t *table);

#endif