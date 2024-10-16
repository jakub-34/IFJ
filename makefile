CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g

SRCS = lexer.c str_buffer.c keyword_check.c hashtable.c symtable_stack.c
OBJS = $(SRCS:.c=.o)
TARGET = symtable_test

.PHONY: all clean

all: $(TARGET)

$(TARGET): 
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	$(RM) $(TARGET)