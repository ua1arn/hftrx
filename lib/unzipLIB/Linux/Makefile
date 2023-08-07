CFLAGS=-D__LINUX__ -Wall -O2 
LIBS = 

all: unzip_test

unzip_test: main.o unzip.o adler32.o crc32.o infback.o inffast.o inflate.o inftrees.o zutil.o
	$(CC) main.o unzip.o adler32.o crc32.o infback.o inffast.o inflate.o inftrees.o zutil.o $(LIBS) -o unzip_test 

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

unzip.o: ../src/unzip.c ../src/unzip.h
	$(CC) $(CFLAGS) -c ../src/unzip.c

adler32.o: ../src/adler32.c
	$(CC) $(CFLAGS) -c ../src/adler32.c

crc32.o: ../src/crc32.c
	$(CC) $(CFLAGS) -c ../src/crc32.c

infback.o: ../src/infback.c
	$(CC) $(CFLAGS) -c ../src/infback.c

inffast.o: ../src/inffast.c
	$(CC) $(CFLAGS) -c ../src/inffast.c

inflate.o: ../src/inflate.c
	$(CC) $(CFLAGS) -c ../src/inflate.c

inftrees.o: ../src/inftrees.c
	$(CC) $(CFLAGS) -c ../src/inftrees.c

zutil.o: ../src/zutil.c
	$(CC) $(CFLAGS) -c ../src/zutil.c

clean:
	rm -rf *.o unzip_test
