/*
* Project: Implementacia prekladaca imperativneho jazyka IFJ2024
*
* @author: Rebeka Tydorova <xtydor01>
*
*/
#include "string_stack.h"
#include <stdlib.h>
#include <string.h>

//initialize stack
void Stack_Init(Stack *stack) {
	stack->size = 50;
	stack->string = (char **)malloc(stack->size * sizeof(char *));
	if(stack->string == NULL){
		fprintf(stderr, "Error: malloc failed\n");
		exit(99);
	}
	stack->topIndex = -1;
}

//check if the stack is empty
bool Stack_IsEmpty(const Stack *stack) {
	return stack->topIndex == -1;
}

//check if the stack is full
bool Stack_IsFull(const Stack *stack) {
	return stack->size == stack->topIndex + 1;
}

//return the top element of the stack
char *Stack_Top(const Stack *stack) {
	if (Stack_IsEmpty(stack)){
		return NULL;
	}
	return stack->string[stack->topIndex];
}

//remove the top element of the stack
void Stack_Pop( Stack *stack ) {
	if (stack->topIndex != -1){
		stack->topIndex --;
	}
}

// Resize stack
void Stack_resize(Stack *stack){
    stack->size *= 2;
    stack->string = (char **)realloc(stack->string, sizeof(char *) * stack->size);
    if(stack->string == NULL){
        fprintf(stderr, "Error: realloc failed\n");
        exit(99);
    }
}

//push a new element onto the stack
void Stack_Push(Stack *stack, char *string) {
	if (Stack_IsFull(stack)){
		Stack_resize(stack);
	}

	stack->topIndex++;
	stack->string[stack->topIndex] = string;
}

//dispose stack
void Stack_Dispose( Stack *stack ) {
	stack->topIndex = -1;
	free(stack->string);
	stack->string = NULL;
}

//return index of the first occurrence of stop sign(=[) in the stack from top to bottom
int Stack_less_than(Stack *stack) {
	for (int stack_idx = stack->topIndex; stack_idx >= 0; stack_idx--) {
		if (!strcmp(stack->string[stack_idx],"[")) {
			return stack_idx;
		}
	}
	return 0;
}

//find the first element from the top that is E or stop sign
int Stack_find(Stack *stack) {
	int less_than_idx = stack->topIndex;
	for (int stack_idx = stack->topIndex; stack_idx >= 0; stack_idx--) {
		if (strcmp(stack->string[stack_idx],"E") && strcmp(stack->string[stack_idx],"[")) {
			less_than_idx = stack_idx;
			return stack_idx;
		}
	}
	return less_than_idx;
}

//create string from the stack elements between the first stop sign and the top of the stack
char *Stack_rule_str(Stack *stack) {
    int stack_idx = Stack_less_than(stack);
    if (stack_idx == 0) {
        return NULL;
    }
    int length = 0;
    for (int i = stack_idx; i < stack->topIndex; i++) {
        length += strlen(stack->string[i]);
    }
    char *string = malloc((length + 1) * sizeof(char));
    if (string == NULL) {
        return NULL;
    }
    string[0] = '\0';
    for (int i = stack_idx+1; i <= stack->topIndex; i++) {
        strcat(string, stack->string[i]);
    }
    return string;
}

//extract string from the stack elements between stop sign and the top of the stack
char *Stack_extract_str(Stack *stack) {
	int stack_idx = Stack_less_than(stack);
	char *string = malloc((stack->topIndex-stack_idx+1)*sizeof(char));
	string[0] = '\0';
	for (int i = stack_idx+1; i < stack->topIndex; i++) {
		strcat(string,stack->string[i]);
	}
	stack->topIndex = stack_idx-1;
	return string;
}

//inserts a stop sign into the stack
void Stack_insert_str(Stack *stack) {
	if (strcmp(Stack_Top(stack), "E") != 0){
		Stack_Push(stack, "[");
	}
	else{
		Stack_Pop(stack);
		Stack_Push(stack, "[");
		Stack_Push(stack, "E");
	}
}