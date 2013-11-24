
CC = gcc
CFLAGS = -D DEBUG 

objs =  obj/main.o obj/misc.o obj/file.o
srcs =  main.c misc.c misc.h file.h file.h

remodel: CFLAGS = -D DEBUG
nd: CFLAGS = -D NON_DEBUG

all: remodel
nd: remodel-nd
remodel: $(objs)
	$(CC) -D DEBUG $(objs) -o remodel.new
	mv -f remodel.new remodel 

# non-debug build
remodel-nd: $(objs)
	$(CFLAGS = -D NON_DEBUG )
	$(CC) $(objs) -o remodel.new
	mv -f remodel.new remodel

clean: 
	rm -fv remodel *.o

# Individual object file and dependencies
obj/main.o: main.c misc.h
	$(CC) -c main.c -o obj/main.o
obj/misc.o: misc.c misc.h
	$(CC) -c misc.c -o obj/misc.o
obj/file.o: file.c misc.h file.h
	$(CC) -c file.c -o obj/file.o

