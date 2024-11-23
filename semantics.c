#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ast.h"
#include "token.h"
#include "symtable.h"
#include "symtable_stack.h"
#include "hashtable.h"
#include "semantics.h"

/*
KNOWN BUGS:
    Cant convert 5.0 to int
    Doesnt account for values known at compile time
        (our semantics approves type conversion from int to float for variables in relational operations)
*/

// Global variable for keeping the current function name
char *current_function_name;

// Function declarations
void get_fun_declarations(AST *ast, ht_table_t *table);
void save_fun_dec(AST *ast, ht_table_t *table);
void get_builtin_fun_declarations(ht_table_t *table);
void analyze_code(AST *ast, ht_table_t *table, sym_stack_t *stack);
void var_definition(AST *ast, ht_table_t *table, sym_stack_t *stack);
symtable_type_t check_expression(AST *ast, ht_table_t *table, sym_stack_t *stack);
void new_scope_if_while(AST *ast, ht_table_t *table, sym_stack_t *stack);
void new_scope_function(AST *ast, ht_table_t *table, sym_stack_t *stack);
void check_return_expr(AST *ast, ht_table_t *table, sym_stack_t *stack);
void assignment_or_expression(AST *ast, ht_table_t *table, sym_stack_t *stack);
void check_function_call_args(AST *ast, ht_table_t *table, sym_stack_t *stack);
bool check_types_compatibility(symtable_type_t expected_type, symtable_type_t actual_type);

// Starting function
void semantic_analysis(AST *ast){
    ast->active = ast->root;

    ht_table_t table;
    ht_init(&table, 101);
    sym_stack_t stack;
    sym_stack_init(&stack);

    get_fun_declarations(ast, &table);

    ht_item_t *main_fun = get_item(&stack, &table, "main");

    // Check for main and correct definition of main
    if (main_fun == NULL){
        fprintf(stderr, "Semantic error 3: Missing main function\n");
        exit(3);
    }
    
    if (main_fun->return_type != sym_void_type){
        fprintf(stderr, "Semantic error 4: Main cannot have a return type\n");
        exit(4);
    }

    if (main_fun->params != NULL){
        fprintf(stderr, "Semantic error 4: Main cannot have parameters\n");
        exit(4);
    }

    // Insert pseudovariable _ into table
    ht_item_t item;
    item.name = "_";
    item.type = sym_void_type;
    item.var_type = sym_var;
    item.used = true;
    item.modified = true;
    item.input_parameters = -1;
    item.params = NULL;
    item.return_type = sym_void_type;

    ht_insert(&table, &item);
    get_builtin_fun_declarations(&table);

    ast->active = ast->root;
    analyze_code(ast, &table, &stack);


    ht_delete_all(&table);
}

// Goes through whole ast but saves only function declarations
void get_fun_declarations(AST *ast, ht_table_t *table){
    while(ast->active != NULL && ast->active->token->type != eof_token){
        
        if (strcmp(ast->active->token->data, "pub") == 0){
            save_fun_dec(ast, table);
        }
        else{
            next_node(ast);
        }
    }
}

// Saves function declaration to symtable
void save_fun_dec(AST *ast, ht_table_t *table){
    next_node(ast); // skip pub
    next_node(ast); // skip fn

    ht_item_t item;
    
    item.name = ast->active->token->data;
    // char *fun_name = ast->active->token->data;
    int args_cnt = 0;
    symtable_type_t *arg_types_ptr = (symtable_type_t *)malloc((sizeof(symtable_type_t))*20);

    next_node(ast); // skip fun_name
    next_node(ast); // skip (
    
    // If there are no parameters
    if (strcmp(ast->active->token->data, ")") == 0){
        free(arg_types_ptr);
        arg_types_ptr = NULL;
    }

    while(strcmp(ast->active->token->data, ")") != 0){
        next_node(ast); // skip arg_name
        next_node(ast); // skip :

        char *arg_type_name = ast->active->token->data;
        symtable_type_t arg_type;

        if (strcmp(arg_type_name, "i32") == 0){
            arg_type = sym_int_type;
        }
        else if (strcmp(arg_type_name, "?i32") == 0){
            arg_type = sym_nullable_int_type;
        }
        else if (strcmp(arg_type_name, "f64") == 0){
            arg_type = sym_float_type;
        }
        else if (strcmp(arg_type_name, "?f64") == 0){
            arg_type = sym_nullable_float_type;
        }
        else if (strcmp(arg_type_name, "[]u8") == 0){
            arg_type = sym_string_type;
        }
        else{
            arg_type = sym_nullable_string_type;
        }

        arg_types_ptr[args_cnt] = arg_type;
        args_cnt++;
        next_node(ast);
        // Skip ',', because ',' can be also after last argument but doesnt have to
        if (strcmp(ast->active->token->data, ",") == 0){
            next_node(ast);
        }
    }

    next_node(ast); // skip )
    
    // Checks return type and save it to symtable
    symtable_type_t return_type;
    if (strcmp(ast->active->token->data, "i32") == 0){
        return_type = sym_int_type;
    }
    else if (strcmp(ast->active->token->data, "?i32") == 0){
        return_type = sym_nullable_int_type;
    }
    else if (strcmp(ast->active->token->data, "f64") == 0){
        return_type = sym_float_type;
    }
    else if (strcmp(ast->active->token->data, "?f64") == 0){
        return_type = sym_nullable_float_type;
    }
    else if (strcmp(ast->active->token->data, "[]u8") == 0){
        return_type = sym_string_type;
    }
    else if (strcmp(ast->active->token->data, "?[]u8") == 0){
        return_type = sym_nullable_string_type;
    }
    else{
        return_type = sym_void_type;
    }

    // Sets the values
    item.type = sym_func_type;
    item.var_type = sym_const;
    item.used = false;
    item.modified = true;
    item.input_parameters = args_cnt;
    item.params = arg_types_ptr;
    item.return_type = return_type;

    // Inserts definition of function to symtable
    ht_insert(table, &item);

    next_node(ast); // skip return_type
    next_node(ast); // skip {
}

