#include <stdio.h>
#include "ast2.h"
#include <string.h>

void generate_initial_values(){
    // TODO: generate header
    // 
    // printf("# - HEADER - #\n");
    // printf(".IFJcode22\n");
    // printf("CREATEFRAME\n");
    // printf("PUSHFRAME\n");

    // printf("# - ARITHMETIC GLOBAL VARIABLES - #\n");
    // printf("DEFVAR GF@%%tmp0\n");
    // printf("DEFVAR GF@%%tmp1\n");
    // printf("DEFVAR GF@%%tmp2\n");

    // printf("# - BOOL GLOBAL VARIABLES - #\n");
    printf("DEFVAR GF@%%condition_bool\n"); // condition_bool for if statements
    // printf("DEFVAR GF@%%bool1\n");
    // printf("DEFVAR GF@%%bvar0\n");
    // printf("DEFVAR GF@%%bvar1\n");

    // printf("# - FUNCTION GLOBAL VARIABLES - #\n");
    // printf("DEFVAR GF@%%freturn\n");
    // printf("DEFVAR GF@%%fparam0\n");
    // printf("DEFVAR GF@%%fparam1\n");
    // printf("DEFVAR GF@%%fparam2\n");
    // printf("DEFVAR GF@%%fparam3\n");
    // printf("DEFVAR GF@%%fparam4\n");
    // printf("# =============================== #\n");
    
    // print langauge defined functions
    generate_predefined_funcs();
    // func_substring();
    // func_intval();
    // func_floatval();
}

void generate_code(AST *ast) {
    ASTNode *line_node = ast->currentLine;
    
    generate_initial_values();

    while (line_node->newLine != NULL) { // until there are no new lines 
        generate_code_for_line(line_node); // when we go through the line
        line_node = line_node->newLine; // go to the left (to the next line)
    }
}



void generate_code_for_line(ASTNode *line_node) {
    ASTNode *token_node = line_node; // we are at the first token

    token_t *first_token = token_node->token;

    if (strcmp(first_token->data, "var") == 0 || strcmp(first_token->data, "const") == 0) {
        generate_variable_declaration(token_node);
    } else if (strcmp(first_token->data, "if") == 0) {
        generate_if_statement(token_node);
    } else if (strcmp(first_token->data, "while") == 0) {
        generate_while_loop(token_node);
    } else if (strcmp(first_token->data, "pub") == 0){
        generate_function_definition(token_node); // TODO
    } else {
        generate_assignment_or_expression(token_node);
    }
}


// IDK: do we need 'type: int32' in our AST? Is it needed in code gen or just syntax analysis?

void generate_variable_declaration(ASTNode *token_node) {
    // Sk'ip 'var' or 'const
    token_node = token_node->next;

    char *var_name = token_node->token->data;
    printf("DEFVAR LF@%s\n", var_name); 

    token_node = token_node->next; // go to the next token
    if (token_node != NULL && strcmp(token_node->token->data, "=") == 0) { // var var_name =;
        // Variable initialization
        token_node = token_node->next;
        if (token_node->token->type == identifier_token) { // var var_name = <function_call>;
            generate_function_call(token_node);
        } else{
            generate_expression_rpn(token_node, var_name); // var var_name = <expression>;
        }
    // } else {
    //     // Uninitialized variable IDK??
    //     printf("MOVE LF@%s nil@nil\n", var_name); // var ..;
    // }
    }
}

void generate_expression(ASTNode *token_node){
    char *current_token_data;
    int current_token_type;

    while (strcmp(current_token_data, ";") != 0 && strcmp(current_token_data,"{") != 0 && strcmp(current_token_data,"|") != 0){
        current_token_data = token_node->token->data;
        current_token_type = token_node->token->type;

        if(strcmp(current_token_data, "<") == 0){
            printf("LTS\n");
        }
        else if(strcmp(current_token_data, ">") == 0){
            printf("GTS\n");
        }
        else if(strcmp(current_token_data, "<=") == 0){
            printf("GTS\n");
            printf("NOTS\n");
        }
        else if(strcmp(current_token_data, ">=") == 0){
            printf("LTS\n");
            printf("NOTS\n");
        }
        else if(strcmp(current_token_data, "!=") == 0){
            printf("EQS\n");
            printf("NOTS\n");
        }
        else if(strcmp(current_token_data, "==") == 0){
            printf("EQS\n");
        }
        else if(strcmp(current_token_data, "+") == 0){
            printf("ADDS\n");
        }
        else if(strcmp(current_token_data, "-") == 0){
            printf("SUBS\n");
        }
        else if(strcmp(current_token_data, "*") == 0){
            printf("MULS\n");
        }
        else if(strcmp(current_token_data, "/") == 0){
            printf("DIVS\n");
        }
        else{
            printf("PUSHS LF@%s", token_node->token->data);                         // Mosime pretypovat!!!
        }
        token_node = token_node->next; // next token
    }
}

void generate_if_statement(ASTNode *token_node) {
    // Skip 'if'
    token_node = token_node->next->next; // skip '(' and go to expression 'x'
    static int label_counter = 0;
    
    // example line
    // if (x < y || y > z || z == y && y == 0){
    // if, (, x y < y z > || z y == y 0 == && ||, ), {
    generate_expression(token_node);

    printf("POPS GF@cond_result"); // store the condition result into global boolean variable
 
    printf("JUMPIFEQ if_then%d GF@cond_result bool@true\n", label_counter);
    printf("JUMP if_else%i\n", label_counter);
    printf("LABEL if_then%d\n", label_counter);

    // Then branch
    generate_code_for_line();
    
    printf("JUMP if_end%d\n", label_counter);
    // generate_code_for_block(token_node);

    printf("LABEL if_else%d\n", label_counter);

    // Else branch
    generate_code_for_line();

    // Move token_node to 'else' block and generate code
    printf("LABEL if_end%d\n", label_counter);
}

void generate_expression_rpn(ASTNode *token_node, char *result_var) {
    // Convert the tokens starting from token_node to RPN
    TokenList *rpn_tokens = get_rpn_tokens(token_node);

    // Generate code from RPN tokens
    generate_code_from_rpn(rpn_tokens);

    // If there's a result variable, pop the result into it
    if (result_var != NULL) {
        printf("POPS LF@%s\n", result_var);
    } else {
        // If no result variable, the result remains on the stack
    }
}


// void generate_code_for_block(ASTNode **token_node) {
//     // Process statements inside the block until we reach '}'
//     while (*token_node != NULL && strcmp((*token_node)->token->data, "}") != 0) {
//         generate_code_for_line(*token_node);
//         *token_node = (*token_node)->next;
//     }
//     // Skip the closing '}'
//     if (*token_node != NULL && strcmp((*token_node)->token->data, "}") == 0) {
//         *token_node = (*token_node)->next;
//     } else {
//         fprintf(stderr, "Syntax error: Expected '}' at the end of block.\n");
//         exit(2);
//     }
// }

// EXAMPLE
if (x == 5){
    return true;
} else {
    return false;
}

jmpeq x 5




PUSHS LF@x       # Push the value of x onto the stack
PUSHS int@5      # Push the value 5 onto the stack
EQS              # Compare if x == 5; result (bool@true/false) is on the stack


case ==:
 EQS              # Compare if x == 5; result (bool@true/false) is on the stack
case >=:
 LEQS

POPS GF@cond_result          # Pop the result into GF@cond_result
JUMPIFEQ if_then_0 GF@cond_resulte bool@true  # If condition is true, jump to if_then_0
JUMP if_else_0               # Else, jump to if_else_0


