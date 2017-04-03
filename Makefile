CC=gcc
CFLAGS=-I. -lwiringPi
DEPS = shape.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
matrixmake: Matrix.o shape.o 
	gcc -o matrixmake Matrix.o shape.o -I. -lwiringPi
