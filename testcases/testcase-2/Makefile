
CC = gcc

objs =  main.o misc.o file.o maindefs.o md5hash.o threads.o queue.o
srcs =  main.c main.h misc.c misc.h file.c file.h maindefs.c maindefs.h md5hash.c md5hash.h queue.c queue.h
remodel: CFLAGS = -D DEBUG -ggdb -g3
remodel-nd: CFLAGS = -D NON_DEBUG 

all:  d
d: remodel
nd: remodel-nd
remodel: $(objs)
	$(CC) $(CFLAGS) $(objs)  -lssl -lcrypto -lpthread -o remodel.new
	mv -f remodel.new remodel 

# non-debug build
remodel-nd: $(objs)
	$(CC) $(CFLAGS)  $(objs) -lssl -lcrypto -lpthread -o remodel.new
	mv -f remodel.new remodel

clean: 
	rm -fv remodel *.o

# Individual object file and dependencies
main.o: main.c main.h misc.h maindefs.h threads.h
	$(CC) $(CFLAGS) -c main.c -o main.o
misc.o: misc.c misc.h maindefs.h
	$(CC) $(CFLAGS) -c misc.c -o misc.o
file.o: file.c misc.h file.h maindefs.h
	$(CC) $(CFLAGS) -c file.c -o file.o
maindefs.o: maindefs.c maindefs.h misc.h
	$(CC) $(CFLAGS) -c maindefs.c -o maindefs.o
md5hash.o: md5hash.c maindefs.h misc.h
	$(CC) $(CFLAGS) -lssl -lcrypto -c md5hash.c -o md5hash.o 
threads.o: threads.c maindefs.h threads.h
	$(CC) $(CFLAGS) -c threads.c -o threads.o 
queue.o: queue.c queue.h maindefs.h
	$(CC) $(CFLAGS) -c queue.c -o queue.o
