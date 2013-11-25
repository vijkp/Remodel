
CC = gcc
CFLAGS = -D DEBUG

objs =  main.o misc.o file.o
srcs =  main.c misc.c misc.h file.h file.h

remodel: CFLAGS = -D DEBUG -ggdb -g3
nd: CFLAGS = -D NON_DEBUG -ggdb -g3

all: remodel
nd: remodel-nd
remodel: $(objs)
	$(CC) $(CFLAGS) $(objs) -o remodel.new
	mv -f remodel.new remodel 

# non-debug build
remodel-nd: $(objs)
	$(CC) $(CFLAGS)  $(objs) -o remodel.new
	mv -f remodel.new remodel

clean: 
	rm -fv remodel *.o

# Individual object file and dependencies
main.o: main.c misc.h
	$(CC) $(CFLAGS) -c main.c -o main.o
misc.o: misc.c misc.h
	$(CC) $(CFLAGS) -c misc.c -o misc.o
file.o: file.c misc.h file.h
	$(CC) $(CFLAGS) -c file.c -o file.o

