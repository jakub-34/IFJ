#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ast.h"


// Creates and initializes AST
AST *createAST() {
    AST *ast = malloc(sizeof(AST));
    if (ast == NULL) {
        fprintf(stderr, "Error allocating memory for AST\n");
        exit(99);
    }
    ast->active = NULL;
    ast->currentLine = NULL;
    ast->root = NULL;
    return ast;
}

// Creates new node and saves "token" as its value
void create_node(token_t *token, AST *ast){
    ASTNode *node = malloc(sizeof(ASTNode)); // allocating for node
    if (node == NULL) {
        fprintf(stderr, "Error allocating memory for ASTNode\n");
        exit(99);
    }
    
    node->next = NULL;    // right child
    node->newLine = NULL; // left child
    node->token = token;

    // If theres no active node == tree is empty
    // Sets new node as root
    if (ast->active == NULL) {
        ast->root = node;
        ast->currentLine = node;
    }
    // Makes current active node (last added to tree) point to new node
    else {
        ast->active->next = node;
    }

    // Moves the active pointer to new node
    ast->active = node;
}

ASTNode *next_node(AST *ast) {
    // If tree was uninitialized
    if (ast->active == NULL){
        return ast->root;
    }

    // Moves the pointer
    ast->active = ast->active->next;

    // printf("Next node: %s\n", ast->active->token->data);

    return ast->active;
}


// Prints value of every token in every node, (Could be used to free the tree??)
// void traverse_AST(AST *ast){
//     ast->active = ast->root;
//     ast->currentLine = ast->root;

//     ASTNode *node = ast->active;

//     while(node != NULL && node->newLine != NULL && node->next != NULL){
//         if (node->token != NULL){
//             printf("Token: %s\n", node->token->data);
//         }

//         node = next_node(ast);
//     }
// }