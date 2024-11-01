#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include "token.h"
#include "ast.h"

token_t* expression(token_t *token, AST *ast);

#endif