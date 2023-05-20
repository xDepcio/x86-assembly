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
##############################
# CC = gcc
# CFLAGS = -Wall -Wextra -pedantic -std=c11

# # Name of the executable
# TARGET = bmp_loader

# # Source files
# SRCS = main.c

# # Object files
# OBJS = $(SRCS:.c=.o)

# all: $(TARGET)

# $(TARGET): $(OBJS)
# 	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# %.o: %.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# clean:
# 	rm -f $(OBJS) $(TARGET)
##############################
# CC = gcc
# CFLAGS = -Wall -Wextra -pedantic -std=c11
# LDFLAGS = -lm

# # Name of the executable
# TARGET = main

# # Source files
# SRCS = main.c

# # Object files
# OBJS = $(SRCS:.c=.o)

# all: $(TARGET)

# $(TARGET): $(OBJS)
# 	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

# %.o: %.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# clean:
# 	rm -f $(OBJS) $(TARGET)
####################################
# CC = gcc
# CFLAGS = -Wall -Wextra -std=c99

# LIBS = -lm -lallegro -lallegro_image -lallegro_memfile

# SRCS = main2.c transformf.s
# OBJS = $(SRCS:.c=.o)

# TARGET = program

# $(TARGET): $(OBJS)
# 	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

# %.o: %.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# %.o: %.s
# 	$(CC) $(CFLAGS) -c $< -o $@

# clean:
# 	rm -f $(OBJS) $(TARGET)
####################################
# CC = gcc
# CFLAGS = -Wall -Wextra -std=c99

# LIBS = -lm -lallegro -lallegro_image -lallegro_memfile

# SRCS = main.c transformf.s
# OBJS = $(SRCS:.c=.o)

# TARGET = program

# $(TARGET): $(OBJS)
# 	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

# %.o: %.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# %.o: %.s
# 	nasm -f elf64 $< -o $@

# clean:
# 	rm -f $(OBJS) $(TARGET)
##################################
CC=gcc
CFLAGS=-Wall -Wextra
LIBS = -lm -lallegro -lallegro_image -lallegro_memfile

all: main2.o	transformf.o
	$(CC) $(CFLAGS) main2.o transformf.o $(LIBS) -o fun

main2.o: main2.c
	$(CC) $(CFLAGS) $(LIBS) -c main2.c -o main2.o

transformf.o: transformf.s
	nasm -f elf64 transformf.s

clean:
	rm -f *.o
