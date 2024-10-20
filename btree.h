/*
 * Hlavičkový soubor pro binární vyhledávací strom.
 * Tento soubor neupravujte.
 */

#ifndef BTREE_H
#define BTREE_H

#include <stdbool.h>
#include "./token.h"


// Uzel stromu
typedef struct bst_node {
  token_t* value;              // hodnota
  struct bst_node *left;  // levý potomek
  struct bst_node *right; // pravý potomek
} bst_node_t;

void bst_insert(bst_node_t *root,bst_node_t *left, bst_node_t *right);
bst_node_t* bts_create_node(token_t* token);
void bst_dispose(bst_node_t *tree);

#endif
