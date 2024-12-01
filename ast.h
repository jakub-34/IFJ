/*
* Project: Implementace překladače imperativního jazyka IFJ24
*
* @author: Jakub Lůčný <xlucnyj00>
* @author: Martin Ševčík <xsevcim00>
*
*/

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

// Creates and initializes new AST
AST *create_ast();

// Creates new node with token as data and saves it to ast
void create_node(token_t *token, AST *ast);

// Moves the active pointer to next node
ASTNode *next_node(AST *ast);

// Prints out ast with data and type of each node
void print_ast(AST *ast);

// Destroys ast and frees allocated memory
void destroy_ast(AST *ast);

#endif //AST_H