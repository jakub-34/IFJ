CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g

SRCS = hashtable.c symtable_stack.c symtable.c symtable_test.c
OBJS = $(SRCS:.c=.o)
TARGET = symtable_test

.PHONY: all clean

all: $(TARGET)

$(TARGET): 
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	$(RM) $(TARGET)