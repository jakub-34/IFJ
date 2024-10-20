#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "./lexer.h"
#include "./token.h"
#include "./string_stack.h"
#include "./bts_stack.h"
#include "./btree.h"

/*pravdila:

E -> E + E 
E -> E - E 
E -> E * E 
E -> E / E 
E -> ( E )
E -> int
E -> float
E -> variable
E -> E == E
E -> E != E
E -> E < E
E -> E > E
E -> E <= E
E -> E >= E


gramatika:

Operator	    *, /	+, -	==, !=	  <, >, <=, >=	(	)	Operand  $
*, /		     >	     >	        >	      >		    <	>	   <     >
+, -		     <	     >	        >	      >		    <	>	   <     >
==, !=		     <	     <	        >	      >		    <	>	   <     >
<, >, <=, >=	 <	     <	        <	      >		    <	>	   <     >
(		         <	     <	        <	      <		    <	=	   <     
)		         >	     >	        >	      >			    >	         >
Operand		     >	     >	        >	      >			    >	         >
$	             <       <	        <	      <	       <           <	


*/

int main(){

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


//initializing 
token_t *token = get_token();

Stack stack;
Stack_Init(&stack, 10);
Stack_Push(&stack, "$", 10);

bts_Stack bts_stack;
bts_Stack_Init(&bts_stack, 10);
while (token != NULL && !(Stack_IsEmpty(&stack))){

    if(token->type == eof_token){
    break;
    }
    printf("Token: %s\n", token->data);
    
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
    printf("%d\n",Stack_find(&stack));
    printf("Token: %s\n", topToken);
        for (int i = 0; i < 15; i++) {
            if (strcmp(topToken, precedence_table[0][i]) == 0) {
            row = i;
            }
        }

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
        token = get_token();
    }
    else if (strcmp(precedence_table[row][column], "R") == 0) {
        // reduce(&stack);
        Stack_extract_str(&stack);
        Stack_Push(&stack, "E",10);
    }

    printf("%s \n", precedence_table[row][column]);        
    for(int i = 0; i <= stack.topIndex; i++){
        printf("%s ", stack.string[i]);
    }      
    printf("\n");
    }


     return 0;
}