// Generates built-in functions declarations
void get_builtin_fun_declarations(ht_table_t *table){
    ht_item_t item;

    // Initialize item with default values same for all funs
    item.type = sym_func_type;
    item.var_type = sym_const;
    item.used = true; // Built-in functions dont have to be used
    item.modified = true;

    // Set individual values that change
    item.name = "ifj$readstr";
    item.input_parameters = 0;
    item.params = NULL;
    item.return_type = sym_nullable_string_type;
    ht_insert(table, &item);

    item.name = "ifj$readi32";
    item.return_type = sym_nullable_int_type;
    ht_insert(table, &item);

    item.name = "ifj$readf64";
    item.return_type = sym_nullable_float_type;
    ht_insert(table, &item);

    item.name = "ifj$write";
    item.input_parameters = 1;
    item.params = malloc(sizeof(symtable_type_t) * 1);
    item.params[0] = sym_void_type;     // sym_void_type indicating that it can take any type of argument
    item.return_type = sym_void_type;
    ht_insert(table, &item);

    item.name = "ifj$i2f";
    item.params = malloc(sizeof(symtable_type_t) * 1);
    item.params[0] = sym_int_type;
    item.return_type = sym_float_type;
    ht_insert(table, &item);

    item.name = "ifj$f2i";
    item.params = malloc(sizeof(symtable_type_t) * 1);
    item.params[0] = sym_float_type;
    item.return_type = sym_int_type;
    ht_insert(table, &item);

    item.name = "ifj$string";
    item.params = malloc(sizeof(symtable_type_t) * 1);
    item.params[0] = sym_string_type;
    item.return_type = sym_string_type;
    ht_insert(table, &item);

    item.name = "ifj$length";
    item.params = malloc(sizeof(symtable_type_t) * 1);
    item.params[0] = sym_string_type;
    item.return_type = sym_int_type;
    ht_insert(table, &item);

    item.name = "ifj$concat";
    item.input_parameters = 2;
    item.params = malloc(sizeof(symtable_type_t) * 2);
    item.params[0] = sym_string_type;
    item.params[1] = sym_string_type;
    item.return_type = sym_string_type;
    ht_insert(table, &item);

    item.name = "ifj$substring";
    item.input_parameters = 3;
    item.params = malloc(sizeof(symtable_type_t) * 3);
    item.params[0] = sym_string_type;
    item.params[1] = sym_int_type;
    item.params[2] = sym_int_type;
    item.return_type = sym_nullable_string_type;
    ht_insert(table, &item);

    item.name = "ifj$strcmp";
    item.input_parameters = 2;
    item.params = malloc(sizeof(symtable_type_t) * 2);
    item.params[0] = sym_string_type;
    item.params[1] = sym_string_type;
    item.return_type = sym_int_type;
    ht_insert(table, &item);

    item.name = "ifj$ord";
    item.params = malloc(sizeof(symtable_type_t) * 2);
    item.params[0] = sym_string_type;
    item.params[1] = sym_int_type;
    item.return_type = sym_int_type;
    ht_insert(table, &item);

    item.name = "ifj$chr";
    item.input_parameters = 1;
    item.params = malloc(sizeof(symtable_type_t) * 1);
    item.params[0] = sym_int_type;
    item.return_type = sym_string_type;
    ht_insert(table, &item);
}

