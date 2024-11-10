#ifndef AST_H
#define AST_H

#include "token.h"

// AST nodes
typedef struct Node{
    struct Node *next;  // right
    struct Node *newLine; // left
    token_t *token;
} ASTNode;

// Structure representing AST with extra helpful infos
typedef struct {
    ASTNode *root;
    ASTNode *active;
    ASTNode *currentLine;
} AST;


AST *createAST();
void create_node(token_t *token, AST *ast);
ASTNode *next_node(AST *ast);
void printAST(AST *ast);
void destroyAST(AST *ast);

#endif //AST_H