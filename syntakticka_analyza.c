#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "token.h"
#include "expression.h"


// needed declarations
token_t* in_param(token_t *token);
token_t* in_param_continuation(token_t *token);
token_t* param(token_t *token);
token_t* param_continuation(token_t *token);


// <VARIABLE>
int variable(token_t *token){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }

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
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }
    if(token->type == type_token){
        return;
    }

    if(strcmp(token->data, "void") == 0){
        return;
    }

    fprintf(stderr, "Syntax error\n");
    exit(2);
}


// <TERM>
int term(token_t *token){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }
    // <TERM> -> nejaky_int
    if(token->type == int_token){
        return 0;
    }

    // <TERM> -> nejaky_float
    if(token->type == float_token){
        return 0;
    }

    // <TERM> -> nejaky_string
    if(token->type == string_token){
        return 0;
    }

    // <TERM> -> NULL
    if(token->type == null_token){
        return 0;
    }

    return 1;
}


// <IN_PARAM_CONTINUATION>
// !!! don't call get_token() after this function !!!
token_t *in_param_continuation(token_t *token){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }

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
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }

    // <IN_PARAM> -> ID <IN_PARAM_CONTINUATION>
    if(token->type == identifier_token){
        token = get_token();
        token = in_param_continuation(token);
        return token;
    }

    // <IN_PARAM> -> <TERM> <IN_PARAM_CONTINUATION>
    if(term(token) == 0){
        token = get_token();
        token = in_param_continuation(token);
        return token;
    }

    // <IN_PARAM> -> ε
    return token;
}