/***************************************************** MAIN CYCLE *************************************************************/
void analyze_code(AST *ast, ht_table_t *table, sym_stack_t *stack){
    while(ast->active != NULL && ast->active->token->type != eof_token){
        // For keeping track of scopes, so we can check missing return keyword
        static int scope_cnt = 0;
        static bool found_return = false;
        static bool in_if = false;
        static bool return_in_if = false;

        if (strcmp(ast->active->token->data, "var") == 0 || strcmp(ast->active->token->data, "const") == 0){
            var_definition(ast, table, stack);
        }
        else if (strcmp(ast->active->token->data, "if") == 0 || strcmp(ast->active->token->data, "while") == 0){
            scope_cnt++;
            in_if = true;
            new_scope_if_while(ast, table, stack);
        }
        else if (strcmp(ast->active->token->data, "else") == 0){
            scope_cnt++;
            in_if = false;
            new_scope(stack, table);
            next_node(ast); // skip else
            next_node(ast); // skip {
        }
        else if (strcmp(ast->active->token->data, "pub") == 0){
            scope_cnt++;
            found_return = false;
            new_scope_function(ast, table, stack);
        }
        else if (strcmp(ast->active->token->data, "}") == 0){
            scope_cnt--;
            if (in_if == false){
                return_in_if = false;
            }
            if (scope_cnt == 1){
                in_if = false;
            }
            else if (scope_cnt == 0 && !found_return){
                ht_item_t *fun = get_item(stack, table, current_function_name);

                if (fun->return_type != sym_void_type){
                    fprintf(stderr, "Semantic error 6: Missing return for non-void function\n");
                    exit(6);
                }
            }
            leave_scope(stack, table);
            next_node(ast);
        }
        else if (strcmp(ast->active->token->data, "return") == 0){
            // Set to true only when finding return in the base function block or...
            if (scope_cnt == 1){
                found_return = true;
            }
            else if (scope_cnt == 2 && in_if == true){
                return_in_if = true;
            }
            // or when in first scope in else while also has found return in if
            else if (scope_cnt == 2 && in_if == false && return_in_if == true){
                found_return = true;
            }

            check_return_expr(ast, table, stack);
        }
        else if (ast->active->token->type == identifier_token){
            assignment_or_expression(ast, table, stack);
        }
        else{
            next_node(ast);
        }
    }
}

// Defining new variable and inserting it into symtable
void var_definition(AST *ast, ht_table_t *table, sym_stack_t *stack){
    symtable_var_type_t var_type;
    if (strcmp(ast->active->token->data, "var") == 0) {
        var_type = sym_var;
    }
    else{
        var_type = sym_const;
    }
    next_node(ast); // skip "var/const"
    char *identifier = ast->active->token->data;
    symtable_type_t type;

    next_node(ast); // skip id

    // type is defined
    if (strcmp(ast->active->token->data, ":") == 0){
        next_node(ast); // skip :

        if (strcmp(ast->active->token->data, "i32") == 0){
            type = sym_int_type;
        }
        else if (strcmp(ast->active->token->data, "f64") == 0){
            type = sym_float_type;
        }
        else if (strcmp(ast->active->token->data, "[]u8") == 0){
            type = sym_string_type;
        }
        else if (strcmp(ast->active->token->data, "?i32") == 0){
            type = sym_nullable_int_type;
        }
        else if (strcmp(ast->active->token->data, "?f64") == 0){
            type = sym_nullable_float_type;
        }
        else {
            type = sym_nullable_string_type;
        }
        next_node(ast); // skip type
        next_node(ast); // skip =

        symtable_type_t res_type;

        // Its function call
        if (strcmp(ast->active->next->token->data, "(") == 0){
            // get return type of function to compare it later to defined return type
            ht_item_t *fun = get_item(stack, table, ast->active->token->data);
            if (fun == NULL){
                fprintf(stderr, "Semantic error 3: Undefined function reference\n");
                exit(3);
            }
            res_type = fun->return_type;
            fun->used = true;

            // check correct function call
            check_function_call_args(ast, table, stack);
        }
        // its an expression
        else{
            res_type = check_expression(ast, table, stack);
        }

        // expression result type (function call return type) is incompatible with defined type
        if (res_type != type){
            if(!check_types_compatibility(type, res_type)){
                fprintf(stderr, "Semantic error 7: Type of expression (function call return type) is incompatible with defined type\n");
                exit(7);
            }
        }
    }
    // Have to derive the type from assignment expression
    else {
        next_node(ast); // skip =
        
        // Its function call
        if (strcmp(ast->active->next->token->data, "(") == 0){

            // check correct result_type
            ht_item_t *fun = get_item(stack, table, ast->active->token->data);
            if (fun == NULL){
                fprintf(stderr, "Semantic error 3: Undefined function reference\n");
                exit(3);
            }
            type = fun->return_type;
            fun->used = true;

            if (type == sym_void_type){
                fprintf(stderr, "Semantic error 7: Incompatible types when assigning from function to variable %s\n", identifier);
                exit(7);
            }

            // check correct function call
            check_function_call_args(ast, table, stack);
        }
        // its an expression
        else{

            if (ast->active->token->type == null_token) {
                fprintf(stderr, "Semantic error 8: Type is not defined and cannot be derived from the expression\n");
                exit(8);
            }

            type = check_expression(ast, table, stack);

            if (type == sym_string_type){
                fprintf(stderr, "Semantic error 8: Invalid expressing type, cannot asign string to var: %s\n", identifier);
                exit(8);
            }
        }
    }

    // Creates item to carry parameters to insert function
    ht_item_t item;
    item.name = identifier;
    item.type = type;
    item.var_type = var_type;
    item.used = false;
    item.input_parameters = -1;
    item.params = NULL;
    item.return_type = sym_void_type;

    // inserts the variable into the sym_table
    if (var_type == sym_const){
        item.modified = true;
        ht_insert(table, &item);
    }
    else{
        item.modified = false;
        ht_insert(table, &item);
    }
}


