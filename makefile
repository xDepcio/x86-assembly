CC=gcc
CFLAGS=-m32 -Wall

all: main.o	swapNumbers.o
	$(CC) $(CFLAGS) main.o swapNumbers.o -o swapNumbers

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

swapNumbers.o: swapNumbers.s
	nasm -f elf swapNumbers.s

clean:
	rm -f *.o
