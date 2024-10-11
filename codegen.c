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
    generate_function_definition
    generate_if_statement
    generate_while_loop
    generate_assignment_or_expression
    generate_expression
    generate_function_return

TODO:
    generate header + create+push frame for main??
    generate_initial_values()
    generate_builtin_funcs()
    
    ifj.readstr

    Celociselne/desetinne deleni u generate_expression

MIGHT NEED CHANGING: Issues with global variables

******************************************** */

void generate_initial_values(){
    // TODO: generate header
    // 
    // printf("# - HEADER - #\n");
    // printf(".IFJcode22\n");

    printf(".IFJcode24\n");             // Prolog

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
    // printf("DEFVAR GF@__return\n");
    
    // printf("DEFVAR GF@%%fparam0\n");
    // printf("DEFVAR GF@%%fparam1\n");
    // printf("DEFVAR GF@%%fparam2\n");
    // printf("DEFVAR GF@%%fparam3\n");
    // printf("DEFVAR GF@%%fparam4\n");
    // printf("# =============================== #\n");
    
    // print langauge built-in functions
    generate_builtin_functions();
}


// TODO: use ast->active? and next_node()
void generate_code(AST *ast) {
    ASTNode *line_node = ast->currentLine;
    
    generate_initial_values();

    while (line_node->newLine != NULL) { // until there are no new lines 
        generate_code_for_line(line_node, ast); // when we go through the line
        line_node = line_node->newLine; // go to the left (to the next line)
    }
}