// Checks if all the operands in expression are compatible and returns type of result of the expression
symtable_type_t check_expression(AST *ast, ht_table_t *table, sym_stack_t *stack){
    ht_item_t type_stack[100] = {0};
    int stack_top = -1;

    symtable_type_t type = sym_void_type;
    while (ast->active->token->type != semicolon_token && ast->active->token->type != bracket_token){
        if (ast->active->token->type == int_token){
            type_stack[++stack_top].type = sym_int_type;
            type_stack[stack_top].var_type = sym_literal;
        }
        else if (ast->active->token->type == float_token){
            type_stack[++stack_top].type = sym_float_type;
            type_stack[stack_top].var_type = sym_literal;
        }
        else if (ast->active->token->type == string_token){
            type_stack[++stack_top].type = sym_string_type;
            type_stack[stack_top].var_type = sym_literal;
        }
        else if (ast->active->token->type == null_token){
            type_stack[++stack_top].type = sym_null_type;
            type_stack[stack_top].var_type = sym_literal;
        }
        else if (ast->active->token->type == identifier_token){
            // check if the variable is defined
            ht_item_t *var_entry = get_item(stack, table, ast->active->token->data);
            if (var_entry == NULL){
                fprintf(stderr, "Semantic error 3: Variable %s is not defined\n", ast->active->token->data);
                exit(3);
            }
            var_entry->used = true;

            type_stack[++stack_top].type = var_entry->type;
            type_stack[stack_top].var_type = sym_var;

        }
        // Binary arithmetic operations
        else if (strcmp(ast->active->token->data, "+") == 0 || strcmp(ast->active->token->data, "-") == 0  || strcmp(ast->active->token->data, "*") == 0  || strcmp(ast->active->token->data, "/") == 0){
            ht_item_t right = type_stack[stack_top--];
            ht_item_t left = type_stack[stack_top--];

            symtable_type_t right_type = right.type;
            symtable_type_t left_type = left.type;

            symtable_var_type_t right_var_type = right.var_type;
            symtable_var_type_t left_var_type = left.var_type;

            // check if the operands are compatible
            // type conversion only allowed for constants not variables
            symtable_type_t result_type;
            symtable_var_type_t result_var_type;

            // Both operands are variables
            if(left_var_type != sym_literal && right_var_type != sym_literal){
                if(left_type == right_type){
                    if (left_type == sym_nullable_int_type || left_type == sym_nullable_float_type ||
                        left_type == sym_nullable_string_type || left_type == sym_null_type){
                        fprintf(stderr, "Semantic error 7: Cannot perform arithmetic operations with nullable types\n");
                        exit(7);
                    }
                    else if (left_type == sym_string_type){
                        fprintf(stderr, "Semantic error 7: Cannot perform arithmetic operations with []u8 types\n");
                        exit(7);
                    }
                    result_type = left_type;
                }
                else if ((left_type == sym_int_type && right_type == sym_float_type) ||
                        (left_type == sym_float_type && right_type == sym_int_type)){
                    result_type = sym_float_type;
                    // Nothing needs to be done
                }
                else{
                    fprintf(stderr, "Semantic error 7: Incompatible types between variables\n");
                    exit(7);
                }
                result_var_type = sym_var;
            }
            // One operand is a variable the other is literal constant
            else if ((left_var_type != sym_literal && right_var_type == sym_literal) || (left_var_type == sym_literal && right_var_type != sym_literal)){
                symtable_type_t var_type = (left_var_type != sym_literal) ? left_type : right_type;
                symtable_type_t const_type = (left_var_type == sym_literal) ? left_type : right_type;

                if (var_type == sym_string_type){
                    fprintf(stderr, "Semantic error 7: Cannot perform arithmetic operations with strings\n");
                    exit(7);
                }
                else if (var_type == const_type){
                    result_type = var_type;
                }
                else if (var_type == sym_float_type && const_type == sym_int_type){
                    result_type = sym_float_type;
                }
                else {
                    fprintf(stderr, "Semantic error 7: Incompatible types between variable and literal\n");
                    exit(7);
                }
                result_var_type = sym_var;
            }
            // Both operands are literal constants
            else if (left_var_type == sym_literal && right_var_type == sym_literal){
                if (left_type == sym_int_type && right_type == sym_int_type){
                    result_type = sym_int_type;
                }
                else if ((left_type == sym_int_type && right_type == sym_float_type) || (left_type == sym_float_type && right_type == sym_int_type) || (left_type == sym_float_type && right_type == sym_float_type)){
                    result_type = sym_float_type;
                }
                else {
                    fprintf(stderr, "Semantic error 7: Incompatible types between literals\n");
                    exit(7);
                }
                result_var_type = sym_literal;
            }
            else{
                // Should not reach here, just in case...
                fprintf(stderr, "Semantic error 7: Unknown operand kinds\n");
                exit(7);
            }
            type_stack[++stack_top].type = result_type;
            type_stack[stack_top].var_type = result_var_type;
        }
        // Relational operation
        else if (strcmp(ast->active->token->data, "<") == 0 || strcmp(ast->active->token->data, ">") == 0 ||
                strcmp(ast->active->token->data, "<=") == 0 || strcmp(ast->active->token->data, ">=") == 0){

            ht_item_t right = type_stack[stack_top--];
            ht_item_t left = type_stack[stack_top--];

            symtable_type_t right_type = right.type;
            symtable_type_t left_type = left.type;

            symtable_var_type_t right_var_type = right.var_type;
            symtable_var_type_t left_var_type = left.var_type;

            symtable_type_t result_type = sym_bool_type;
            symtable_var_type_t result_var_type = sym_literal;

            // Both operands are variables
            if(left_var_type != sym_literal && right_var_type != sym_literal){
                // Same types
                if (left_type == right_type){
                    // Both are nullable/null type
                    if (left_type == sym_nullable_int_type || left_type == sym_nullable_float_type ||
                        left_type == sym_nullable_string_type || left_type == sym_null_type){
                        fprintf(stderr, "Semantic error 7: Cannot perform relational operations with nullable types\n");
                        exit(7);
                    }
                    // Both are []u8
                    else if (left_type == sym_string_type){
                        fprintf(stderr, "Semantic error 7: Cannot perform relational operations with []u8 types\n");
                        exit(7);
                    }
                }
                // One is int, other float
                else if ((left_type == sym_int_type && right_type == sym_float_type) ||
                        (left_type == sym_float_type && right_type == sym_int_type)){
                    // Nothing needs to be done
                }
                else{
                    fprintf(stderr, "Semantic error 7: Incompatible types between variables\n");
                    exit(7);
                }
                result_var_type = sym_var;
            }
            // One is variable, one is literal constant
            else if ((left_var_type != sym_literal && right_var_type == sym_literal) || (left_var_type == sym_literal && right_var_type != sym_literal)){
                symtable_type_t var_type = (left_var_type != sym_literal) ? left_type : right_type;
                symtable_type_t const_type = (left_var_type == sym_literal) ? left_type : right_type;
                
                if (var_type == sym_string_type){
                    fprintf(stderr, "Semantic error 7: Cannot perform arithmetic operations with strings\n");
                    exit(7);
                }
                else if (var_type == const_type){
                    // Nothing needs to be done
                }
                // Possible int -> float conversion
                else if ((var_type == sym_float_type && const_type == sym_int_type) ||
                        (var_type == sym_int_type && const_type == sym_float_type)){
                    // Nothing needs to be done
                }
                else {
                    fprintf(stderr, "Semantic error 7: Incompatible types between variable and literal\n");
                    exit(7);
                }
                result_var_type = sym_var;
            }
            // Both are literals
            else{
                if ((left_type == sym_int_type && right_type == sym_int_type) ||
                    (left_type == sym_float_type && right_type == sym_float_type)){
                    // Nothing needs to be done
                }
                else if ((left_type == sym_int_type && right_type == sym_float_type) ||
                        (left_type == sym_float_type && right_type == sym_int_type)){
                    // Nothing needs to be done
                }
                else {
                    fprintf(stderr, "Semantic error 7: Incompatible types between literals\n");
                    exit(7);
                }
                result_var_type = sym_literal;
            }
            result_type = sym_bool_type;

            // Push the result back onto the stack
            type_stack[++stack_top].type = result_type;
            type_stack[stack_top].var_type = result_var_type;
        }
        // Relational operations using == or !=
        else if (strcmp(ast->active->token->data, "==") == 0 || strcmp(ast->active->token->data, "!=") == 0){
            ht_item_t right = type_stack[stack_top--];
            ht_item_t left = type_stack[stack_top--];

            symtable_type_t right_type = right.type;
            symtable_type_t left_type = left.type;

            symtable_var_type_t right_var_type = right.var_type;
            symtable_var_type_t left_var_type = left.var_type;

            symtable_type_t result_type = sym_bool_type;
            symtable_var_type_t result_var_type = sym_literal;

            // Both operands are variables
            if(left_var_type != sym_literal && right_var_type != sym_literal){
                if (left_type == right_type){
                    if (left_type == sym_nullable_string_type || left_type == sym_string_type){
                        fprintf(stderr, "Semantic error 7: Cannot perform relational operations with []u8 types\n");
                        exit(7);
                    }
                }
                // One is int, other float
                else if ((left_type == sym_int_type && right_type == sym_float_type) ||
                        (left_type == sym_float_type && right_type == sym_int_type)){
                    // Nothing needs to be done
                }
                // Same types but one of them is nullable
                else if ((left_type == sym_int_type && right_type == sym_nullable_int_type) || (right_type == sym_int_type && left_type == sym_nullable_int_type) ||
                    (left_type == sym_float_type && right_type == sym_nullable_float_type) || (right_type == sym_float_type && left_type == sym_nullable_float_type)){
                    // Correct and nothing needs to be done
                }
                else{
                    fprintf(stderr, "Semantic error 7: Incompatible types between variables\n");
                    exit(7);
                }
                result_var_type = sym_var;
            }
            // One is variable, one is literal
            else if ((left_var_type != sym_literal && right_var_type == sym_literal) || (left_var_type == sym_literal && right_var_type != sym_literal)){
                symtable_type_t var_type = (left_var_type != sym_literal) ? left_type : right_type;
                symtable_type_t const_type = (left_var_type == sym_literal) ? left_type : right_type;
                
                if (var_type == sym_string_type){
                    fprintf(stderr, "Semantic error 7: Cannot perform arithmetic operations with strings\n");
                    exit(7);
                }
                else if (var_type == const_type){
                    // Nothing needs to be done
                }
                // Possible int -> float conversion
                else if ((var_type == sym_float_type && const_type == sym_int_type) ||
                        (var_type == sym_int_type && const_type == sym_float_type)){
                    // Nothing needs to be done
                }
                else if (var_type == sym_nullable_int_type && const_type == sym_int_type){
                    // Nothing needs to be done
                }
                else if (var_type == sym_nullable_float_type && const_type == sym_float_type){
                    // Nothing needs to be done
                }
                else if (const_type == sym_null_type && 
                    (var_type == sym_nullable_int_type || var_type == sym_nullable_float_type || var_type == sym_nullable_string_type)){
                    // Nothing needs to be done
                    }
                else {
                    fprintf(stderr, "Semantic error 7: Incompatible types between variable and literal\n");
                    exit(7);
                }
                result_var_type = sym_var;
            }
            // Both are literals
            else{
                if ((left_type == sym_int_type && right_type == sym_int_type) ||
                    (left_type == sym_float_type && right_type == sym_float_type)){
                    // Nothing needs to be done
                }
                else if ((left_type == sym_int_type && right_type == sym_float_type) ||
                        (left_type == sym_float_type && right_type == sym_int_type)){
                    // Nothing needs to be done
                }
                else if (left_type == sym_null_type && right_type == sym_null_type){
                    // Nothing needs to be done
                }
                else {
                    fprintf(stderr, "Semantic error 7: Incompatible types between literals\n");
                    exit(7);
                }
                result_var_type = sym_literal;
            }
            result_type = sym_bool_type;

            // Push the result back onto the stack
            type_stack[++stack_top].type = result_type;
            type_stack[stack_top].var_type = result_var_type;
        }
        next_node(ast);
    }

    type = type_stack[stack_top].type;
    return type;
}

