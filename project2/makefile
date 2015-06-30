CC=gcc
CFLAGS=-c -Wall

all: project2

project2: project2.o
	$(CC) project2.o -o project2
	./project2

project2.o: project2.c
	$(CC) $(CFLAGS) project2.c

clean:
	rm -rf *o project2
