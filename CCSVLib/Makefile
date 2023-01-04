CC=g++
CFLAGS=-std=gnu++20
OBJS=Parser.o FileMapper.o
HEADERS=Parser.h FileMapper.h

all: libCCSVLib.a

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<
libCCSVLib.a: $(OBJS) 
	ar rcs libCCSVLib.a $(OBJS)
clean:
	rm $(OBJS)
	rm libCCSVLib.a
