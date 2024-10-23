#include "./bts_stack.h"
#include <stdlib.h>
#include <stdbool.h>


void bts_Stack_Init(bts_Stack *bts_Stack, size_t size) {
	bts_Stack->node = (bst_node_t **)malloc(size * sizeof(bst_node_t *));
	bts_Stack->topIndex = -1;
}

bool bts_Stack_IsEmpty(const bts_Stack *bts_Stack) {
	return bts_Stack->topIndex == -1;
}

bool bts_Stack_IsFull(const bts_Stack *bts_Stack, int size) {
	return size == bts_Stack->topIndex + 1;
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

void bts_Stack_Push(bts_Stack *bts_Stack, bst_node_t *node, size_t size) {
	if (bts_Stack_IsFull(bts_Stack, size)){
		return;
	}

	bts_Stack->topIndex ++;
	bts_Stack->node[bts_Stack->topIndex] = node;
}


void bts_Stack_Dispose( bts_Stack *bts_Stack ) {
	bts_Stack->topIndex = -1;
	free(bts_Stack->node);
	bts_Stack->node = NULL;
}

