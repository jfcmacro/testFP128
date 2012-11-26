CFLAGS=-g
LDFLAGS=-g

all: testFP128

testFP128: main.o OperBigRealNumber.o 
	gcc -g -o $@ main.o OperBigRealNumber.o

clean:
	rm -f *.o *.c~ *.h~ *~ testFP128


