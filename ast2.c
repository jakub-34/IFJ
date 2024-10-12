#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ast2.h"


// Creates and initializes AST
AST *createAST() {
    AST *ast = malloc(sizeof(AST));
    if (ast == NULL) {
       exit(1000);                          // random number, idk (in every exit in this file)
    }
    ast->active = NULL;
    ast->currentLine = NULL;
    ast->root = NULL;
    return ast;
}

void create_node(token_t *token, AST *ast){
    ASTNode *node = malloc(sizeof(ASTNode)); // allocing for node
    if (node == NULL) {
        exit(1000);
    }
    
    node->next = NULL;    // right child
    node->newLine = NULL; // ptr to left child, for quick access back
    node->token = token;


    if (ast->active != NULL) {
        // If last added token was end of line character, we create a new line
        if (ast->active->token->data == semicolon_token || strcmp(ast->active->token->data, '{') == 0) {
            ast->currentLine->newLine = node;
        }
        // We just continue on the line
        else {
            ast->active->next = node; // last active node will point to new node
        }
    
    }
    // If no current grammar or active node, set as root
    else {
        ast->root = node;
    }

    ast->active = node;
}

ASTNode *next_node(AST *ast) {
    if (ast->active == NULL){
        return NULL;
    }

    if (ast->active->next == NULL) {
        ast->active = ast->currentLine->newLine; 
        ast->currentLine = ast->active;
    }

    else {
        ast->active = ast->active->next;
    }

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
