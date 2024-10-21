#include "./string_stack.h"
#include <stdlib.h>
#include <string.h>

void Stack_Init(Stack *stack, size_t size) {
	stack->string = (char **)malloc(size * sizeof(char *));
	stack->topIndex = -1;
}

bool Stack_IsEmpty(const Stack *stack) {
	return stack->topIndex == -1;
}

bool Stack_IsFull(const Stack *stack, size_t size) {
	return size == stack->topIndex + 1;
}

char *Stack_Top(const Stack *stack) {
	if (Stack_IsEmpty(stack)){
		return NULL;
	}
	return stack->string[stack->topIndex];
}

void Stack_Pop( Stack *stack ) {
	if (stack->topIndex != -1){
		stack->topIndex --;
	}
}

void Stack_Push(Stack *stack, char *string, size_t size) {
	if (Stack_IsFull(stack, size)){
		return;
	}

	stack->topIndex ++;
	stack->string[stack->topIndex] = string;
	
}


void Stack_Dispose( Stack *stack ) {
	stack->topIndex = -1;
	free(stack->string);
	stack->string = NULL;
	}

int Stack_less_than(Stack *stack) {
	for (int stack_idx = stack->topIndex; stack_idx >= 0; stack_idx--) {
		if (!strcmp(stack->string[stack_idx],"<")) {
			return stack_idx;
		}
	}
	return 0;
}

int Stack_find(Stack *stack) {
	int less_than_idx = stack->topIndex;
	for (int stack_idx = stack->topIndex; stack_idx >= 0; stack_idx--) {
		if (strcmp(stack->string[stack_idx],"E") && strcmp(stack->string[stack_idx],"<")) {
			less_than_idx = stack_idx;
			return stack_idx;
		}
	}
	return less_than_idx;
}

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

void Stack_insert_str(Stack *stack) {
	if (strcmp(Stack_Top(stack), "E") != 0){
		Stack_Push(stack, "<", 10);
	}
	else{
		Stack_Pop(stack);
		Stack_Push(stack, "<", 10);
		Stack_Push(stack, "E", 10);
	}
}