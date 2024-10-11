#include <stdio.h>
#include <string.h>

#include "ast2.h"

/* *******************************************
DONE
    generate_code
    generate_code_for_line
    generate_variable_declaration
    generate_function_call_assignment
    generate_expression_assignment

TODO:
    generate_initial_values()
    generate_predefined_funcs()
    generate_function_definition()
    generate_if_statement
    generate_while_loop
    generate_function_definition
    generate_assignment_or_expression
    generate_expression
    generate_function_return
    
******************************************** */

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
    printf("DEFVAR GF@__condition_bool\n"); // condition_bool for if statements
    // printf("DEFVAR GF@%%bool1\n");

    // variables for type check and conversion
    printf("DEFVAR GF@__type_conver_var1\n");
    printf("DEFVAR GF@__type_conver_var2\n");

    printf("DEFVAR GF@__type_conver_type1\n");
    printf("DEFVAR GF@__type_conver_type2\n");

    printf("DEFVAR GF@__type_conver_res\n");


    // printf("DEFVAR GF@%%bvar1\n");

    // printf("# - FUNCTION GLOBAL VARIABLES - #\n");
    printf("DEFVAR GF@%%__return\n");
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


// TODO: use ast->active and next_node()
void generate_code(AST *ast) {
    ASTNode *line_node = ast->currentLine;
    
    generate_initial_values();

    while (line_node->newLine != NULL) { // until there are no new lines 
        generate_code_for_line(line_node, ast); // when we go through the line
        line_node = line_node->newLine; // go to the left (to the next line)
    }
}


// TODO: use ast->active and next_node()
void generate_code_for_line(ASTNode *line_node, AST *ast) {
    ASTNode *token_node = line_node; // we are at the first token

    token_t *first_token = token_node->token;

    // skip '}'
    if (strcmp(first_token->data, "}") == 0) {
        token_node = next_node(ast);
        first_token = token_node->token;
    }

    if (strcmp(first_token->data, "var") == 0 || strcmp(first_token->data, "const") == 0) {
        generate_variable_declaration(token_node, ast);
    } else if (strcmp(first_token->data, "if") == 0) {
        generate_if_statement(token_node, ast);
    } else if (strcmp(first_token->data, "while") == 0) {
        generate_while_loop(token_node, ast);
    } else if (strcmp(first_token->data, "pub") == 0){
        generate_function_definition(token_node, ast); // TODO
    } else if (strcmp(first_token->data, "return") == 0) { // TODO
        generate_function_return(token_node, ast);
    } else {
        generate_assignment_or_expression(token_node, ast);
    }
}


void generate_expression(ASTNode *token_node, AST *ast){
    char *current_token_data = token_node->token->data;
    int current_token_type;

    static int bi_operations_counter = 0;

    while (strcmp(current_token_data, ";") != 0 && strcmp(current_token_data,"{") != 0 && strcmp(current_token_data,"|") != 0){
        current_token_data = token_node->token->data;
        current_token_type = token_node->token->type;

        if (current_token_type == binary_operator_token) {
            
            // Pops last 2 operands from stack and check their types
            printf("POPS GF@__type_conver_var1\n");
            printf("POPS GF@__type_conver_var2\n");
            printf("TYPE GF@__type_conver_type1 GF@__type_conver_var1\n");
            printf("TYPE GF@__type_conver_type2 GF@__type_conver_var2\n");
            // Compares the types
            printf("EQ GF@__type_conver_res GF@__type_conver_type1 GF@__type_conver_type2\n");
            printf("JUMPIFEQ convert_push_back%d GF@__type_conver_res bool@true\n", bi_operations_counter); // if same type, no conversion needed
            
            printf("JUMPIFEQ convert_second%d GF@__type_conver_type1 string@float\n", bi_operations_counter); // if this is true, 1. operand is float, 2. is int
            
            // Here convert 1.
            printf("PUSHS GF@__type_conver_var2\n");
            printf("PUSHS GF@__type_conver_var1\n");
            printf("INT2FLOATS\n");

            printf("JUMP convert_end%d\n", bi_operations_counter);

            // Here convert 2.
            printf("LABEL convert_second%d\n", bi_operations_counter);
            printf("PUSHS GF@__type_conver_var2\n");
            printf("INT2FLOATS\n");
            printf("PUSHS GF@__type_conver_var1\n");

            printf("JUMP convert_end%d\n", bi_operations_counter);

            // If same operands, just pushes them back
            printf("LABEL convert_push_back%d\n", bi_operations_counter);
            printf("PUSHS GF@__type_conver_var2\n");
            printf("PUSHS GF@__type_conver_var1\n");

            printf("LABEL convert_end%d\n", bi_operations_counter);
        }

        // TODO: Doesnt handle relation operators as they can have other types than int and float
        // if (current_token_type != identifier_token) {            
        // }

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
        else{ // variables
            printf("PUSHS LF@%s", token_node->token->data);                         // Mosime pretypovat!!!
        }
        token_node = token_node->next; // next token
    }
}

