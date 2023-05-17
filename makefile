# CC=gcc
# CFLAGS=-Wall

# all: main.o	swapNumbers.o
# 	$(CC) $(CFLAGS) main.o swapNumbers.o -o swapNumbers

# main.o: main.c
# 	$(CC) $(CFLAGS) -c main.c -o main.o

# swapNumbers.o: swapNumbers.s
# 	nasm -f elf64 swapNumbers.s

# clean:
# 	rm -f *.o

CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11

# Name of the executable
TARGET = bmp_loader

# Source files
SRCS = main.c

# Object files
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
