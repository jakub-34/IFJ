#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "./lexer.h"
#include "./token.h"
#include "./string_stack.h"
#include "./bts_stack.h"
#include "./btree.h"
#include "./expression.h"


/**
 * @brief append to binary tree and push to stack
 * 
 * @param[in] stack binary stack
 * @param[in] operation value for root node
 */
void bts_append(bts_Stack *stack, char* operation) {
    //get nodes from stack
    bst_node_t *right_child = bts_Stack_Top(stack); 
    bts_Stack_Pop(stack); 
    bst_node_t *left_child = bts_Stack_Top(stack);
    bts_Stack_Pop(stack); 

    //token representing operation
    token_t *token = (token_t *)malloc(sizeof(token_t));
    if (token == NULL) {
        fprintf(stderr, "Error: malloc failed for token\n");
        exit(99);
    }
    token->data = strdup(operation);
    if (token->data == NULL) {
        fprintf(stderr, "Error: strdup failed\n");
        free(token);
        exit(1);
    }
    token->type = binary_operator_token;

    //creating new node
    bst_node_t *node = bts_create_node(token);
    node->left = left_child;
    node->right = right_child;

    printf("root je %s\n", token->data);
    printf("lavy koren je %s\n", node->left->value->data);
    printf("pravy koren je %s\n", node->right->value->data);

    //push new node to stack
    bts_Stack_Push(stack, node, 10); 
}

/**
 * @brief rules recognition
 * 
 * @param[in] rule extracted string from string stack
 * @param[in] stack binary stack
 */
void process_rule(const char *rule, bts_Stack *stack) {
    if (strcmp(rule, "E+E") == 0) {
        printf("rule: E -> E + E\n");
        bts_append(stack,  "+");
    } 
    else if (strcmp(rule, "E-E") == 0) {
        printf("rule: E -> E - E\n");
        bts_append(stack,  "-");
    } 
    else if (strcmp(rule, "E*E") == 0) {
        printf("rule: E -> E * E\n");
        bts_append(stack,  "*");
    } 
    else if (strcmp(rule, "E/E") == 0) {
        printf("rule: E -> E / E\n");
        bts_append(stack,  "/");
    } 
    else if (strcmp(rule, "(E)") == 0) {
        printf("rule: E -> ( E )\n");
    } 
    else if (strcmp(rule, "i") == 0) {
        printf("rule: E -> i\n");
    } 
    else if (strcmp(rule, "E==E") == 0) {
        printf("rule: E -> E == E\n");
        bts_append(stack,  "==");
    } 
    else if (strcmp(rule, "E!=E") == 0) {
        printf("rule: E -> E != E\n");
        bts_append(stack,  "!=");
    } 
    else if (strcmp(rule, "E<E") == 0) {
        printf("rule: E -> E < E\n");
        bts_append(stack,  "<");
    } 
    else if (strcmp(rule, "E>E") == 0) {
        printf("rule: E -> E > E\n");
        bts_append(stack,  ">");
    } 
    else if (strcmp(rule, "E<=E") == 0) {
        printf("rule: E -> E <= E\n");
        bts_append(stack,  "<=");
    } 
    else if (strcmp(rule, "E>=E") == 0) {
        printf("rule: E -> E >= E\n");
        bts_append(stack,  ">=");
    } else {
        printf("rule unknown\n");
    }
}

/**
 * @brief checking input token
 * 
 * @param[in] token input token
 * @param[in] brackets count
 * @param[in] output_token output token
 */
token_t* check_token(token_t* token, int* brackets, token_t* output_token){
    if(*brackets != -1){
        token = get_token();
        if (strcmp(token->data,"(") == 0){
            (*brackets)++;
        }
        else if (strcmp(token->data,")") == 0){
            (*brackets)--;
        }
    }
            
    if(strcmp(token->data, ";") == 0 || *brackets == -1){
        output_token->data = token->data;
        output_token->type = token->type;
        token->data = "$";
    }

    return token;
}

