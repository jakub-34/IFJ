#ifndef TOKEN_H
#define TOKEN_H


typedef enum token_type {
    identifier_token,
    int_token,
    string_token,
    keyword_token,
    eof_token
} token_type_t;


typedef struct token{
    char *data;
    token_type_t type;
}token_t;


#endif