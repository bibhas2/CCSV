CC=g++
CFLAGS=-std=gnu++20 -I../CCSVLib
OBJS=main.o
HEADERS=

all: ccsv-lib ccsv-test
	@./ccsv-test
%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<
ccsv-lib:
	@make -C ../CCSVLib
ccsv-test: $(OBJS) ../CCSVLib/libCCSVLib.a $(HEADERS)
	$(CC) -L../CCSVLib -o ccsv-test $(OBJS) -lCCSVLib
clean:
	rm $(OBJS)
	rm ccsv-test