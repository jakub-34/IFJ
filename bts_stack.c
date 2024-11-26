#include "./bts_stack.h"
#include <stdlib.h>
#include <stdbool.h>


void bts_Stack_Init(bts_Stack *bts_Stack, size_t size) {
	bts_Stack->node = (bst_node_t **)malloc(size * sizeof(bst_node_t *));
	bts_Stack->topIndex = -1;
	bts_Stack->size = 10;
}

bool bts_Stack_IsEmpty(const bts_Stack *bts_Stack) {
	return bts_Stack->topIndex == -1;
}

bool bts_Stack_IsFull(const bts_Stack *bts_Stack) {
	return bts_Stack->size == bts_Stack->topIndex + 1;
}

bst_node_t *bts_Stack_Top(const bts_Stack *bts_Stack) {
	if (bts_Stack_IsEmpty(bts_Stack)){
		return NULL;
	}
	return bts_Stack->node[bts_Stack->topIndex];
}

void bts_Stack_Pop( bts_Stack *bts_Stack ) {
	if (bts_Stack->topIndex != -1){
		bts_Stack->topIndex --;
	}
}

void bts_Stack_Resize(bts_Stack *bts_Stack) {
	// printf("Resizing stack\n");
    bts_Stack->size *= 2;
    bts_Stack->node = (bst_node_t **)realloc(bts_Stack->node, bts_Stack->size * sizeof(bst_node_t *));
    if(bts_Stack->node == NULL){
        fprintf(stderr, "Error: realloc failed\n");
        exit(99);
    }
}

void bts_Stack_Push(bts_Stack *bts_Stack, bst_node_t *node) {
	if (bts_Stack_IsFull(bts_Stack)){
		bts_Stack_Resize(bts_Stack);
	}

	bts_Stack->topIndex++;
	bts_Stack->node[bts_Stack->topIndex] = node;
}

void bts_Stack_Dispose( bts_Stack *bts_Stack ) {
	bts_Stack->topIndex = -1;
	free(bts_Stack->node);
	bts_Stack->node = NULL;
}

