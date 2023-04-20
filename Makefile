# variables
CC=gcc
CFLAGS=-std=c11 -Wall -g -Wno-vla-parameter -Werror 
LDLIBS=-lm -lcurses
OUTPUT=agario

# targets
all: $(OUTPUT)

$(OUTPUT): main.o agario.o name.o 
	cppcheck --enable=performance,unusedFunction --error-exitcode=1 *.c
	$(CC) $(CFLAGS) agario.o main.o name.o $(LDLIBS) -o $(OUTPUT)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c $(LDLIBS) -o main.o

agario.o: agario.c agario.h config.h 
	$(CC) $(CFLAGS) -c agario.c $(LDLIBS) -o agario.o
	
name.o: name.c name.h
	$(CC) $(CFLAGS) -c name.c $(LDLIBS) -o name.o

# remove compiled files
clean:
	rm -rf $(OUTPUT) *.o