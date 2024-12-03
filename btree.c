/*
* Project: Implementacia prekladaca imperativneho jazyka IFJ2024
*
* @author: Rebeka Tydorova <xtydor01>
*
*/
#include "btree.h"
#include <stdio.h>
#include <stdlib.h>


//insert node into binary tree
void bst_insert(bst_node_t *root,bst_node_t *left, bst_node_t *right) {
		root->left = left;
		root->right = right;
}

//create node
bst_node_t* bts_create_node(token_t* token){
	bst_node_t* bst_node = (bst_node_t*)calloc(1,sizeof(bst_node_t));
	 if(bst_node == NULL){
        fprintf(stderr, "Error: malloc failed\n");
        exit(99);
    }
	bst_node->value = token;
	return bst_node;

}

//delete the entire tree
void bst_dispose(bst_node_t* tree) {
	if (tree == NULL) {
		return;
	}

	bst_dispose(tree->left);
	bst_dispose(tree->right);

	free(tree);
}

