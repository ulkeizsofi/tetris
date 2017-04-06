CC=gcc
PIBUILD?=0
ifeq ($(PIBUILD),1)
CCOPT=-D__PIBUILD__
CFLAGS=-lwiringPi -lncurses
else
CFLAGS=-lncurses
endif
DEPS = shape.h matrixDrv.h

%.o: %.c $(DEPS)
	$(CC) $(CCOPT) -c -o $@ $< $(CFLAGS)
all: Matrix.o shape.o matrixDrv.o
	$(CC) $(CCOPT) -o matrixmake Matrix.o shape.o matrixDrv.o $(CFLAGS)
