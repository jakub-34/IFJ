/*
* Project: Implementacia prekladaca imperativneho jazyka IFJ2024
*
* @author: Jakub Hrdlicka <xhrdli18>
*
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "str_buffer.h"

str_buffer_t *create_str_buffer() {

    str_buffer_t* buffer = (str_buffer_t *)malloc(sizeof(struct str_buffer));
    if (buffer == NULL) {
        fprintf(stderr, "Failed to malloc space for buffer.\n");
        exit(99);
    }

    buffer->size = 0;
    buffer->capacity = 30;
    size_t malcap = sizeof(char) * buffer->capacity;
    buffer->string = (char *)malloc(malcap);
    if (buffer->string == NULL){
        fprintf(stderr, "Failed to malloc space for buffer.\n");
        exit(99);
    }

    buffer->string[0] = '\0';
    return buffer;

}

void append_to_str_buffer(str_buffer_t* buffer, char c) {
    if (buffer->size + 1 >= buffer->capacity) {
        buffer->capacity *= 2;
        buffer->string = (char*)realloc(buffer->string, sizeof(char) * buffer->capacity);
        if (buffer->string == NULL) {
            fprintf(stderr, "Failed to realloc space for buffer.\n");
            exit(99);
        }
    }

    buffer->string[buffer->size] = c;
    buffer->size ++;

    buffer->string[buffer->size] = '\0';
}