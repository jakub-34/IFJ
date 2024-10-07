#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lexer.h"
#include "str_buffer.h"
#include "token.h"
#include "keyword_check.h"


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
    bool multiline = false;
    while(true){
        char current_char = getchar();
        //printf("Current char: %c\n", current_char);
        //printf("Current char: %i\n", current_char);
        //printf("Current state: %i\n", state);

        switch(state){
            //starting point
            case start:
                if (current_char == EOF){
                    return create_token(eof_token, NULL);
                }
                
                if (current_char == '_' || isalpha(current_char)){
                    state = identifier;
                    append_to_str_buffer(buffer, current_char);
                }
                else if (current_char == '['){
                    state = string_type;
                    append_to_str_buffer(buffer, current_char);
                }
                else if (current_char == '?'){
                    state = question_mark;
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
                else if (current_char == '\\'){
                    state = multiline_string_check;
                    append_to_str_buffer(buffer, current_char);
                }
                else if (current_char == '/'){
                    state = divide;
                }
                else if (current_char == ';'){
                    append_to_str_buffer(buffer, current_char);
                    return create_token(semicolon_token, buffer->string);
                }
                else if (current_char == '{' || current_char == '}' || current_char == '(' || current_char == ')'){
                    append_to_str_buffer(buffer, current_char);
                    return create_token(bracket_token, buffer->string);
                }
                else if (current_char == ':'){
                    append_to_str_buffer(buffer, current_char);
                    return create_token(colon_token, buffer->string);
                }
                else if (current_char == '|' || current_char == '.' || current_char == ','){
                    append_to_str_buffer(buffer, current_char);
                    return create_token(punctuation_token, buffer->string);
                }
                else if (current_char == '+' || current_char == '-' || current_char == '*'){
                    append_to_str_buffer(buffer, current_char);
                    return create_token(binary_operator_token, buffer->string);
                }
                else if (current_char == '='){
                    state = equal;
                    append_to_str_buffer(buffer, current_char);
                }
                else if (current_char == '!'){
                    state = not_equal;
                    append_to_str_buffer(buffer, current_char);
                }
                else if (current_char == '@'){
                    state = import;
                    append_to_str_buffer(buffer, current_char);
                }
                else if (current_char == '<' || current_char == '>'){
                    state = relational_operator;
                    append_to_str_buffer(buffer, current_char);
                }
                else if (current_char == 9 || current_char == 32 || current_char == '\n' || current_char == '\r'){
                    state = start;
                }
                else if (current_char == -1){
                    return NULL;
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            //identifier/keyword/underscore
            case identifier:
                if (current_char == '_' || isalpha(current_char) || isdigit(current_char)){
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    ungetc(current_char, stdin);
                    return create_token(kw_check(buffer->string), buffer->string);
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
                    ungetc(current_char, stdin);
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
                    ungetc(current_char, stdin);
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
                    ungetc(current_char, stdin);
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
                    ungetc(current_char, stdin);
                    return create_token(float_token, buffer->string);
                }
                break;

            //handling string
            case string:
                if (current_char == '"'){
                    append_to_str_buffer(buffer, current_char);
                    return create_token(string_token, buffer->string);
                }
                else if (current_char == '\\'){
                    state = escape_sequence;
                    //vytvorime novy buffer na escpae sequencie aby sme ich vedeli previest ked skoncia
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
                if ((current_char == '\'' || current_char == '"' || current_char == 'n' || current_char == 'r' || current_char == 't' || current_char == '\\') && (multiline == false)){
                    state = string;
                    //koniec sekvencie, treba previest a appendnut do bufferu
                }
                if ((current_char == '\'' || current_char == '"' || current_char == 'n' || current_char == 'r' || current_char == 't' || current_char == '\\') && (multiline == true)){
                    multiline = false;
                    state = multiline_string;
                    //koniec sekvencie, treba previest a appendnut do bufferu
                }
                else if (current_char == 'x'){
                    state = escape_sequence2x;
                    //sequence buffer
                }
                else if (current_char == 'u'){
                    state = escape_sequence3u;
                    //sequence buffer
                }
                else {
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case escape_sequence2x:
                if (isdigit(current_char) || (current_char >= 'a' && current_char <= 'f') || (current_char >= 'A' && current_char <= 'F')){
                    state = escape_sequence2;
                    //sequence buffer
                }
                else {
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case escape_sequence2:
                if ((isdigit(current_char) || (current_char >= 'a' && current_char <= 'f') || (current_char >= 'A' && current_char <= 'F')) && (multiline == true)){
                    multiline = false;
                    state = multiline_string;
                    //sequence buffer 
                    //preved sekvenciu
                }
                else if ((isdigit(current_char) || (current_char >= 'a' && current_char <= 'f') || (current_char >= 'A' && current_char <= 'F')) && (multiline == false)){
                    state = escape_sequence2_end;
                    //sequence buffer 
                    //preved sekvenciu
                }
                else {
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case escape_sequence2_end:
                if (current_char == '"'){
                    append_to_str_buffer(buffer, current_char);
                    return create_token(string_token, buffer->string);
                }
                else if (current_char == '\\'){
                    state = escape_sequence;
                    //sequence buffer
                }
                else if(current_char > 31){
                    state = string;
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case escape_sequence3u:
                if (current_char == '{'){
                    state = escape_sequence3;
                    //sequence buffer
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;
            
            case escape_sequence3:
                if (isdigit(current_char) || (current_char >= 'a' && current_char <= 'f') || (current_char >= 'A' && current_char <= 'F')){
                    //sequence buffer
                }
                else if ((current_char == '}') && (multiline == false)){
                    state = escape_sequence3_end;
                    //sequence buffer
                    //preved sekvenciu
                }
                else if ((current_char == '}') && (multiline == true)){
                    multiline = false;
                    state = multiline_string;
                    //sequence buffer
                    //preved sekvenciu
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case escape_sequence3_end:
                 if (current_char == '"'){
                    append_to_str_buffer(buffer, current_char);
                    return create_token(string_token, buffer->string);
                }
                else if (current_char == '\\'){
                    state = escape_sequence;
                    //sequence buffer
                }
                else if(current_char > 31){
                    state = string;
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case multiline_string_check:
                if (current_char == '\\'){
                    state = multiline_string;
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case multiline_string:
                if (current_char == '\n'){
                    state = multiline_string_end;
                }
                else if (current_char == '\\'){
                    multiline = true;
                    state = escape_sequence;
                    //sequence buffer
                }
                else if (current_char > 31){
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case multiline_string_end:
                if (current_char == '\\'){
                    state = multiline_string_check;
                    append_to_str_buffer(buffer, '\n');
                }
                else{
                    ungetc(current_char, stdin);
                    return create_token(string_token, buffer->string);
                }
                break;

            //others
            case divide:
                if (current_char == '/'){
                    state = comment;
                }
                else{
                    ungetc(current_char, stdin);
                    append_to_str_buffer(buffer, '/');
                    return create_token(binary_operator_token, buffer->string);
                }
                break;

            case comment:
                if (current_char == '\n'){ //alebo eof
                    state = start;
                }
                break;

            case equal:
                if (current_char == '='){
                    append_to_str_buffer(buffer, current_char);
                    return create_token(double_equal_token, buffer->string);
                }
                else{
                    ungetc(current_char, stdin);
                    return create_token(equal_token, buffer->string);
                }
                break;

            case not_equal:
                if (current_char == '='){
                    append_to_str_buffer(buffer, current_char);
                    return create_token(not_equal_token, buffer->string);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case relational_operator:
                if (current_char == '='){
                    append_to_str_buffer(buffer, current_char);
                    return create_token(relational_operator_token, buffer->string);
                }
                else{
                    ungetc(current_char, stdin);
                    return create_token(relational_operator_token, buffer->string);
                }
                break;

            //handling types
            case string_type:
                if (current_char == ']'){
                    state = string_type_u;
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case string_type_u:
                if (current_char == 'u'){
                    state = string_type_8;
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case string_type_8:
                if (current_char == '8'){
                    append_to_str_buffer(buffer, current_char);
                    return create_token(type_token, buffer->string);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case question_mark:
                if (current_char == '['){
                    state = string_type;
                    append_to_str_buffer(buffer, current_char);
                }
                else if (current_char == 'i'){
                    state = int_type;
                    append_to_str_buffer(buffer, current_char);
                }
                else if (current_char == 'f'){
                    state = float_type;
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case int_type:
                if (current_char == '3'){
                    state = int_type_3;
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;
            
            case int_type_3:
                if (current_char == '2'){
                    append_to_str_buffer(buffer, current_char);
                    return create_token(type_token, buffer->string);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case float_type:
                if (current_char == '6'){
                    state = float_type_6;
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;
            
            case float_type_6:
                if (current_char == '4'){
                    append_to_str_buffer(buffer, current_char);
                    return create_token(type_token, buffer->string);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            //handling import 
            case import:
                if (current_char == 'i'){
                    state = import_m;
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case import_m:
                if (current_char == 'm'){
                    state = import_p;
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case import_p:
                if (current_char == 'p'){
                    state = import_o;
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case import_o:
                if (current_char == 'o'){
                    state = import_r;
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case import_r:
                if (current_char == 'r'){
                    state = import_end;
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;

            case import_end:
                if (current_char == 't'){
                    append_to_str_buffer(buffer, current_char);
                    return create_token(import_token, buffer->string);
                }
                else{
                    fprintf(stderr, "lexical error\n");
                    exit(1);
                }
                break;
            
        }
    }
}

int main(){
    token_t* token;
    while ((token = get_token()) != NULL) {
        printf("Token: %s\n", token->data);
        printf("Type: %d\n", token->type);
        
        free(token->data);  
        free(token);        
    }
    return 0;
}