// TODO: use ast->active? and next_node()
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
    while(strcmp(token_node->token->data, "}") != 0){
        generate_code_for_line(token_node, ast);
    }
    
    printf("JUMP if_end%d\n", current_if_label);


    printf("LABEL if_else%d\n", current_if_label);

    // Else branch
    while(strcmp(token_node->token->data, "}") != 0){
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
    while(strcmp(token_node->token->data, "}") != 0){
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
            generate_function_call(token_node, ast);
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
                    generate_function_call_assignment(identifier, function_call_node, ast);
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
            generate_function_call(token_node, ast);
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

    // Pop return value from data stack into the identifier
    printf("POPS LF@%s\n", identifier);
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
        printf("DEFVAR TF@__arg%d\n", arg_count);

        if (token_node->token->data == identifier_token){
            // If argument is a variable
            printf("MOVE TF@__arg%d LF@%s\n", arg_count, token_node->token->data);
        }
        else if (token_node->token->data == int_token){
            // If argument is an int literal
            printf("MOVE TF@__arg%d int@%d\n", arg_count, token_node->token->data);
        }
        else if (token_node->token->data == float_token){
            // If argument is an float literal
            printf("MOVE TF@__arg%d int@%f\n", arg_count, token_node->token->data);
        }
        else if (token_node->token->data == string_token){
            // If argument is an string literal
            printf("MOVE TF@__arg%d int@%s\n", arg_count, escape_string(token_node->token->data));
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

// pub fn ID (parametry) <return TYPE> {
// pub fn add(a : i32, b : i32) i32{

void generate_function_definition(ASTNode *token_node, AST *ast) {
    // Skip 'pub'
    token_node = node_next(ast); // <- 'fn'
    // Skip 'fn'
    token_node = node_next(ast); // <- function name

    // LABEL function_name
    printf("LABEL %s\n", token_node->token->data);

    // Skip '('
    token_node = node_next(ast); // <- '('
    // Go to first parameter or ')'
    token_node = node_next(ast); // <- parameter or ')'

    // Going through parameters
    while(strcmp(token_node->token->data, ")") != 0){
        // Parameter: <id> : <type>
        char *param_name = token_node->token->data; // Store identifier

        // Skip ':'
        token_node = node_next(ast); // <- ':'
        // Move to type
        token_node = node_next(ast); // <- type
        
        printf("DEFVAR LF@%s\n", param_name);

        int param_idx = 0; // IDK: maybe should be static int?               = if broken - put in static
        printf("MOVE LF@%s LF@__arg%d\n", param_name, param_idx);
        param_idx++;

        // Move to ',' or ')'
        token_node = node_next(ast); // <- ',' or ')'

        // Skip ','
        if(strcmp(token_node->token->data, ",") == 0){
            token_node = node_next(ast); // go to next parameter
        }
    }

    // Return type
    token_node = node_next(ast); // <- return type

    // pub fn ID (parametry) <return TYPE> {
    token_node = next_node(ast); // <- '{'

    while(strcmp(token_node->token->data, "return") != 0 && strcmp(token_node->token->data, "}") != 0){
        generate_code_for_line(token_node, ast);                // maybe check this
        token_node = next_node(ast);
    }
    // return;}
    generate_function_return(token_node, ast);
}

void generate_function_return(ASTNode *token_node, AST *ast) {
    // Skip 'return'
    token_node = token_node->next;

    if (token_node == NULL || strcmp(token_node->token->data, ";") == 0) {
        // Void return
        printf("POPFRAME\n");
        printf("RETURN\n");
        return;
    }

    // Generate code for the return expression
    generate_expression(token_node, ast);
    
    // The result is on top of the stack
    // No need to do anything else, just call RETURN
    // printf("POPS GF@__return");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void generate_builtin_functions(){
// just for copying
printf("\n");

// Functions for reading / writing
    // pub fn ifj.readstr() ?[]u8
    
    // Definice lokálních proměnných
    printf("DEFVAR LF@__retval\n"); // Načtený řetězec
    printf("DEFVAR LF@__type\n");   // Typ načtené hodnoty

    // Načtení vstupu jako string
    printf("READ LF@__retval string\n");
    printf("TYPE LF@__type LF@__retval\n");

    // Kontrola, zda je vstup typu string
    printf("JUMPIFEQ ifj_readstr_end LF@__type string@string\n");

    // Pokud není typu string, nastav návratovou hodnotu na nil
    printf("MOVE LF@__retval nil@nil\n");

    // Vložení načteného řetězce na datový zásobník
    printf("LABEL ifj_readstr_end\n");

    printf("PUSHS LF@__retval\n");
    printf("POPFRAME\n");
    printf("RETURN\n\n");

    ///////////////// Function: ifj.readstr WITH HANDLING ESCAPE CHARACTERS HANDLING //////////////////////////

    // TODO: testing - does READ command read string with 

    printf("LABEL ifj.readstr\n");
    
    // Define local variables
    printf("DEFVAR LF@__retval\n");         // The read string
    printf("DEFVAR LF@__type\n");           // Type of the read value
    printf("DEFVAR LF@__len\n");            // Length of the string
    printf("DEFVAR LF@__char\n");           // Character at a position
    printf("DEFVAR LF@__new_str\n");        // String without newline
    printf("DEFVAR LF@__i\n");              // Loop counter
    printf("DEFVAR LF@__cmp_res\n");        // Comparison result
    
    // Read input as string
    printf("READ LF@__retval string\n");
    printf("TYPE LF@__type LF@__retval\n");
    
    // Check if input is of type string
    printf("JUMPIFEQ ifj_readstr_process LF@__type string@string\n");
    // If not, set return value to nil
    printf("MOVE LF@__retval nil@nil\n");
    printf("JUMP ifj_readstr_end\n");
    
    printf("LABEL ifj_readstr_process\n");
    // Get the length of the string
    printf("STRLEN LF@__len LF@__retval\n");
    
    // Check if length is zero (empty input)
    printf("JUMPIFEQ ifj_readstr_empty LF@__len int@0\n");
    
    // Get the last character index
    printf("SUB LF@__i LF@__len int@1\n");      // LF@__i = len - 1
    printf("GETCHAR LF@__char LF@__retval LF@__i\n");
    
    // Compare the last character to '\n'
    printf("EQ LF@__cmp_res LF@__char string@\\010\n");   // '\010' is octal for '\n'
    printf("JUMPIFEQ ifj_readstr_remove_newline LF@__cmp_res bool@true\n");
    // If not equal, no need to remove newline
    printf("JUMP ifj_readstr_end\n");
    
    printf("LABEL ifj_readstr_remove_newline\n");
    // Initialize new string
    printf("MOVE LF@__new_str string@\n");       // Empty string
    printf("MOVE LF@__i int@0\n");
    printf("SUB LF@__len LF@__len int@1\n");     // New length is len - 1
    
    printf("LABEL ifj_readstr_loop\n");
    // Loop condition: if __i >= __len, exit loop
    printf("JUMPIFEQ ifj_readstr_loop_end LF@__i LF@__len\n");

    // Get character at position __i
    printf("GETCHAR LF@__char LF@__retval LF@__i\n");
    // Append character to new string
    printf("CONCAT LF@__new_str LF@__new_str LF@__char\n");
    // Increment __i
    printf("ADD LF@__i LF@__i int@1\n");
    // Loop back
    printf("JUMP ifj_readstr_loop\n");
    
    printf("LABEL ifj_readstr_loop_end\n");
    // Push the new string onto the data stack
    printf("PUSHS LF@__new_str\n");
    printf("JUMP ifj_readstr_end\n");
    
    printf("LABEL ifj_readstr_empty\n");
    // Return nil for empty input
    printf("MOVE LF@__retval nil@nil\n");
    
    printf("LABEL ifj_readstr_end\n");
    printf("PUSHS LF@__retval\n");
    printf("POPFRAME\n");
    printf("RETURN\n\n");
    
    ///////////////////////////////////////////////////////////////////////////////
    // pub fn ifj.readi32() ?i32
    printf("LABEL ifj.readi32\n");                                      // Trochu by to tu chtelo ponerfovat, je tto jaksi overbuffed zbytecne
                                                                        // Nemusime checkovat, co vlastne tisknem, who cares, jenom jestli to neni NILL

    printf("DEFVAR LF@__retval\n");
    printf("DEFVAR LF@__type\n");

    printf("READ LF@__retval int\n");
    printf("TYPE LF@__type LF@__retval\n");

    printf("JUMPIFEQ ifj_readi32_end LF@__type string@int\n");
    printf("MOVE LF@__retval nil@nil\n");

    printf("LABEL ifj_readi32_end\n");
    printf("PUSHS LF@__retval\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

    // pub fn ifj.readf64() ?f64
    printf("LABEL ifj.readf64\n");

    printf("DEFVAR LF__retval\n");
    printf("DEFVAR LF@__type\n");

    printf("READ LF@__retval float\n");
    printf("TYPE LF@__type LF@__retval\n");

    printf("JUMPIFEQ ifj_readf64_end LF@__type string@float\n");
    printf("MOVE LF@__retval nil@nil\n");

    printf("LABEL ifj_readf64_end\n");
    printf("PUSHS LF@__retval\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

    // pub fn ifj.write(term) void
    printf("LABEL ifj.write\n");
    printf("DEFVAR LF@__term\n");

    printf("MOVE LF@__term LF@__arg0\n");
    printf("DEFVAR LF@__type\n");
    printf("TYPE LF@__type LF@__term\n");

    printf("JUMPIFEQ ifj_write_nil LF@__type string@nil\n");

                // If term is float, ensure it is printed with '%a' format                  // sus
                // printf("JUMPIFEQ ifj_write_float LF@__type string@float\n");

    printf("WRITE LF@__term\n");
    printf("JUMP ifj_write_end\n");

    printf("LABEL ifj_write_float\n");
    printf("WRITE LF@__term\n");
    printf("JUMP ifj_write_end\n");

    printf("LABEL ifj_write_nil\n");
    printf("WRITE string@null\n");

    printf("LABEL ifj_write_end\n");
    printf("POPFRAME\n");
    printf("RETURN\n\n");


// ---------- Type conversion functions ----------
    /// pub fn ifj.i2f(term ∶ i32) f64
    printf("LABEL ifj.i2f\n");
    
    printf("DEFVAR LF@term\n");
    printf("MOVE LF@term LF@%%param0\n");
    
    printf("DEFVAR LF@__retval\n");
    printf("INT2FLOAT LF@__retval LF@term\n");
    
    // Push the result onto the data stack
    printf("PUSHS LF@__retval\n");
    printf("POPFRAME\n");
    printf("RETURN\n\n");

    // pub fn ifj.f2i(term ∶ f64) i32
    printf("LABEL ifj.f2i\n");
    
    printf("DEFVAR LF@term\n");
    printf("MOVE LF@term LF@%%param0\n");
    
    printf("DEFVAR LF@__retval\n");
    printf("FLOAT2INT LF@__retval LF@term\n");
    
    // Push the result onto the data stack
    printf("PUSHS LF@__retval\n");
    printf("POPFRAME\n");
    printf("RETURN\n\n");

// Functions for strings
    // pub fn ifj.string(term) []u8
    printf("LABEL ifj.string\n");

    // The parameter is in LF@%param0
    printf("DEFVAR LF@term\n");
    printf("MOVE LF@term LF@%%param0\n");

    // Push the term onto the data stack (identity function)
    printf("PUSHS LF@term\n");
    printf("POPFRAME\n");
    printf("RETURN\n\n");


    // pub fn ifj.length(𝑠 : []u8) i32
    printf("LABEL ifj.length\n");
    
    printf("DEFVAR LF@s\n");
    printf("MOVE LF@s LF@%%param0\n");
    
    printf("DEFVAR LF@__retval\n");
    printf("STRLEN LF@__retval LF@s\n");
    
    // Push the result onto the data stack
    printf("PUSHS LF@__retval\n");
    printf("POPFRAME\n");
    printf("RETURN\n\n");

    // pub fn ifj.concat(𝑠1 : []u8, 𝑠2 : []u8) []u8
    printf("LABEL ifj.concat\n");
    
    printf("DEFVAR LF@s1\n");
    printf("DEFVAR LF@s2\n");
    printf("MOVE LF@s1 LF@%%param0\n");
    printf("MOVE LF@s2 LF@%%param1\n");
    
    printf("DEFVAR LF@__retval\n");
    printf("CONCAT LF@__retval LF@s1 LF@s2\n");
    
    // Push the result onto the data stack
    printf("PUSHS LF@__retval\n");
    printf("POPFRAME\n");
    printf("RETURN\n\n");

    // pub fn ifj.substring(𝑠 : []u8, 𝑖 : i32, 𝑗 : i32) ?[]u8
    printf("LABEL ifj.substring\n");

    // Define local variables
    printf("DEFVAR LF@s\n");
    printf("DEFVAR LF@i\n");
    printf("DEFVAR LF@j\n");
    printf("MOVE LF@s LF@%%param0\n");
    printf("MOVE LF@i LF@%%param1\n");
    printf("MOVE LF@j LF@%%param2\n");

    printf("DEFVAR LF@__retval\n");
    printf("DEFVAR LF@__len\n");
    printf("DEFVAR LF@__cond\n");

    // Get the length of the string s
    printf("STRLEN LF@__len LF@s\n");

    // Check for error conditions
    // If i < 0
    printf("LT LF@__cond LF@i int@0\n");
    printf("JUMPIFEQ ifj_substring_error LF@__cond bool@true\n");

    // If j < 0
    printf("LT LF@__cond LF@j int@0\n");
    printf("JUMPIFEQ ifj_substring_error LF@__cond bool@true\n");

    // If i > j
    printf("GT LF@__cond LF@i LF@j\n");
    printf("JUMPIFEQ ifj_substring_error LF@__cond bool@true\n");

    // If i >= length(s)
    printf("GT LF@__cond LF@i LF@__len\n");
    printf("JUMPIFEQ ifj_substring_error LF@__cond bool@true\n");
    printf("EQ LF@__cond LF@i LF@__len\n");
    printf("JUMPIFEQ ifj_substring_error LF@__cond bool@true\n");

    // If j > length(s)
    printf("GT LF@__cond LF@j LF@__len\n");
    printf("JUMPIFEQ ifj_substring_error LF@__cond bool@true\n");

    // Calculate the length of the substring: len = j - i
    printf("DEFVAR LF@__sub_len\n");
    printf("SUB LF@__sub_len LF@j LF@i\n");

    // Extract the substring using SUBSTR
    printf("SUBSTR LF@__retval LF@s LF@i LF@__sub_len\n");
    printf("PUSHS LF@__retval\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

    // Error label: Return nil
    printf("LABEL ifj_substring_error\n");
    printf("MOVE LF@__retval nil@nil\n");
    printf("PUSHS LF@__retval\n");
    printf("POPFRAME\n");
    printf("RETURN\n\n");

    // pub fn ifj.strcmp(𝑠1 : []u8, 𝑠2 : []u8) i32

    printf("LABEL ifj.strcmp\n");

    // Define local variables
    printf("DEFVAR LF@s1\n");
    printf("DEFVAR LF@s2\n");
    printf("MOVE LF@s1 LF@%%param0\n");
    printf("MOVE LF@s2 LF@%%param1\n");

    printf("DEFVAR LF@__len1\n");
    printf("DEFVAR LF@__len2\n");
    printf("DEFVAR LF@__min_len\n");
    printf("DEFVAR LF@__i\n");
    printf("DEFVAR LF@__char1\n");
    printf("DEFVAR LF@__char2\n");
    printf("DEFVAR LF@__cmp_res\n");
    printf("DEFVAR LF@__retval\n");

    printf("STRLEN LF@__len1 LF@s1\n");
    printf("STRLEN LF@__len2 LF@s2\n");

    // Determine the minimum length
    printf("LT LF@__cmp_res LF@__len1 LF@__len2\n");
    printf("JUMPIFEQ ifj_strcmp_set_min_len1 LF@__cmp_res bool@true\n");
    printf("MOVE LF@__min_len LF@__len2\n");
    printf("JUMP ifj_strcmp_start\n");
    printf("LABEL ifj_strcmp_set_min_len1\n");
    printf("MOVE LF@__min_len LF@__len1\n");

    printf("LABEL ifj_strcmp_start\n");
    printf("MOVE LF@__i int@0\n");

    printf("LABEL ifj_strcmp_loop\n");
    // Loop condition: __i < __min_len
    printf("LT LF@__cmp_res LF@__i LF@__min_len\n");
    printf("JUMPIFEQ ifj_strcmp_compare_chars LF@__cmp_res bool@true\n");
    printf("JUMP ifj_strcmp_length_compare\n");

    printf("LABEL ifj_strcmp_compare_chars\n");
    // Get characters at position __i
    printf("GETCHAR LF@__char1 LF@s1 LF@__i\n");
    printf("GETCHAR LF@__char2 LF@s2 LF@__i\n");
    // Compare characters
    printf("GT LF@__cmp_res LF@__char1 LF@__char2\n");
    printf("JUMPIFEQ ifj_strcmp_s1_greater LF@__cmp_res bool@true\n");
    printf("LT LF@__cmp_res LF@__char1 LF@__char2\n");
    printf("JUMPIFEQ ifj_strcmp_s1_less LF@__cmp_res bool@true\n");
    // Characters are equal, continue loop
    printf("ADD LF@__i LF@__i int@1\n");
    printf("JUMP ifj_strcmp_loop\n");

    // If s1 > s2
    printf("LABEL ifj_strcmp_s1_greater\n");
    printf("MOVE LF@__retval int@1\n");
    printf("JUMP ifj_strcmp_end\n");

    // If s1 < s2
    printf("LABEL ifj_strcmp_s1_less\n");
    printf("MOVE LF@__retval int@-1\n");
    printf("JUMP ifj_strcmp_end\n");

    // After loop, compare lengths
    printf("LABEL ifj_strcmp_length_compare\n");
    printf("EQ LF@__cmp_res LF@__len1 LF@__len2\n");
    printf("JUMPIFEQ ifj_strcmp_equal LF@__cmp_res bool@true\n");
    printf("GT LF@__cmp_res LF@__len1 LF@__len2\n");
    printf("JUMPIFEQ ifj_strcmp_s1_greater LF@__cmp_res bool@true\n");
    // Else, s1 is less than s2
    printf("MOVE LF@__retval int@-1\n");
    printf("JUMP ifj_strcmp_end\n");

    printf("LABEL ifj_strcmp_equal\n");
    printf("MOVE LF@__retval int@0\n");
    printf("JUMP ifj_strcmp_end\n");

    printf("LABEL ifj_strcmp_end\n");
    printf("PUSHS LF@__retval\n");
    printf("POPFRAME\n");
    printf("RETURN\n\n");

    // pub fn ifj.ord(𝑠 : []u8, 𝑖 : i32) i32
    printf("LABEL ifj.ord\n");

    // Define local variables
    printf("DEFVAR LF@s\n");
    printf("DEFVAR LF@i\n");
    printf("MOVE LF@s LF@%%param0\n");
    printf("MOVE LF@i LF@%%param1\n");

    printf("DEFVAR LF@__len\n");
    printf("DEFVAR LF@__char\n");
    printf("DEFVAR LF@__retval\n");
    printf("DEFVAR LF@__cond\n");

    printf("STRLEN LF@__len LF@s\n");

    // Default return value is 0
    printf("MOVE LF@__retval int@0\n");

    // Check if s is empty
    printf("EQ LF@__cond LF@__len int@0\n");
    printf("JUMPIFEQ ifj_ord_end LF@__cond bool@true\n");

    // Check if i < 0 or i >= len(s)
    printf("LT LF@__cond LF@i int@0\n");
    printf("JUMPIFEQ ifj_ord_end LF@__cond bool@true\n");
    printf("GT LF@__cond LF@i LF@__len\n");
    printf("JUMPIFEQ ifj_ord_end LF@__cond bool@true\n");
    printf("EQ LF@__cond LF@i LF@__len\n");
    printf("JUMPIFEQ ifj_ord_end LF@__cond bool@true\n");

    // Get character at position i
    printf("GETCHAR LF@__char LF@s LF@i\n");
    // Convert character to integer (ASCII code)
    printf("STRI2INT LF@__retval LF@__char int@0\n");

    printf("LABEL ifj_ord_end\n");
    printf("PUSHS LF@__retval\n");
    printf("POPFRAME\n");
    printf("RETURN\n\n");

    // pub fn ifj.chr(𝑖 : i32) []u8
    printf("LABEL ifj.chr\n");

    // Define local variables
    printf("DEFVAR LF@i\n");
    printf("MOVE LF@i LF@%%param0\n");

    printf("DEFVAR LF@__retval\n");

    // Convert integer to character
    printf("INT2CHAR LF@__retval LF@i\n");

    // Push the result onto the data stack
    printf("PUSHS LF@__retval\n");
    printf("POPFRAME\n");
    printf("RETURN\n\n");

}
