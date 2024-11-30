#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"

// Function that calls all other necessarry functions and generates code for the given AST
void generate_code(AST *ast);

#endif //CODEGEN_H