// TODO: Fix nested if statements
void generate_if_statement(ASTNode *token_node, AST *ast) {
    // Skip 'if'
    token_node = token_node->next->next; // skip '(' and go to expression 'x'
    static int if_label_counter = 0;
    int current_if_label = if_label_counter++;
    // example line
    // if (x < y || y > z || z == y && y == 0){
    // if, (, x y < y z > || z y == y 0 == && ||, ), {
    generate_expression(token_node, ast);

    printf("POPS GF@__condition_bool"); // store the condition result into global boolean variable
 
    printf("JUMPIFEQ if_then%d GF@__condition_bool bool@true\n", current_if_label);
    printf("JUMP if_else%i\n", current_if_label);
    printf("LABEL if_then%d\n", current_if_label);

    // Then branch
    while(strcmp(token_node->token->data, '}') != 0){
        generate_code_for_line(token_node, ast);
    }
    
    printf("JUMP if_end%d\n", current_if_label);


    printf("LABEL if_else%d\n", current_if_label);

    // Else branch
    while(strcmp(token_node->token->data, '}') != 0){
        generate_code_for_line(token_node, ast);
    }

    // Move token_node to 'else' block and generate code
    printf("LABEL if_end%d\n", current_if_label);
}

// TODO: FIX nested while loops
void generate_while_loop(ASTNode *token_node, AST *ast){
    // Skip 'while'
    token_node = next_node(ast);
    // Skip '('
    token_node = next_node(ast);

    static int while_label_counter = 0;
    int current_while_label = while_label_counter++;

    printf("LABEL while_start%d\n", current_while_label);

    generate_expression(token_node, ast);

    printf("POPS GF@__condition_bool");

    // if condition is false jump to loop body
    printf("JUMPIFEQ while_end%d GF@__condition_bool bool@false\n", current_while_label);

    // Skip '{' token to start generating the loop body
    // TODO: |idk| while extention
    token_node = next_node(ast);

    // Loop body until we reach '}'
    while(strcmp(token_node->token->data, '}') != 0){
        generate_code_for_line(token_node, ast);
    }

    printf("JUMP while_start%d\n", current_while_label);
    printf("LABEL while_end%d\n", current_while_label);
}

// IDK: do we need 'type: int32' in our AST? Is it needed in code gen or just syntax analysis?
// var x = 5;
// const y = foo();
void generate_variable_declaration(ASTNode *token_node, AST *ast) {
    // Skip 'var' or 'const
    token_node = next_node(ast);

    char *var_name = token_node->token->data;
    printf("DEFVAR LF@%s\n", var_name); 

    token_node = token_node->next; // go to the next token
    if (token_node != NULL && strcmp(token_node->token->data, "=") == 0) { // var var_name =;
        // Variable initialization
        token_node = token_node->next;
        if (token_node->token->type == identifier_token) { // var var_name = <function_call>;
            generate_function_call(token_node);
        } else{
            generate_expression(token_node, var_name); // var var_name = <expression>;
        }
    // } else {
    //     // Uninitialized variable IDK??
    //     printf("MOVE LF@%s nil@nil\n", var_name); // var ..;
    // }
    }
}

