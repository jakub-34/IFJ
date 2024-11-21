#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ast.h"


// Creates and initializes AST
// Returns pointer to the newly created AST
AST *create_ast() {
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

// Prints whole tree data with type
void print_ast(AST *ast){
    ast->active = ast->root;

    while(ast->active != NULL && ast->active->token->type != eof_token){
        printf("%s\t\t", ast->active->token->data);
        printf("type: %d\n", ast->active->token->type);
        next_node(ast);
    }
}

// Frees the whole tree
void destroy_ast(AST *ast){
    ast->active = ast->root;
    ASTNode *tmp = NULL;

    // Frees all nodes
    while(ast->active != NULL && ast->active->token->type != eof_token){
        tmp = ast->active;
        next_node(ast);
        free(tmp->token->data);
        free(tmp->token);
        free(tmp);
    }

    // and then the structure for tree itself
    free(ast);
}
