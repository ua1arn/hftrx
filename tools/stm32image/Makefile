CC=gcc
CFLAGS=-I.

OBJ = getopt.o stm32image.o

all : stm32image

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

stm32image: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean :
	rm -f *.o stm32image
