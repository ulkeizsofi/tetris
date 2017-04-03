CC=gcc
CFLAGS=-lwiringPi
DEPS = shape.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
matrixmake: Matrix.o shape.o 
	$(CC) -o matrixmake Matrix.o shape.o $(CFLAGS)
