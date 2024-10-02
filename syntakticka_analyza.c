#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "token.h"


// needed declarations
token_t* in_param(token_t *token);
token_t* in_param_continuation(token_t *token);
token_t* param(token_t *token);
token_t* param_continuation(token_t *token);


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
    return;
}


// <TERM>
int term(){
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


// <NEXT_ID_DEFINING>
void next_id_defining(token_t *token){
    // <NEXT_ID_DEFINING> -> ID ( <IN_PARAM> )
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
    }

    // TODO expression
}



// <ID_DEFINING>
void id_defining(token_t *token){
    // <ID_DEFINING> -> = <NEXT_ID_DEFINING>
    if(strcmp(token->data, "=") == 0){
        token = get_token();
        next_id_defining(token);
        return;
    }

    // <ID_DEFINING> -> ( <IN_PARAM> )
    if(strcmp(token->data, "(") == 0){
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


// <WHILE_IF_EXTENSION>
// !!! don't call get_token() after this function !!!
token_t *while_if_extension(token_t *token){
    // <WHILE_IF_EXTENSION> -> | ID |
    if(strcmp(token->data, "|") == 0){
        token = get_token();
        if(token->type != identifier_token){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        if(strcmp(token->data, "|") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        return token;
    }

    // <WHILE_IF_EXTENSION> -> ε
    return token;  
}


// <CODE_SEQUENCE>
// !!! don't call get_token() after this function !!!
token_t *code_sequence(token_t *token){
    // TODO
    return token;
}


// <PARAM_CONTINUATION>
// !!! don't call get_token() after this function !!!
token_t *param_continuation(token_t *token){
    // <PARAM_CONTINUATION> -> , <PARAM>
    if(strcmp(token->data, ",") == 0){
        token = get_token();
        token = param(token);
        return token;
    }

    // <PARAM_CONTINUATION> -> ε
    return token;
}



// <PARAM>
// !!! don't call get_token() after this function !!!
token_t *param(token_t *token){
    //<PARAM> -> ID : <TYPE> <PARAM_CONTINUATION>
    if(token->type == identifier_token){
        token = get_token();
        if(strcmp(token->data, ":") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        type(token);
        token = get_token();
        token = param_continuation(token);
        return token;
    }

    // <PARAM> -> ε
    return token;
}


// <RETURN_VALUE>
// !!! don't call get_token() after this function !!!
token_t *return_value(token_t *token){
    // TODO expression

    // <RETURN_VALUE> -> ε
    return token;
}


// <FUNC_EXTENSION>
// !!! don't call get_token() after this function !!!
token_t *func_extension(token_t *token){
    // <FUNC_EXTENSION> -> return <RETURN_VALUE>
    if(strcmp(token->data, "return") == 0){
        token = get_token();
        token = return_value(token);
        return token;
    }

    // <FUNC_EXTENSION> -> ε
    return token;
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
        return;
    }

    // <CODE> -> ID <ID_DEFINING> ; <CODE>
    if(token->type == identifier_token){
        token = get_token();
        id_defining(token);
        token = get_token();
        if(strcmp(token->data, ";") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        code(token);
        return;
    }

    // <CODE> -> if ( <EXPRESSION> ) <WHILE_IF_EXTENSION> { <CODE_SEQUENCE> } else { <CODE_SEQUENCE> } <CODE>
    if(strcmp(token->data, "if") == 0){
        token = get_token();
        if(strcmp(token->data, "(") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        // TODO expression
        token = get_token();
        if(strcmp(token->data, ")") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = while_if_extension(token);
        if(strcmp(token->data, "{") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = code_sequence(token);
        if(strcmp(token->data, "}") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        if(strcmp(token->data, "else") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        if(strcmp(token->data, "{") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = code_sequence(token);
        if(strcmp(token->data, "}") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        code(token);
        return;
    }

    // <CODE> -> while ( <EXPRESSION> ) <WHILE_IF_EXTENSION> { <CODE_SEQUENCE> } <CODE>
    if(strcmp(token->data, "while") == 0){
        token = get_token();
        if(strcmp(token->data, "(") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        // TODO expression
        token = get_token();
        if(strcmp(token->data, ")") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = while_if_extension(token);
        if(strcmp(token->data, "{") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = code_sequence(token);
        if(strcmp(token->data, "}") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        code(token);
        return;
    }

    // <CODE> -> pub fn ID ( <PARAM> ) <TYPE> { <CODE_SEQUENCE> <FUNC_EXTENSION> } <CODE>
    if(strcmp(token->data, "pub") == 0){
        token = get_token();
        if(strcmp(token->data, "fn") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        if(token->type != identifier_token){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        if(strcmp(token->data, "(") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = param(token);
        if(strcmp(token->data, ")") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        type(token);
        token = get_token();
        if(strcmp(token->data, "{") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = code_sequence(token);
        token = func_extension(token);
        if(strcmp(token->data, "}") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        code(token);
        return;
    }

    // <CODE> -> EOF
    if(token->type == eof_token){
        return;
    }

    fprintf(stderr, "Syntax error\n");
    exit(2);
}


int main(){
    token_t *token = get_token();

    code(token);

    return 0;
}