/*
* Project: Implementacia prekladaca imperativneho jazyka IFJ2024
*
* @author: Rebeka Tydorova <xtydor01>
*
*/
#ifndef _STACK_H_
#define _STACK_H_

#include <stdio.h>
#include <stdbool.h>
#include "lexer.h"

//stack
typedef struct {
	char **string;
	int size;
	int topIndex;
} Stack;

//initialize stack
void Stack_Init(Stack *stack);

//check if the stack is empty
bool Stack_IsEmpty(const Stack *stack);

//check if the stack is full
bool Stack_IsFull(const Stack *stack);

//return the top element of the stack
char *Stack_Top(const Stack *stack);

//remove the top element of the stack
void Stack_Pop(Stack *stack);

//push a new element onto the stack
void Stack_Push(Stack *stack, char *string);

//dispose stack
void Stack_Dispose(Stack *stack);

//find the first element from the top that is E or stop sign
int Stack_find(Stack *stack);

//create string from the stack elements between the first stop sign and the top of the stack
char *Stack_rule_str(Stack *stack);

//inserts a stop sign into the stack
void Stack_insert_str(Stack *stack);

//extract string from the stack elements between stop sign and the top of the stack
char *Stack_extract_str(Stack *stack);

#endif

