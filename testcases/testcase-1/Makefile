all: snap

snap: main.o drivers.o filesystem.o network.o sched.o
	gcc main.o drivers.o filesystem.o network.o sched.o -o snap

main.o: main.c 
	gcc -c main.c -o main.o

drivers.o: drivers.c
	gcc -c drivers.c -o drivers.o

filesystem.o: filesystem.c sched.h
	gcc -c filesystem.c -o filesystem.o

network.o: network.c sched.h
	gcc -c network.c -o network.o

sched.o: sched.c
	gcc -c sched.c -o sched.o

clean: 
	rm -rf *.o snap

