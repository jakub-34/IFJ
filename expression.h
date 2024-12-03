/*
* Project: Implementacia prekladaca imperativneho jazyka IFJ2024
*
* @author: Rebeka Tydorova <xtydor01>
*
*/
#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include "token.h"
#include "ast.h"

//process expression 
token_t* expression(token_t *token, AST *ast);

#endif