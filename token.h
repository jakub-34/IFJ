#ifndef TOKEN_H
#define TOKEN_H


typedef enum token_type {
    identifier_token,
    int_token,
    float_token,
    string_token,
    keyword_token,
    eof_token,
    binary_operator_token,
    semicolon_token,
    bracket_token,
    colon_token,
    punctuation_token,
    equal_token,
    double_equal_token,
    not_equal_token,
    relational_operator_token,
    type_token,
    null_token,
    import_token
} token_type_t;


typedef struct token{
    char *data;
    token_type_t type;
}token_t;


#endif