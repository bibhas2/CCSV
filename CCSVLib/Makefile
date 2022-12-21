CC=g++
CFLAGS=-std=gnu++20
OBJS=Parser.o StringReader.o MemoryMappedReader.o
HEADERS=Parser.h StringReader.h MemoryMappedReader.h

all: libCCSVLib.a

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<
libCCSVLib.a: $(OBJS) 
	ar rcs libCCSVLib.a $(OBJS)
clean:
	rm $(OBJS)
	rm libCCSVLib.a