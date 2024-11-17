#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "token.h"
#include "expression.h"
#include "ast.h"
#include "codegen.h"
#include "semantics.h"


// needed declarations
token_t* in_param(token_t *token,  AST *ast);
token_t* in_param_continuation(token_t *token,  AST *ast);
token_t* param(token_t *token,  AST *ast);
token_t* param_continuation(token_t *token, AST *ast);


// <VARIABLE>
int variable(token_t *token){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error 1\n");
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
        fprintf(stderr, "Syntax error 2\n");
        exit(2);
    }
    if(token->type == type_token){
        return;
    }

    if(strcmp(token->data, "void") == 0){
        return;
    }

    fprintf(stderr, "Syntax error 3\n");
    exit(2);
}


// <TERM>
int term(token_t *token){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error 4\n");
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
token_t *in_param_continuation(token_t *token, AST *ast){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error 5\n");
        exit(2);
    }

    // <IN_PARAM_CONTINUATION> -> , <IN_PARAM>
    if(strcmp(token->data, ",") == 0){
        token = get_token();
        create_node(token, ast);
        token = in_param(token, ast);
        return token;
    }

    // <IN_PARAM_CONTINUATION> -> ε
    return token;
}


// <IN_PARAM>
// !!! don't call get_token() after this function !!!
token_t *in_param(token_t *token, AST *ast){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error 6\n");
        exit(2);
    }

    // <IN_PARAM> -> ID <IN_PARAM_CONTINUATION>
    if(token->type == identifier_token){
        token = get_token();
        create_node(token, ast);
        token = in_param_continuation(token, ast);
        return token;
    }

    // <IN_PARAM> -> <TERM> <IN_PARAM_CONTINUATION>
    if(term(token) == 0){
        token = get_token();
        create_node(token, ast);
        token = in_param_continuation(token, ast);
        return token;
    }

    // <IN_PARAM> -> ε
    return token;
}


// <NEXT_VARIABLE_CONTINUATION>
token_t *next_variable_continuaton(token_t *token, AST *ast){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error 7\n");
        exit(2);
    }

    // <NEXT_VARIABLE_CONTINUATION> -> <EXPRESSION>
    if(token->type == null_token){
        create_node(token, ast);
        token = get_token();
        create_node(token, ast);
    }
    else{
        token = expression(token, ast);
        create_node(token, ast);
    }

    if(strcmp(token->data, ";") == 0){
        return token;
    }

    // <NEXT_VARIABLE_CONTINUATION> -> ID ( <IN_PARAM> )
    if(token->type == eof_token || strcmp(token->data, "(") != 0){
        fprintf(stderr, "Syntax error 8\n");
        exit(2);
    }
    token = get_token();
    create_node(token, ast);
    token = in_param(token, ast);
    if(token->type == eof_token || strcmp(token->data, ")") != 0){
        fprintf(stderr, "Syntax error 9\n");
        exit(2);
    }
    token = get_token();
    create_node(token, ast);
    return token;

    fprintf(stderr, "Syntax error 10\n");
    exit(2);
}


// <VARIABLE_CONTINUATION>
token_t *variable_continuation(token_t *token, AST *ast){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error 11\n");
        exit(2);
    }

    // <VARIABLE_CONTINUATION> -> : <TYPE> = <NEXT_VARIABLE_CONTINUATION>
    if(strcmp(token->data, ":") == 0){
        token = get_token();
        create_node(token, ast);
        type(token);
        token = get_token();
        create_node(token, ast);
        if(token->type == eof_token || strcmp(token->data, "=") != 0){
            fprintf(stderr, "Syntax error 12\n");
            exit(2);
        }
        token = get_token();
        token = next_variable_continuaton(token, ast);
        return token;
    }

    // <VARIABLE_CONTINUATION> -> = <NEXT_VARIABLE_CONTINUATION>
    if(strcmp(token->data, "=") == 0){
        token = get_token();
        token = next_variable_continuaton(token, ast);
        return token;
    }

    fprintf(stderr, "Syntax error 13\n");
    exit(2);
}


