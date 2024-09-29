#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lexer.h"
#include "str_buffer.h"
#include "token.h"


token_t* create_token(token_type_t type, char* data) {
    token_t* token = (token_t*)malloc(sizeof(token_t));
    if (token == NULL){
        fprintf(stderr, "Failed to alloc space for token.\n");
        exit(99);
    }
    token->type = type;
    token->data = data;
    return token;
}


token_t* get_token(){
    str_buffer_t* buffer = create_str_buffer();
    lexer_state_t state = start;
    while(true){
        char current_char = getchar();
        printf("Current char: %c\n", current_char);

        switch(state){
            //starting point
            case start:
                if (current_char == '_' || isalpha(current_char)){
                    state = identifier;
                    append_to_str_buffer(buffer, current_char);
                }
                else if (current_char == '['){
                    state = string_type;
                    append_to_str_buffer(buffer, current_char);
                }
                else if (current_char == '0'){
                    state = zero;
                    append_to_str_buffer(buffer, current_char);
                }
                else if (isdigit(current_char)){
                    state = integer;
                    append_to_str_buffer(buffer, current_char);
                }
                else if (current_char == '"'){
                    state = string;
                    append_to_str_buffer(buffer, current_char);
                }
                break;

            //identifier/keyword/underscore
            case identifier:
                if (current_char == '_' || isalpha(current_char) || isdigit(current_char)){
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    return create_token(identifier_token, buffer->string);
                }
                break;

            //handling numbers
            case zero:
                if (current_char == '0'){
                    append_to_str_buffer(buffer, current_char);
                }
                else if (isdigit(current_char)){
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                else if (current_char == 'e' || current_char == 'E'){
                    state = exponent_number_check;
                    append_to_str_buffer(buffer, current_char);
                }
                else if (current_char == '.'){
                    state = decimal_number_check;
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    return create_token(int_token, buffer->string);
                }
                break;

            case integer:
                if (isdigit(current_char)){
                    append_to_str_buffer(buffer, current_char);
                }
                else if (current_char == 'e' || current_char == 'E'){
                    state = exponent_number_check;
                    append_to_str_buffer(buffer, current_char);
                }
                else if (current_char == '.'){
                    state = decimal_number_check;
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    return create_token(int_token, buffer->string);
                }
                break;

            case decimal_number_check:
                if (isdigit(current_char)){
                    state = decimal_number;
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case decimal_number:
                if (isdigit(current_char)){
                    append_to_str_buffer(buffer, current_char);
                }
                else if (current_char == 'e' || current_char == 'E'){
                    state = exponent_number_check;
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    return create_token(float_token, buffer->string);
                }
                break;

            case exponent_number_check:
                if (current_char == '+' || current_char == '-'){
                    state = signed_exponent_number;
                    append_to_str_buffer(buffer, current_char);
                }
                else if (isdigit(current_char)){
                    state = exponent_number;
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case signed_exponent_number:
                if (isdigit(current_char)){
                    state = exponent_number;
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case exponent_number:
                if (isdigit(current_char)){
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    return create_token(float_token, buffer->string);
                }
                break;

            //handling string
            case string:
                if (current_char == '"'){
                    return create_token(string_token, buffer->string);
                }
                else if (current_char == '\\'){
                    state = escape_sequence;
                    //vytvorime novy buffer na escpae sequencie aby sme ich vedeli previest ked skoncia?
                }
                else if (current_char < 32 || current_char == '\n'){
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                else {
                    append_to_str_buffer(buffer, current_char);
                }
                break;

            case escape_sequence:
                if (current_char == '\'' || current_char == '"' || current_char == 'n' || current_char == 'r' || current_char == 't' || current_char == '\\'){
                    state = string;
                    //koniec sekvencie, treba previest a appendnut do bufferu
                }
                else if (current_char == 'x'){
                    state = escape_sequence2;
                    //sequence buffer
                }
                else if (current_char == 'u'){
                    state = escape_sequence3;
                    //sequence buffer
                }
                break;


            case string_type:
                if (current_char == ']'){
                    //mozem to spravit cez if-i? 
                    //alebo nejako inak inateligentnejsie lebo mi pride hlupe robit kvoli tomuto 5 stavov
                }
                break;
        }
    }
}

int main(){
    token_t* token = get_token();
    printf("Token: %s\n", token->data);
    return 0;
}