//precedence table
const char *precedence_table[15][15] = {
    {"",     "*",   "/",   "+",   "-",  "==",  "!=",   "<",   ">",   "<=",  ">=",  "(",   ")",   "i", "$"},
    { "*",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   "S",   "R",   "S", "R"},
    { "/",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   "S",   "R",   "S", "R"},
    { "+",   "S",   "S",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   "S",   "R",   "S", "R"},
    { "-",   "S",   "S",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   "S",   "R",   "S", "R"},
    { "==",  "S",   "S",   "S",   "S",   "R",   "R",   "S",   "S",   "S",   "S",   "S",   "R",   "S", "R"},
    { "!=",  "S",   "S",   "S",   "S",   "R",   "R",   "S",   "S",   "S",   "S",   "S",   "R",   "S", "R"},
    { "<",   "S",   "S",   "S",   "S",   "S",   "S",   "R",   "R",   "R",   "R",   "S",   "R",   "S", "R"},
    { ">",   "S",   "S",   "S",   "S",   "S",   "S",   "R",   "R",   "R",   "R",   "S",   "R",   "S", "R"},
    { "<=",  "S",   "S",   "S",   "S",   "S",   "S",   "R",   "R",   "R",   "R",   "S",   "R",   "S", "R"},
    { ">=",  "S",   "S",   "S",   "S",   "S",   "S",   "R",   "R",   "R",   "R",   "S",   "R",   "S", "R"},
    { "(",   "S",   "S",   "S",   "S",   "S",   "S",   "S",   "S",   "S",   "S",   "S",   "=",   "S", " "},
    { ")",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   " ",   "R",   " ", "R"},
    { "i",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   "R",   " ",   "R",   " ", "R"},
    { "$",   "S",   "S",   "S",   "S",   "S",   "S",   "S",   "S",   "S",   "S",   "S",   " ",   "S", " "}
        
};

token_t* expression(token_t *token){

    //initializing 
    int brackets = 0;

    if (strcmp(token->data,"(") == 0){
        brackets++;
    }
    else if (strcmp(token->data,")") == 0){
        brackets--;
    }

    Stack stack;
    Stack_Init(&stack, 10);
    Stack_Push(&stack, "$", 10);

    bts_Stack bts_stack;
    bts_Stack_Init(&bts_stack, 10);

    token_t *output_token;
    output_token = malloc(sizeof(token_t));


    while (true) {
        printf("Token_input: %s\n", token->data);
        printf("%i\n",brackets);

        //getting column number in precedence table
        int row;
        int column;
        if(token->type == identifier_token || token->type == int_token || token->type == float_token || token->type == string_token) {

            column = 13;
        }
        else{
            for (int i = 0; i < 15; i++) {
                if (strcmp(token->data, precedence_table[0][i]) == 0) {
                column = i;
                }
            }
        }
        
        //getting row number in precedence table
        char *topToken = stack.string[Stack_find(&stack)];
        printf("Token_stack: %s\n", topToken);
            for (int i = 0; i < 15; i++) {
                if (strcmp(topToken, precedence_table[0][i]) == 0) {
                row = i;
                }
            }

        //expression processed 
        if (strcmp(topToken, "$") == 0 && strcmp(token->data, "$") == 0){
            break;
        }

        //shift
        if(strcmp(precedence_table[row][column], "S") == 0){
            if(token->type == identifier_token || token->type == int_token || token->type == float_token || token->type == string_token) {
            Stack_insert_str(&stack);
                Stack_Push(&stack, "i", 10);
                bst_node_t *node = bts_create_node(token);
                bts_Stack_Push(&bts_stack,node,10);
            }
            else {
                Stack_insert_str(&stack);
                Stack_Push(&stack, token->data, 10);
            }

            token = check_token(token, &brackets, output_token);
        }

        //reduce
        else if (strcmp(precedence_table[row][column], "R") == 0) {
            char *rule = Stack_rule_str(&stack);
            process_rule(rule, &bts_stack);
            Stack_extract_str(&stack);
            Stack_Push(&stack, "E",10);
        }

        //equal operation
        else if (strcmp(precedence_table[row][column], "=") == 0) {
            Stack_Push(&stack, token->data, 10);
            char *rule = Stack_rule_str(&stack);
            process_rule(rule, &bts_stack);
            Stack_extract_str(&stack);
            Stack_Push(&stack, "E",10);

            token = check_token(token, &brackets, output_token);
        }

        //stack
        printf("%s \n", precedence_table[row][column]);        
        for(int i = 0; i <= stack.topIndex; i++){
            printf("%s ", stack.string[i]);
        }      
        printf("\n");


    }
    
    printf("output data %s\n", output_token->data);
    printf("output type %i\n", output_token->type);
    printf("end\n");

    return output_token;
}


