CC=gcc
CFLAGS=-I.

OBJ = getopt.o bin2ihex.o

all : bin2ihex

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

bin2ihex: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean :
	rm -f *.o bin2ihex
