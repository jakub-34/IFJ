#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ast.h"


// Creates and initializes AST
// Returns pointer to the newly created AST
AST *createAST() {
    AST *ast = malloc(sizeof(AST));
    if (ast == NULL) {
        fprintf(stderr, "Error allocating memory for AST\n");
        exit(99);
    }
    // Initializes to default values
    ast->active = NULL;
    ast->currentLine = NULL;
    ast->root = NULL;
    return ast;
}

// Creates new node, saves "token" as its value
// and moves ast->active to point to the new node
void create_node(token_t *token, AST *ast){
    ASTNode *node = malloc(sizeof(ASTNode)); // allocating for node
    if (node == NULL) {
        fprintf(stderr, "Error allocating memory for ASTNode\n");
        exit(99);
    }
    
    node->next = NULL;    // right child
    node->newLine = NULL; // left child
    node->token = token;

    // If theres no active node => tree is empty
    // Sets new node as root
    if (ast->active == NULL) {
        ast->root = node;
        ast->currentLine = node;
    }
    // Makes current active node point to new node
    else {
        ast->active->next = node;
    }
    // Moves the active pointer to new node
    ast->active = node;
}

// Moves ast->active to next node and returns value of ast->active
ASTNode *next_node(AST *ast) {
    // If tree was uninitialized
    if (ast->active == NULL){
        ast->active = ast->root;
    }
    else {
        // Moves the pointer
        ast->active = ast->active->next;
    }
    
    return ast->active;
}

// Prints whole tree
void printAST(AST *ast){
    ast->active = ast->root;

    while(ast->active != NULL && ast->active->token->type != eof_token){
        printf("%s\n", ast->active->token->data);
        next_node(ast);
    }
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