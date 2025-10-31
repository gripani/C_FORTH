CC = gcc
CFLAGS = -Wall -O3
OBJ = main.o forth.o

build: $(OBJ)
	$(CC) $(CFLAGS) -o forth_compiler $(OBJ)

main.o: src/main.c src/forth.h
	$(CC) $(CFLAGS) -c src/main.c

forth.o: src/forth.c src/forth.h
	$(CC) $(CFLAGS) -c src/forth.c

clean:
	rm *.o
