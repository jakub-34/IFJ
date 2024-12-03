/*
* Project: Implementacia prekladaca imperativneho jazyka IFJ2024
*
* @author: Rebeka Tydorova <xtydor01>
*
*/
#ifndef _bts_Stack_H_
#define _bts_Stack_H_

#include <stdio.h>
#include <stdbool.h>
#include "btree.h"

//stack
typedef struct {
	bst_node_t **node;
	int size;
	int topIndex;
} bts_Stack;

//stack error
void bts_Stack_Error( int );

//initialize the binary tree stack
void bts_Stack_Init(bts_Stack *bts_Stack);

//check if the stack is empty
bool bts_Stack_IsEmpty(const bts_Stack *bts_Stack);

//check if the stack is full
bool bts_Stack_IsFull(const bts_Stack *bts_Stack);

//return the top element of the stack
bst_node_t *bts_Stack_Top(const bts_Stack *bts_Stack);

//remove the top element of the stack
void bts_Stack_Pop(bts_Stack *bts_Stack);

//push a new element onto the stack
void bts_Stack_Push(bts_Stack *bts_Stack, bst_node_t *node);

//dispose stack
void bts_Stack_Dispose(bts_Stack *bts_Stack);

#endif

