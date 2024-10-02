#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "token.h"


// <VARIABLE>
int variable(token_t *token){
    // <VARIABLE> -> var
    if(strcmp(token->data, "var") == 0){
        return 0;
    }

    // <VARIABLE> -> const
    if(strcmp(token->data, "const") == 0){
        return 0;
    }

    return 1;
}


// <TYPE>
void type(token_t *token){
    // TODO
    return 0;
}


// <IN_PARAM_CONTINUATION>
// !!! don't call get_token() after this function !!!
token_t *in_param_continuation(token_t *token){
    // <IN_PARAM_CONTINUATION> -> , <IN_PARAM>
    if(strcmp(token->data, ",") == 0){
        token = get_token();
        token = in_param(token);
        return token;
    }

    // <IN_PARAM_CONTINUATION> -> ε
    return token;
}


// <IN_PARAM>
// !!! don't call get_token() after this function !!!
token_t *in_param(token_t *token){
    // <IN_PARAM> -> ID <IN_PARAM_CONTINUATION>
    if(token->type == identifier_token){
        token = get_token();
        token = in_param_continuation(token);
        return token;
    }

    // <IN_PARAM> -> <TERM> <IN_PARAM_CONTINUATION>
    if(term() == 0){
        token = get_token();
        token = in_param_continuation(token);
        return token;
    }

    // <IN_PARAM> -> ε
    return token;
}


// <NEXT_VARIABLE_CONTINUATION>
void next_variable_continuaton(token_t *token){
    // TODO expression

    // <NEXT_VARIABLE_CONTINUATION> -> ID ( <IN_PARAM> )
    if(token->type == identifier_token){
        token = get_token();
        if(strcmp(token->data, "(") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = in_param(token);
        if(strcmp(token->data, ")") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        return;
    }

    fprintf(stderr, "Syntax error\n");
    exit(2);
}


// <VARIABLE_CONTINUATION>
void variable_continuation(token_t *token){
    // <VARIABLE_CONTINUATION> -> : <TYPE> = <NEXT_VARIABLE_CONTINUATION>
    if(strcmp(token->data, ":") == 0){
        token = get_token();
        type(token);
        token = get_token();
        if(strcmp(token->data, "=") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        next_variable_continuaton(token);
        return;
    }

    // <VARIABLE_CONTINUATION> -> = <NEXT_VARIABLE_CONTINUATION>
    if(strcmp(token->data, "=") == 0){
        token = get_token();
        next_variable_continuaton(token);
        return;
    }

    fprintf(stderr, "Syntax error\n");
    exit(2);
}


// <CODE>
void code(token_t *token){
    // <CODE> -> <VARIABLE> ID <VARIABLE_CONTINUATION> ; <CODE>
    if(variable(token) == 0){
        token = get_token();
        if(token->type != identifier_token){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        variable_continuation(token);
        token = get_token();
        if(strcmp(token->data, ";") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        code(token);
    }
}


int main(){
    token_t *token = get_token();

    code(token);
}