// Creates new scope for if/while and defines new variable if there is while/if extension
void new_scope_if_while(AST *ast, ht_table_t *table, sym_stack_t *stack){

    new_scope(stack, table);

    next_node(ast); // skip while/if
    next_node(ast); // skip (

    // Doesnt have extension
    if (strcmp(ast->active->next->next->token->data, "|") != 0){
        // Get expression result type
        symtable_type_t type = check_expression(ast, table, stack);
        // after expression, ast->active is ) || ;

        if (type != sym_bool_type){
            fprintf(stderr, "Semantic error 7: Condition result is not of type boolean\n");
            exit(7);
        }

        next_node(ast); // skip )
        next_node(ast); // skip {

    }
    // Does have extension
    else{
        ht_item_t *item = get_item(stack, table, ast->active->token->data);
        if (item == NULL){
            fprintf(stderr, "Semantic error 3: Undefined variable in condition\n");
            exit(3);
        }

        item->used = true;
        // Checks if variable in condition is of type including null
        symtable_type_t type = item->type;
        if (type != sym_nullable_int_type && type != sym_nullable_float_type && type != sym_nullable_string_type) {
            fprintf(stderr, "Semantic error 7: Variable is not of type including null\n");
            exit(7);
        }
        // Convert to type not including null
        type--;

        next_node(ast); // skip expr
        next_node(ast); // skip )
        next_node(ast); // skip |

        ht_item_t new_item;
        new_item.name = ast->active->token->data;
        new_item.type = type;
        new_item.var_type = sym_var;
        new_item.used = false;
        new_item.modified = true;
        new_item.input_parameters = -1;
        new_item.params = NULL;
        new_item.return_type = sym_void_type;

        ht_insert(table, &new_item);
        
        next_node(ast); // skip id_without_null
        next_node(ast); // skip |
        next_node(ast); // skip {
    }
}

