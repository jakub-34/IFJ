CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g

SRCS = lexer.c str_buffer.c keyword_check.c syntakticka_analyza.c expression.c btree.c bts_stack.c string_stack.c ast.c codegen.c semantics.c hashtable.c symtable.c symtable_stack.c
OBJS = $(SRCS:.c=.o)
TARGET = test

.PHONY: all clean

all: $(TARGET)

$(TARGET): 
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	$(RM) $(TARGET)