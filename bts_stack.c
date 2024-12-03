/*
* Project: Implementacia prekladaca imperativneho jazyka IFJ2024
*
* @author: Rebeka Tydorova <xtydor01>
*
*/
#include "bts_stack.h"
#include <stdlib.h>
#include <stdbool.h>

//initialize the binary tree stack
void bts_Stack_Init(bts_Stack *bts_Stack) {
	bts_Stack->size = 50;
	bts_Stack->node = (bst_node_t **)malloc(bts_Stack->size * sizeof(bst_node_t *));
	if(bts_Stack->node == NULL){
		fprintf(stderr, "Error: malloc failed\n");
		exit(99);
	}
	bts_Stack->topIndex = -1;
}

//check if the stack is empty
bool bts_Stack_IsEmpty(const bts_Stack *bts_Stack) {
	return bts_Stack->topIndex == -1;
}

//check if the stack is full
bool bts_Stack_IsFull(const bts_Stack *bts_Stack) {
	return bts_Stack->size == bts_Stack->topIndex + 1;
}

//return the top element of the stack
bst_node_t *bts_Stack_Top(const bts_Stack *bts_Stack) {
	if (bts_Stack_IsEmpty(bts_Stack)){
		return NULL;
	}
	return bts_Stack->node[bts_Stack->topIndex];
}

//remove the top element of the stack
void bts_Stack_Pop( bts_Stack *bts_Stack ) {
	if (bts_Stack->topIndex != -1){
		bts_Stack->topIndex --;
	}
}

//resize stack
void bts_Stack_Resize(bts_Stack *bts_Stack) {
    bts_Stack->size *= 2;
    bts_Stack->node = (bst_node_t **)realloc(bts_Stack->node, bts_Stack->size * sizeof(bst_node_t *));
    if(bts_Stack->node == NULL){
        fprintf(stderr, "Error: realloc failed\n");
        exit(99);
    }
}

//push a new element onto the stack
void bts_Stack_Push(bts_Stack *bts_Stack, bst_node_t *node) {
	if (bts_Stack_IsFull(bts_Stack)){
		bts_Stack_Resize(bts_Stack);
	}

	bts_Stack->topIndex++;
	bts_Stack->node[bts_Stack->topIndex] = node;
}

//dispose stack
void bts_Stack_Dispose( bts_Stack *bts_Stack ) {
	bts_Stack->topIndex = -1;
	free(bts_Stack->node);
	bts_Stack->node = NULL;
}

