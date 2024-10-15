CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g

SRCS = lexer.c str_buffer.c keyword_check.c syntakticka_analyza.c
OBJS = $(SRCS:.c=.o)
TARGET = syn_test

.PHONY: all clean

all: $(TARGET)

$(TARGET): 
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	$(RM) $(TARGET)