// <NEXT_ID_DEFINING>
token_t *next_id_defining(token_t *token, AST *ast){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error 14\n");
        exit(2);
    }

    // <NEXT_ID_DEFINING> -> <EXPRESSION>
    if(token->type == null_token){
        create_node(token, ast);
        token = get_token();
        create_node(token, ast);
    }
    else{
        token = expression(token, ast);
        create_node(token, ast);
    }

    if(strcmp(token->data, ";") == 0){
        return token;
    }

    // <NEXT_ID_DEFINING> -> ID ( <IN_PARAM> )
    if(strcmp(token->data, "(") == 0){
        token = get_token();
        create_node(token, ast);
        token = in_param(token, ast);
        if(token->type == eof_token || strcmp(token->data, ")") != 0){
            fprintf(stderr, "Syntax error 15\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        return token;
    }

    fprintf(stderr, "Syntax error 16\n");
    exit(2);
}



// <ID_DEFINING>
token_t *id_defining(token_t *token, AST *ast){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error 17\n");
        exit(2);
    }

    // <ID_DEFINING> -> = <NEXT_ID_DEFINING>
    if(strcmp(token->data, "=") == 0){
        token = get_token();
        token = next_id_defining(token, ast);
        return token;
    }

    // <ID_DEFINING> -> ( <IN_PARAM> )
    if(strcmp(token->data, "(") == 0){
        token = get_token();
        create_node(token, ast);
        token = in_param(token, ast);
        if(token->type == eof_token || strcmp(token->data, ")") != 0){
            fprintf(stderr, "Syntax error 18\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        return token;
    }

    fprintf(stderr, "Syntax error 19\n");
    exit(2);
}


// <WHILE_IF_EXTENSION>
// !!! don't call get_token() after this function !!!
token_t *while_if_extension(token_t *token, AST *ast){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error 20\n");
        exit(2);
    }

    // <WHILE_IF_EXTENSION> -> | ID |
    if(strcmp(token->data, "|") == 0){
        token = get_token();
        create_node(token, ast);
        if(token->type != identifier_token){
            fprintf(stderr, "Syntax error 21\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        if(token->type == eof_token || strcmp(token->data, "|") != 0){
            fprintf(stderr, "Syntax error 22\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        return token;
    }

    // <WHILE_IF_EXTENSION> -> ε
    return token;  
}


// <PARAM_CONTINUATION>
// !!! don't call get_token() after this function !!!
token_t *param_continuation(token_t *token, AST *ast){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error 23\n");
        exit(2);
    }

    // <PARAM_CONTINUATION> -> , <PARAM>
    if(strcmp(token->data, ",") == 0){
        token = get_token();
        create_node(token, ast);
        token = param(token, ast);
        return token;
    }

    // <PARAM_CONTINUATION> -> ε
    return token;
}



// <PARAM>
// !!! don't call get_token() after this function !!!
token_t *param(token_t *token, AST *ast){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error 24\n");
        exit(2);
    }

    //<PARAM> -> ID : <TYPE> <PARAM_CONTINUATION>
    if(token->type == identifier_token){
        token = get_token();
        create_node(token, ast);
        if(token->type == eof_token || strcmp(token->data, ":") != 0){
            fprintf(stderr, "Syntax error 25\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        type(token);
        token = get_token();
        create_node(token, ast);
        token = param_continuation(token, ast);
        return token;
    }

    // <PARAM> -> ε
    return token;
}


// <RETURN_VALUE>
// !!! don't call get_token() after this function !!!
token_t *return_value(token_t *token, AST *ast){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error 26\n");
        exit(2);
    }

    if(token->type == null_token){
        create_node(token, ast);
        token = get_token();
        create_node(token, ast);
        return token;
    }
    else if(token->type == semicolon_token){
        create_node(token, ast);
        return token;
    }
    else{
        token = expression(token, ast);
        create_node(token, ast);
        return token;
    }

    // <RETURN_VALUE> -> ε
    return token;
}


// <FUNC_EXTENSION>
// !!! don't call get_token() after this function !!!
token_t *func_extension(token_t *token, AST *ast){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error 27\n");
        exit(2);
    }

    // <FUNC_EXTENSION> -> return <RETURN_VALUE>
    if(strcmp(token->data, "return") == 0){
        token = get_token();
        token = return_value(token, ast);
        if(token->type == eof_token || strcmp(token->data, ";") != 0){
            fprintf(stderr, "Syntax error 28\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        return token;
    }

    // <FUNC_EXTENSION> -> ε
    return token;
}


// <CODE_SEQUENCE>
// !!! don't call get_token() after this function !!!
token_t *code_sequence(token_t *token, AST *ast){
    if(token->type == eof_token){
        fprintf(stderr, "Syntax error 29\n");
        exit(2);
    }

    // <CODE_SEQUENCE> -> <VARIABLE> ID <VARIABLE_CONTINUATION> ; <CODE_SEQUENCE>
    if(variable(token) == 0){
        token = get_token();
        create_node(token, ast);
        if(token->type != identifier_token){
            fprintf(stderr, "Syntax error 30\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        token = variable_continuation(token, ast);
        if(token->type == eof_token || strcmp(token->data, ";") != 0){
            fprintf(stderr, "Syntax error 31\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        token = code_sequence(token, ast);
        return token;
    }


    // <CODE_SEQUENCE> -> ID <ID_DEFINING> ; <CODE_SEQUENCE>
    if(token->type == identifier_token){
        token = get_token();
        create_node(token, ast);
        token = id_defining(token, ast);
        if(token->type == eof_token || strcmp(token->data, ";") != 0){
            fprintf(stderr, "Syntax error 32\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        token = code_sequence(token, ast);
        return token;
    }

    // <CODE_SEQUENCE> -> if ( <EXPRESSION> ) <WHILE_IF_EXTENSION> { <CODE_SEQUENCE> } else { <CODE_SEQUENCE> } <CODE_SEQUENCE>
    if(token->type != eof_token && strcmp(token->data, "if") == 0){
        token = get_token();
        create_node(token, ast);
        if(token->type == eof_token || strcmp(token->data, "(") != 0){
            fprintf(stderr, "Syntax error 33\n");
            exit(2);
        }
        token = get_token();
        if(token->type == null_token){
            create_node(token, ast);
            token = get_token();
            create_node(token, ast);
        }
        else{
            token = expression(token, ast);
            create_node(token, ast);
        }
        if(token->type == eof_token || strcmp(token->data, ")") != 0){
            fprintf(stderr, "Syntax error 34\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        token = while_if_extension(token, ast);
        if(token->type == eof_token || strcmp(token->data, "{") != 0){
            fprintf(stderr, "Syntax error 35\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        token = code_sequence(token, ast);
        if(token->type == eof_token || strcmp(token->data, "}") != 0){
            fprintf(stderr, "Syntax error 36\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        if(token->type == eof_token || strcmp(token->data, "else") != 0){
            fprintf(stderr, "Syntax error 37\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        if(token->type == eof_token || strcmp(token->data, "{") != 0){
            fprintf(stderr, "Syntax error 38\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        token = code_sequence(token, ast);
        if(token->type == eof_token || strcmp(token->data, "}") != 0){
            fprintf(stderr, "Syntax error 39\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        token = code_sequence(token, ast);
        return token;
    }

    // <CODE_SEQUENCE> -> while ( <EXPRESSION> ) <WHILE_IF_EXTENSION> { <CODE_SEQUENCE> } <CODE_SEQUENCE>
    if(token->type != eof_token && strcmp(token->data, "while") == 0){
        token = get_token();
        create_node(token, ast);
        if(token->type == eof_token || strcmp(token->data, "(") != 0){
            fprintf(stderr, "Syntax error 40\n");
            exit(2);
        }
        token = get_token();
        if(token->type == null_token){
            create_node(token, ast);
            token = get_token();
            create_node(token, ast);
        }
        else{
            token = expression(token, ast);
            create_node(token, ast);
        }
        if(token->type == eof_token || strcmp(token->data, ")") != 0){
            fprintf(stderr, "Syntax error 41\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        token = while_if_extension(token, ast);
        if(token->type == eof_token || strcmp(token->data, "{") != 0){
            fprintf(stderr, "Syntax error 42\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        token = code_sequence(token, ast);
        if(token->type == eof_token || strcmp(token->data, "}") != 0){
            fprintf(stderr, "Syntax error 43\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        token = code_sequence(token, ast);
        return token;
    }

    // <CODE_SEQUENCE> -> <FUNC_EXTENSION>
    token = func_extension(token, ast);

    // <CODE_SEQUENCE> -> ε
    return token;
}


// <CODE>
void code(token_t *token, AST *ast){
    // <CODE> -> EOF
    if(token->type == eof_token){
        return;
    }

    // <CODE> -> <VARIABLE> ID <VARIABLE_CONTINUATION> ; <CODE>
    if(variable(token) == 0){
        token = get_token();
        create_node(token, ast);
        if(token->type != identifier_token){
            fprintf(stderr, "Syntax error 44\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        token = variable_continuation(token, ast);
        if(token->type == eof_token || strcmp(token->data, ";") != 0){
            fprintf(stderr, "Syntax error 45\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        code(token, ast);
        return;
    }

    // <CODE> -> ID <ID_DEFINING> ; <CODE>
    if(token->type == identifier_token){
        token = get_token();
        create_node(token, ast);
        token = id_defining(token, ast);
        if(token->type == eof_token || strcmp(token->data, ";") != 0){
            fprintf(stderr, "Syntax error 46\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        code(token, ast);
        return;
    }

    // <CODE> -> if ( <EXPRESSION> ) <WHILE_IF_EXTENSION> { <CODE_SEQUENCE> } else { <CODE_SEQUENCE> } <CODE>
    if(token->type != eof_token && strcmp(token->data, "if") == 0){
        token = get_token();
        create_node(token, ast);
        if(token->type == eof_token || strcmp(token->data, "(") != 0){
            fprintf(stderr, "Syntax error 47\n");
            exit(2);
        }
        token = get_token();
        if(token->type == null_token){
            create_node(token, ast);
            token = get_token();
            create_node(token, ast);
        }
        else{
            token = expression(token, ast);
            create_node(token, ast);
        }
        if(token->type == eof_token || strcmp(token->data, ")") != 0){
            fprintf(stderr, "Syntax error 48\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        token = while_if_extension(token, ast);
        if(token->type == eof_token || strcmp(token->data, "{") != 0){
            fprintf(stderr, "Syntax error 49\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        token = code_sequence(token, ast);
        if(token->type == eof_token || strcmp(token->data, "}") != 0){
            fprintf(stderr, "Syntax error 50\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        if(token->type == eof_token || strcmp(token->data, "else") != 0){
            fprintf(stderr, "Syntax error 51\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        if(token->type == eof_token || strcmp(token->data, "{") != 0){
            fprintf(stderr, "Syntax error 52\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        token = code_sequence(token, ast);
        if(token->type == eof_token || strcmp(token->data, "}") != 0){
            fprintf(stderr, "Syntax error 53\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        code(token, ast);
        return;
    }

    // <CODE> -> while ( <EXPRESSION> ) <WHILE_IF_EXTENSION> { <CODE_SEQUENCE> } <CODE>
    if(token->type != eof_token && strcmp(token->data, "while") == 0){
        token = get_token();
        create_node(token, ast);
        if(token->type == eof_token || strcmp(token->data, "(") != 0){
            fprintf(stderr, "Syntax error 54\n");
            exit(2);
        }
        token = get_token();
        if(token->type == null_token){
            create_node(token, ast);
            token = get_token();
            create_node(token, ast);
        }
        else{
            token = expression(token, ast);
            create_node(token, ast);
        }
        if(token->type == eof_token || strcmp(token->data, ")") != 0){
            fprintf(stderr, "Syntax error 55\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        token = while_if_extension(token, ast);
        if(token->type == eof_token || strcmp(token->data, "{") != 0){
            fprintf(stderr, "Syntax error 56\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        token = code_sequence(token, ast);
        if(token->type == eof_token || strcmp(token->data, "}") != 0){
            fprintf(stderr, "Syntax error 57\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        code(token, ast);
        return;
    }

    // <CODE> -> pub fn ID ( <PARAM> ) <TYPE> { <CODE_SEQUENCE> } <CODE>
    if(token->type != eof_token && strcmp(token->data, "pub") == 0){
        token = get_token();
        create_node(token, ast);
        if(token->type == eof_token || strcmp(token->data, "fn") != 0){
            fprintf(stderr, "Syntax error 58\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        if(token->type != identifier_token){
            fprintf(stderr, "Syntax error 59\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        if(token->type == eof_token || strcmp(token->data, "(") != 0){
            fprintf(stderr, "Syntax error 60\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        token = param(token, ast);
        if(token->type == eof_token || strcmp(token->data, ")") != 0){
            fprintf(stderr, "Syntax error 61\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        type(token);
        token = get_token();
        create_node(token, ast);
        if(token->type == eof_token || strcmp(token->data, "{") != 0){
            fprintf(stderr, "Syntax error 62\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        token = code_sequence(token, ast);
        if(token->type == eof_token || strcmp(token->data, "}") != 0){
            fprintf(stderr, "Syntax error 63\n");
            exit(2);
        }
        token = get_token();
        create_node(token, ast);
        code(token, ast);
        return;
    }

    fprintf(stderr, "Syntax error 64\n");
    exit(2);
}


int main(){
    // AST Initialization
    AST *ast = createAST();
    
    token_t *token = get_token();

    // Check for header
    if(token->type == eof_token || strcmp(token->data, "const") != 0){
        fprintf(stderr, "Syntax error 65\n");
        exit(2);
    }
    token = get_token();
    if(token->type == eof_token || strcmp(token->data, "ifj") != 0){
        fprintf(stderr, "Syntax error 66\n");
        exit(2);
    }
    token = get_token();
    if(token->type != equal_token){
        fprintf(stderr, "Syntax error 67\n");
        exit(2);
    }
    token = get_token();
    if(token->type != import_token){
        fprintf(stderr, "Syntax error 68\n");
        exit(2);
    }
    token = get_token();
    if(token->type == eof_token || strcmp(token->data, "(") != 0){
        fprintf(stderr, "Syntax error 69\n");
        exit(2);
    }
    token = get_token();
    if(token->type == eof_token || strcmp(token->data, "ifj24.zig") != 0){
        fprintf(stderr, "Syntax error 70\n");
        exit(2);
    }
    token = get_token();
    if(token->type == eof_token || strcmp(token->data, ")") != 0){
        fprintf(stderr, "Syntax error 71\n");
        exit(2);
    }
    token = get_token();
    if(token->type == eof_token || strcmp(token->data, ";") != 0){
        fprintf(stderr, "Syntax error 72\n");
        exit(2);
    }
    // First token after prolog
    token = get_token();
    create_node(token, ast);

    code(token, ast);

    semantic_analysis(ast);

    ast->active = NULL;
    generate_code(ast);

    destroyAST(ast);

    // printf("Syntax OK\n");

    return 0;
}