// Creates new scope for function and define fun arguments in it
void new_scope_function(AST *ast, ht_table_t *table, sym_stack_t *stack){
    next_node(ast); // skip pub
    next_node(ast); // skip fn
    
    current_function_name = ast->active->token->data; // saving the name of the current function we are in
    next_node(ast); // skip fun_name
    next_node(ast); // skip (

    new_scope(stack, table);

    while(strcmp(ast->active->token->data, ")") != 0){
        char *arg_name = ast->active->token->data;
        next_node(ast); // skip arg_name
        next_node(ast); // skip :

        char *arg_type_name = ast->active->token->data;
        symtable_type_t arg_type;

        if (strcmp(arg_type_name, "i32") == 0){
            arg_type = sym_int_type;
        }
        else if (strcmp(arg_type_name, "?i32") == 0){
            arg_type = sym_nullable_int_type;
        }
        else if (strcmp(arg_type_name, "f64") == 0){
            arg_type = sym_float_type;
        }
        else if (strcmp(arg_type_name, "?f64") == 0){
            arg_type = sym_nullable_float_type;
        }
        else if (strcmp(arg_type_name, "[]u8") == 0){
            arg_type = sym_string_type;
        }
        else{
            arg_type = sym_nullable_string_type;
        }

        next_node(ast); // skip type

        ht_item_t item;
        item.name = arg_name;
        item.type = arg_type;
        item.var_type = sym_const;
        item.used = false;
        item.modified = true;
        item.input_parameters = -1;
        item.params = NULL;
        item.return_type = sym_void_type;

        ht_insert(table, &item);

        // Skip ',', because ',' can be also after last argument but doesnt have to
        if (strcmp(ast->active->token->data, ",") == 0){
            next_node(ast);
        }
    }

    next_node(ast); // skip )
    next_node(ast); // skip return_type
    next_node(ast); // skip {
}

