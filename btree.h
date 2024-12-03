/*
* Project: Implementacia prekladaca imperativneho jazyka IFJ2024
*
* @author: Rebeka Tydorova <xtydor01>
*
*/
#ifndef BTREE_H
#define BTREE_H

#include <stdbool.h>
#include "token.h"


// node
typedef struct bst_node {
  token_t* value;
  struct bst_node *left;
  struct bst_node *right;
} bst_node_t;

//insert node into binary tree
void bst_insert(bst_node_t *root,bst_node_t *left, bst_node_t *right);

//create node
bst_node_t* bts_create_node(token_t* token);

//delete the entire tree
void bst_dispose(bst_node_t *tree);

#endif
