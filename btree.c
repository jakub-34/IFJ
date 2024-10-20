#include "./btree.h"
#include <stdio.h>
#include <stdlib.h>


/*
 * Vložení uzlu do stromu.
 *
 * Pokud uzel se zadaným klíče už ve stromu existuje, nahraďte jeho hodnotu.
 * Jinak vložte nový listový uzel.
 *
 * Výsledný strom musí splňovat podmínku vyhledávacího stromu — levý podstrom
 * uzlu obsahuje jenom menší klíče, pravý větší.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_insert(bst_node_t *root,bst_node_t *left, bst_node_t *right) {
		root->left = left;
		root->right = right;
}

bst_node_t* bts_create_node(token_t* token){
	bst_node_t* bst_node = (bst_node_t*)calloc(1,sizeof(bst_node_t));
	 if(bst_node == NULL){
        fprintf(stderr, "Error: malloc failed\n");
        exit(99);
    }
	bst_node->value = token;
	return bst_node;

}


/*
 * Zrušení celého stromu.
 *
 * Po zrušení se celý strom bude nacházet ve stejném stavu jako po
 * inicializaci. Funkce korektně uvolní všechny alokované zdroje rušených
 * uzlů.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_dispose(bst_node_t* tree) {
	if (tree == NULL) {
		return;
	}

	bst_dispose(tree->left);
	bst_dispose(tree->right);

	free(tree);
}

