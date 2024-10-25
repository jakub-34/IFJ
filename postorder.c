#include <stdio.h>

#include "ast2.h"
#include "btree.h"

void create_postfix(bst_node_t *node, AST *ast){
    if (node != NULL){
        create_postfix(node->left, ast);
        create_postfix(node->right, ast);
        create_node(node->value, ast);
    }
}
