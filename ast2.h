#ifndef AST_H
#define AST_H

#include "token.h"

// Tree nodes
typedef struct Node{
    struct Node *next;  // right
    struct Node *newLine; // left
    token_t *token;
} ASTNode;


typedef struct {
    ASTNode *root;
    ASTNode *active;
    ASTNode *currentLine;
} AST;


AST *createAST();

void create_node(token_t *token, AST *ast);

// void create_child(token_t *token, AST *ast);

ASTNode *next_node(AST *ast);


#endif