// Calls check_expression and compares the type to the type the expression is supposed to return
// (DOESNT leave scope, that will happen next time it goes through while)
void check_return_expr(AST *ast, ht_table_t *table, sym_stack_t *stack){
    next_node(ast); // skip return
    
    ht_item_t *fun_entry = get_item(stack, table, current_function_name);
    symtable_type_t current_function_type = fun_entry->return_type;

    // Check for "return;"
    if (ast->active->token->type == semicolon_token){
        if(current_function_type != sym_void_type){
            fprintf(stderr, "Semantic error 6: Missing expression in function return\n");
            exit(6);
        }

        next_node(ast); // skip ;
        return;
    }

    if (current_function_type == sym_void_type){
        fprintf(stderr, "Semantic error 6: Returning expression in a function with void return type\n");
        exit(6);
    }

    symtable_type_t expr_type = check_expression(ast, table, stack);

    if (expr_type != current_function_type){
        if (!check_types_compatibility(current_function_type, expr_type)){
            fprintf(stderr, "Semantic error 4: Function '%s' return type mismatch.\n", current_function_name);
            exit(4);
        }
    }
    
    next_node(ast); // skip ;
}

/* Can be:
foo();
x = foo;
x = y + 2;
*/
void assignment_or_expression(AST *ast, ht_table_t *table, sym_stack_t *stack){
    // Its a function call without assignment
    if (strcmp(ast->active->next->token->data, "(") == 0){
        char *fun_name = ast->active->token->data;
        ht_item_t *fun = get_item(stack, table, fun_name);
        if (fun == NULL){
            fprintf(stderr, "Semantic error 3: Undefined function reference\n");
            exit(3);
        }
        fun->used = true;

        if (fun->return_type != sym_void_type){
            fprintf(stderr, "Semantic error 4: Illegal discarding of function return value\n");
            exit(4);
        }
        check_function_call_args(ast, table, stack);
    }
    // Its an assignment
    else{
        char *var_name = ast->active->token->data;
        ht_item_t *var = get_item(stack, table, var_name);
        var->used = true;
        var->modified = true;
        
        if (var->var_type == sym_const){
            fprintf(stderr, "Semantic error 5: Cannot modify variable %s of type const\n", var_name);
            exit(5);
        }

        symtable_type_t var_type = var->type;

        next_node(ast); // skip var_name
        next_node(ast); // skip =

        // Its a function assignment
        if (strcmp(ast->active->next->token->data, "(") == 0){
            char *fun_name = ast->active->token->data;
            ht_item_t *fun = get_item(stack, table, fun_name);
            if (fun == NULL){
                fprintf(stderr, "Semantic error 3: Undefined function reference '%s'\n", fun_name);
                exit(3);
            }

            fun->used = true;

            symtable_type_t fun_ret_type = fun->return_type;

            if (fun_ret_type == sym_void_type){
                fprintf(stderr, "Semantic erorr 7: Assignment from function '%s' that doesnt return anything\n", fun->name);
                exit(7);
            }
            else if (fun_ret_type != var_type && strcmp(var_name, "_") != 0){
                if (!check_types_compatibility(var_type, fun_ret_type)){
                    fprintf(stderr, "Semantic erorr 7: Incompatible types when assigning from function\n");
                    exit(7);
                }
            }

            check_function_call_args(ast, table, stack);

        }
        // Its an expression assignment
        else{
            symtable_type_t expr_res_type = check_expression(ast, table, stack);
            
            if (var_type != expr_res_type && strcmp(var_name, "_") != 0){
                if (!check_types_compatibility(var_type, expr_res_type)){
                    fprintf(stderr, "Semantic error 7: Incompatible assignment type\n");
                    exit(7);
                }
            }
        }
    }
}