// <NEXT_VARIABLE_CONTINUATION>
token_t *next_variable_continuaton(token_t *token){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }

    // <NEXT_VARIABLE_CONTINUATION> -> ID ( <IN_PARAM> )
    if(token->type == identifier_token){
        token = get_token();
        if(token->type == eof_token || strcmp(token->data, "(") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = in_param(token);
        if(token->type == eof_token || strcmp(token->data, ")") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        return token;

    }

    // <NEXT_VARIABLE_CONTINUATION> -> <EXPRESSION>
    if(token->type == null_token){
        token = get_token();
    }
    else{
        token = expression(token);
    }

    if(strcmp(token->data, ";") == 0){
        return token;
    }

    fprintf(stderr, "Syntax error\n");
    exit(2);
}


// <VARIABLE_CONTINUATION>
token_t *variable_continuation(token_t *token){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }

    // <VARIABLE_CONTINUATION> -> : <TYPE> = <NEXT_VARIABLE_CONTINUATION>
    if(strcmp(token->data, ":") == 0){
        token = get_token();
        type(token);
        token = get_token();
        if(token->type == eof_token || strcmp(token->data, "=") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = next_variable_continuaton(token);
        return token;
    }

    // <VARIABLE_CONTINUATION> -> = <NEXT_VARIABLE_CONTINUATION>
    if(strcmp(token->data, "=") == 0){
        token = get_token();
        token = next_variable_continuaton(token);
        return token;
    }

    fprintf(stderr, "Syntax error\n");
    exit(2);
}


// <NEXT_ID_DEFINING>
token_t *next_id_defining(token_t *token){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }

    // <NEXT_ID_DEFINING> -> <EXPRESSION>
    if(token->type == null_token){
        token = get_token();
    }
    else{
        token = expression(token);
    }

    if(strcmp(token->data, ";") == 0){
        return token;
    }

    // <NEXT_ID_DEFINING> -> ID ( <IN_PARAM> )
    if(strcmp(token->data, "(") == 0){
        token = get_token();
        token = in_param(token);
        if(token->type == eof_token || strcmp(token->data, ")") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        return token;
    }

    fprintf(stderr, "Syntax error\n");
    exit(2);
}



// <ID_DEFINING>
token_t *id_defining(token_t *token){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }

    // <ID_DEFINING> -> = <NEXT_ID_DEFINING>
    if(strcmp(token->data, "=") == 0){
        token = get_token();
        printf("kokot %s\n", token->data);
        token = next_id_defining(token);
        printf("kokot %s\n", token->data);
        return token;
    }

    // <ID_DEFINING> -> ( <IN_PARAM> )
    if(strcmp(token->data, "(") == 0){
        token = get_token();
        token = in_param(token);
        if(token->type == eof_token || strcmp(token->data, ")") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        return token;
    }

    fprintf(stderr, "Syntax error\n");
    exit(2);
}


// <WHILE_IF_EXTENSION>
// !!! don't call get_token() after this function !!!
token_t *while_if_extension(token_t *token){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }

    // <WHILE_IF_EXTENSION> -> | ID |
    if(strcmp(token->data, "|") == 0){
        token = get_token();
        if(token->type != identifier_token){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        if(token->type == eof_token || strcmp(token->data, "|") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        printf("ddddddddd\n");
        return token;
    }

    // <WHILE_IF_EXTENSION> -> ε
    return token;  
}


// <PARAM_CONTINUATION>
// !!! don't call get_token() after this function !!!
token_t *param_continuation(token_t *token){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }

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
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }

    //<PARAM> -> ID : <TYPE> <PARAM_CONTINUATION>
    if(token->type == identifier_token){
        token = get_token();
        if(token->type == eof_token || strcmp(token->data, ":") != 0){
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
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }

    if(token->type == null_token){
        token = get_token();
    }
    else{
        token = expression(token);
    }

    // <RETURN_VALUE> -> ε
    return token;
}


// <FUNC_EXTENSION>
// !!! don't call get_token() after this function !!!
token_t *func_extension(token_t *token){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }

    // <FUNC_EXTENSION> -> return <RETURN_VALUE>
    if(strcmp(token->data, "return") == 0){
        token = get_token();
        token = return_value(token);
        if(token->type == eof_token || strcmp(token->data, ";") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        return token;
    }

    // <FUNC_EXTENSION> -> ε
    return token;
}


// <CODE_SEQUENCE>
// !!! don't call get_token() after this function !!!
token_t *code_sequence(token_t *token){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }

    // <CODE_SEQUENCE> -> <VARIABLE> ID <VARIABLE_CONTINUATION> ; <CODE_SEQUENCE>
    if(variable(token) == 0){
        token = get_token();
        if(token->type != identifier_token){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = variable_continuation(token);
        if(token->type == eof_token || strcmp(token->data, ";") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = code_sequence(token);
        return token;
    }


    // <CODE_SEQUENCE> -> ID <ID_DEFINING> ; <CODE_SEQUENCE>
    if(token->type == identifier_token){
        token = get_token();
        token = id_defining(token);;
        if(token->type == eof_token || strcmp(token->data, ";") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = code_sequence(token);
        return token;
    }

    // <CODE_SEQUENCE> -> if ( <EXPRESSION> ) <WHILE_IF_EXTENSION> { <CODE_SEQUENCE> } else { <CODE_SEQUENCE> } <CODE_SEQUENCE>
    if(token->type != eof_token && strcmp(token->data, "if") == 0){
        token = get_token();
        if(token->type == eof_token || strcmp(token->data, "(") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        if(token->type == null_token){
            token = get_token();
        }
        else{
            token = expression(token);
        }
        if(token->type == eof_token || strcmp(token->data, ")") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = while_if_extension(token);
        if(token->type == eof_token || strcmp(token->data, "{") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = code_sequence(token);
        if(token->type == eof_token || strcmp(token->data, "}") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        if(token->type == eof_token || strcmp(token->data, "else") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        if(token->type == eof_token || strcmp(token->data, "{") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = code_sequence(token);
        if(token->type == eof_token || strcmp(token->data, "}") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = code_sequence(token);
        return token;
    }

    // <CODE_SEQUENCE> -> while ( <EXPRESSION> ) <WHILE_IF_EXTENSION> { <CODE_SEQUENCE> } <CODE_SEQUENCE>
    if(token->type != eof_token && strcmp(token->data, "while") == 0){
        token = get_token();
        if(token->type == eof_token || strcmp(token->data, "(") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        if(token->type == null_token){
            token = get_token();
        }
        else{
            token = expression(token);
        }
        if(token->type == eof_token || strcmp(token->data, ")") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = while_if_extension(token);
        if(token->type == eof_token || strcmp(token->data, "{") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = code_sequence(token);
        if(token->type == eof_token || strcmp(token->data, "}") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = code_sequence(token);
        return token;
    }

    // <CODE_SEQUENCE> -> ε
    return token;
}


// <CODE>
void code(token_t *token){
    // <CODE> -> EOF
    if(token->type == eof_token){
        return;
    }

    // <CODE> -> <VARIABLE> ID <VARIABLE_CONTINUATION> ; <CODE>
    if(variable(token) == 0){
        token = get_token();
        if(token->type != identifier_token){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = variable_continuation(token);
        if(token->type == eof_token || strcmp(token->data, ";") != 0){
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
        token = id_defining(token);
        if(token->type == eof_token || strcmp(token->data, ";") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        code(token);
        return;
    }

    // <CODE> -> if ( <EXPRESSION> ) <WHILE_IF_EXTENSION> { <CODE_SEQUENCE> } else { <CODE_SEQUENCE> } <CODE>
    if(token->type != eof_token && strcmp(token->data, "if") == 0){
        token = get_token();
        if(token->type == eof_token || strcmp(token->data, "(") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        if(token->type == null_token){
            token = get_token();
        }
        else{
            token = expression(token);
        }
        if(token->type == eof_token || strcmp(token->data, ")") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = while_if_extension(token);
        if(token->type == eof_token || strcmp(token->data, "{") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = code_sequence(token);
        if(token->type == eof_token || strcmp(token->data, "}") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        if(token->type == eof_token || strcmp(token->data, "else") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        if(token->type == eof_token || strcmp(token->data, "{") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = code_sequence(token);
        if(token->type == eof_token || strcmp(token->data, "}") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        code(token);
        return;
    }

    // <CODE> -> while ( <EXPRESSION> ) <WHILE_IF_EXTENSION> { <CODE_SEQUENCE> } <CODE>
    if(token->type != eof_token && strcmp(token->data, "while") == 0){
        token = get_token();
        if(token->type == eof_token || strcmp(token->data, "(") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        if(token->type == null_token){
            token = get_token();
        }
        else{
            token = expression(token);
        }
        if(token->type == eof_token || strcmp(token->data, ")") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = while_if_extension(token);
        if(token->type == eof_token || strcmp(token->data, "{") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = code_sequence(token);
        if(token->type == eof_token || strcmp(token->data, "}") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        code(token);
        return;
    }

    // <CODE> -> pub fn ID ( <PARAM> ) <TYPE> { <CODE_SEQUENCE> <FUNC_EXTENSION> } <CODE>
    if(token->type != eof_token && strcmp(token->data, "pub") == 0){
        token = get_token();
        if(token->type == eof_token || strcmp(token->data, "fn") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        if(token->type != identifier_token){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        if(token->type == eof_token || strcmp(token->data, "(") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = param(token);
        if(token->type == eof_token || strcmp(token->data, ")") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        type(token);
        token = get_token();
        if(token->type == eof_token || strcmp(token->data, "{") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        token = code_sequence(token);
        token = func_extension(token);
        if(token->type == eof_token || strcmp(token->data, "}") != 0){
            fprintf(stderr, "Syntax error\n");
            exit(2);
        }
        token = get_token();
        code(token);
        return;
    }

    fprintf(stderr, "Syntax error\n");
    exit(2);
}


int main(){
    token_t *token = get_token();

    // Check for header
    if(token->type == eof_token || strcmp(token->data, "const") != 0){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }
    token = get_token();
    if(token->type == eof_token || strcmp(token->data, "ifj") != 0){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }
    token = get_token();
    if(token->type != equal_token){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }
    token = get_token();
    if(token->type != import_token){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }
    token = get_token();
    if(token->type == eof_token || strcmp(token->data, "(") != 0){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }
    token = get_token();
    if(token->type == eof_token || strcmp(token->data, "ifj24.zig") != 0){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }
    token = get_token();
    if(token->type == eof_token || strcmp(token->data, ")") != 0){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }
    token = get_token();
    if(token->type == eof_token || strcmp(token->data, ";") != 0){
        fprintf(stderr, "Syntax error\n");
        exit(2);
    }
    token = get_token();

    code(token);

    printf("Syntax OK\n");

    return 0;
}