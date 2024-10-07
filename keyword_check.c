#include <string.h>
#include "token.h"
#include "keyword_check.h"

enum token_type kw_check(char *data){
    if((strcmp(data, "i32") == 0) || (strcmp(data, "f64") == 0)){
        return type_token;
    }

    if(strcmp(data, "null") == 0){
        return null_token;
    }

    if((strcmp(data, "const") == 0) || (strcmp(data, "else") == 0) || (strcmp(data, "fn") == 0) || (strcmp(data, "if") == 0) || (strcmp(data, "pub") == 0) || (strcmp(data, "return") == 0) || (strcmp(data, "var") == 0) || (strcmp(data, "void") == 0) || (strcmp(data, "while") == 0)){
        return keyword_token;
    }

    return identifier_token;
}