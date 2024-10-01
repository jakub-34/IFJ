#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "token.h"


int variable(token_t *token){
    return(((strcmp(token->data, "var") == 0) || (strcmp(token->data, "const") == 0)) ? 0 : 1);
}


void type(token_t *token){
    // TODO
    return 0;
}


// !!! don't call get_token() after this function !!!
token_t *in_param_continuation(token_t *token){
    if(strcmp(token->data, ",") == 0){
        token = get_token();
        in_param(token);
        return token;
    }
    return token;
}


// !!! don't call get_token() after this function !!!
token_t *in_param(token_t *token){
    if(token->type == identifier_token){
        token = get_token();
        token = in_param_continuation(token);
        return token;
    }

    if(term() == 0){
        token = get_token();
        token = in_param_continuation(token);
        return token;
    }

    return token;
}


void next_variable_continuaton(token_t *token){
    // TODO expression
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


void variable_continuation(token_t *token){
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

    if(strcmp(token->data, "=") == 0){
        token = get_token();
        next_variable_continuaton(token);
        return;
    }

    fprintf(stderr, "Syntax error\n");
    exit(2);
}


void code(token_t *token){
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