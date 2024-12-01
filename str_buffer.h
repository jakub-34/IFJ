/*
* Project: Implementacia prekladaca imperativneho jazyka IFJ2024
*
* @author: Jakub Hrdlicka <xhrdli18>
*
*/

#ifndef STR_BUFFER_H
#define STR_BUFFER_H

#include <stdlib.h>

typedef struct str_buffer {
    char* string;
    size_t size;
    size_t capacity;
} str_buffer_t;

str_buffer_t* create_str_buffer();
void append_to_str_buffer(str_buffer_t* buffer, char c);


#endif