// Checks if given function is called with correct types of arguments
// ast->active == function_name
void check_function_call_args(AST *ast, ht_table_t *table, sym_stack_t *stack){
    ht_item_t *fun_entry = get_item(stack, table, ast->active->token->data);
    // Function reference is always already checked before calling this
    int expected_params = fun_entry->input_parameters;
    symtable_type_t *expected_types = fun_entry->params;
    
    next_node(ast); // skip function name
    next_node(ast); // skip '(
    
    int idx = 0;
    while (strcmp(ast->active->token->data, ")") != 0){
        symtable_type_t arg_type;

        if (ast->active->token->type == identifier_token){
            ht_item_t *var_entry = get_item(stack, table, ast->active->token->data);
            if (var_entry == NULL){
                fprintf(stderr, "Semantic error 3: Variable '%s' not defined\n", ast->active->token->data);
                exit(3);
            }
            var_entry->used = true;
            arg_type = var_entry->type;
        }
        else if (ast->active->token->type == int_token){
            arg_type = sym_int_type;
        }
        else if (ast->active->token->type == float_token){
            arg_type = sym_float_type;
        }
        else if (ast->active->token->type == string_token){
            arg_type = sym_string_type;
        }
        else if (ast->active->token->type == null_token){
            arg_type = sym_null_type;
        }

        symtable_type_t expected_type = expected_types[idx];


        // Check for correct argument type
        if (arg_type != expected_type && expected_type != sym_void_type){
            if (!check_types_compatibility(expected_type, arg_type)){
                fprintf(stderr, "Semantic error 4: Invalid argument type\n");
                exit(4);
            }
        }

        idx++;
        next_node(ast);
        
        // skip ','
        if (strcmp(ast->active->token->data, ",") == 0){
            next_node(ast);
        }
    }
    if (idx != expected_params){
        fprintf(stderr, "Semantic error 4: Invalid number of arguments\n");
        exit(4);
    }
    next_node(ast); // skip ')'
}

// For checking compatibility with nullable types
// Can use only when the types are different
bool check_types_compatibility(symtable_type_t expected_type, symtable_type_t actual_type){
    if (expected_type == sym_int_type){
        return false;
    }
    else if (expected_type == sym_nullable_int_type){
        if (actual_type == sym_int_type || actual_type == sym_null_type){
            return true;
        }
        else{
            return false;
        }
    }
    else if (expected_type == sym_float_type){
        return false;
    }
    else if (expected_type == sym_nullable_float_type){
        if (actual_type == sym_float_type || actual_type == sym_null_type){
            return true;
        }
        else{
            return false;
        }
    }
    else if (expected_type == sym_string_type){
        return false;
    }
    else if (expected_type == sym_nullable_string_type){
        if (actual_type == sym_string_type || actual_type == sym_null_type){
            return true;
        }
        else{
            return false;
        }
    }
    else{
        return false;
    }
}
