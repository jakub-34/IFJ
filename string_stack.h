#ifndef _STACK_H_
#define _STACK_H_

#include <stdio.h>
#include <stdbool.h>
#include "lexer.h"

/** ADT zásobník implementovaný v dynamickém poli. */
typedef struct {
	/** Pole pro uložení hodnot. */
	char **string;
	int size;
	/** Index prvku na vrcholu zásobníku. */
	int topIndex;
} Stack;

void Stack_Error( int );

void Stack_Init(Stack *stack);

bool Stack_IsEmpty(const Stack *stack);

bool Stack_IsFull(const Stack *stack);

char *Stack_Top(const Stack *stack);

void Stack_Pop(Stack *stack);

void Stack_Push(Stack *stack, char *string);

void Stack_Dispose(Stack *stack);

int Stack_find(Stack *stack);

char *Stack_rule_str(Stack *stack);

void Stack_insert_str(Stack *stack);

char *Stack_extract_str(Stack *stack);

#endif

