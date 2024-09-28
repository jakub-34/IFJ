CC = gcc
CFLAGS = -Wall -Wextra -pedantic

SRCS = lexer.c str_buffer.c
OBJS = $(SRCS:.c=.o)
TARGET = lexer_test

.PHONY: all clean

all: $(TARGET)

$(TARGET): 
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	$(RM) $(TARGET)