
#ifndef LEXER_H
#define LEXER_H
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
    escape_sequence2x,
    escape_sequence2,
    escape_sequence3u,
    escape_sequence3,
    escape_sequence3_end,
    multiline_string_check,
    multiline_string,
    multiline_string_end,
    divide,
    comment,
    equal, 
    not_equal,
    relational_operator,
    string_type_u,
    string_type_8,
    question_mark,
    int_type,
    int_type_3,
    float_type,
    float_type_6,
    import,
    import_m,
    import_p,
    import_o,
    import_r,
    import_end,
    built_in_function_f,
    built_in_function_j,
    built_in_function_dot,
    built_in_function_dot1,
    built_in_function,
    built_in_function_end
} lexer_state_t;


token_t *get_token();


#endif