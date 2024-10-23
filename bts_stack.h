#ifndef _bts_Stack_H_
#define _bts_Stack_H_

#include <stdio.h>
#include <stdbool.h>
#include "./btree.h"

/** ADT zásobník implementovaný v dynamickém poli. */
typedef struct {
	/** Pole pro uložení hodnot. */
	bst_node_t **node;
	/** Index prvku na vrcholu zásobníku. */
	int topIndex;
} bts_Stack;

void bts_Stack_Error( int );

void bts_Stack_Init(bts_Stack *bts_Stack, size_t size);

bool bts_Stack_IsEmpty(const bts_Stack *bts_Stack);

bool bts_Stack_IsFull(const bts_Stack *bts_Stack, int size);

bst_node_t *bts_Stack_Top(const bts_Stack *bts_Stack);

void bts_Stack_Pop(bts_Stack *bts_Stack);

void bts_Stack_Push(bts_Stack *bts_Stack, bst_node_t *node, size_t size);

void bts_Stack_Dispose(bts_Stack *bts_Stack);

#endif