// x = a + b * c;
// x = someFunction(a, b);
// someFunction(a, b);
void generate_assignment_or_expression(ASTNode *token_node, AST *ast){
    if(token_node->token->type == identifier_token){
        char *identifier = token_node->token->data; // variable or funciton name

        token_node = next_node(ast); // next token
        
        if(strcmp(token_node->token->data, "=") == 0){
            // Variable assignment
            token_node = next_node(ast); // moving past '=' to R value

            if(token_node->token->type == identifier_token){
                // Possible function call
                ASTNode *function_call_node = token_node;
                ASTNode *after_function_node = function_call_node->next; // checking whats function name
                if(strcmp(after_function_node, "(") == 0){
                    // Its a function call
                    generate_function_call_assignment(identifier, function_call_node);
                } else{
                    // Its not a function call, it has to be an expression
                    generate_expression_assignment(identifier, token_node, ast);
                }
            } else{
                // R value is an expression
                generate_expression_assignment(identifier, token_node, ast);
            }
        } else if(strcmp(token_node->token->data, "(") == 0){
            // Its a function call as a statement
            generate_function_call(token_node);
        }
    }
}

void generate_expression_assignment(char *identifier, ASTNode *token_node, AST *ast){
    // Generate expression code
    generate_expression(token_node, ast);

    // Pop the result into variable
    printf("POPS LF@%s\n", identifier);
}

void generate_function_call_assignment(char *identifier, ASTNode *function_call_node, AST *ast){
    // Generate function call code
    generate_function_call(function_call_node, ast);

    // We stored the function return value into global variable '__return'
    // Move the return value into the identifier
    printf("MOVE LF@%s GF@__return\n", identifier);
}

void generate_function_call(ASTNode *token_node, AST *ast){

    // Store the function name from the function node token
    char *function_name_label = token_node->token->data;

    // Skip '(' (hoping that '(' is the next token - ehm SYNTAX ANALYSIS)
    token_node = next_node(ast);

    // Move to first argument, or ')'
    token_node = next_node(ast);

    printf("CREATEFRAME\n");

    int arg_count = 0;
    while (strcmp(token_node->token->data, ")") != 0){
        // generate_expression(token_node, ast);
        printf("DEFVAR TF@arg%d\n", arg_count);

        if (token_node->token->data == identifier_token){
            // If argument is a variable
            printf("MOVE TF@arg%d LF@%s\n", arg_count, token_node->token->data);
        }
        else if (token_node->token->data == int_token){
            // If argument is an int literal
            printf("MOVE TF@arg%d int@%d\n", arg_count, token_node->token->data);
        }
        else if (token_node->token->data == float_token){
            // If argument is an float literal
            printf("MOVE TF@arg%d int@%f\n", arg_count, token_node->token->data);
        }
        else if (token_node->token->data == string_token){
            // If argument is an string literal
            printf("MOVE TF@arg%d int@%s\n", arg_count, escape_string(token_node->token->data));
        }

        // Move to the next token
        token_node = next_node(ast);

        if(strcmp(token_node->token->data, ",") == 0){
            token_node = next_node(ast);
        }

        arg_count++;
    }


    printf("PUSHFRAME\n");

    printf("CALL %s\n", function_name_label);

    // printf("POPFRAME\n"); - use it when we handle function return
}

// Converts escape sequences to \xyz format
char *escape_string(const char *input) {

    size_t input_len = strlen(input);
    // Allocate enough space: worst case every character is escaped as \xyz
    size_t max_len = (input_len * 4) + 1;
    char *escaped = malloc(sizeof(char)*max_len);
    if (!escaped) {
        fprintf(stderr, "Memory allocation failed in escape_string\n");
        exit(1000);                         // random exit number, mad?
    }

    size_t j = 0; // Index for escaped string

    for (size_t i = 0; i < input_len; i++) {
        unsigned char c = input[i];
        // Check if character needs to be escaped
        if (c <= 32 || c == 35 || c == 92) {
            escaped[j++] = '\\';
            escaped[j++] = '0';
            escaped[j++] = '0' + (c / 10) % 10;
            escaped[j++] = '0' + c % 10;
        } else {
            escaped[j++] = c;
        }
    }

    escaped[j] = '\0'; // Null-terminate the string
    return escaped;
}