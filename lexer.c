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
            case start:
                if (current_char == '_' || isalpha(current_char)){
                    state = identifier;
                    append_to_str_buffer(buffer, current_char);
                }
                break;

            case identifier:
                if (current_char == '_' || isalpha(current_char) || isdigit(current_char)){
                    append_to_str_buffer(buffer, current_char);
                }
                else{
                    return create_token(identifier_token, buffer->string);
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