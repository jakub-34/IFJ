#ifndef LEXER_H
#define LEXER_H
#include <stdio.h>
#include "token.h"

typedef enum lexer_state {
    start,
    identifier,
    string_type,
    zero,
    integer,
    decimal_number_check,
    decimal_number,
    exponent_number_check,
    signed_exponent_number,
    exponent_number,
    string,
    escape_sequence,
    escape_sequence2,
    escape_sequence3
} lexer_state_t;


token_t *get_token();


#endif
