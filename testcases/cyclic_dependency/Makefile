all: snap

snap: obj/main.o obj/drivers.o obj/filesystem.o obj/network.o obj/sched.o
	gcc obj/main.o obj/drivers.o obj/filesystem.o obj/network.o obj/sched.o -o snap

obj/main.o: main.c 
	gcc -c main.c -o obj/main.o

obj/drivers.o: drivers.c
	gcc -c drivers.c -o obj/drivers.o

obj/filesystem.o: filesystem.c sched.h
	gcc -c filesystem.c -o obj/filesystem.o

obj/network.o: network.c sched.h
	gcc -c network.c -o obj/network.o

obj/sched.o: sched.c
	gcc -c sched.c -o obj/sched.o

clean: 
	rm -rf obj/*.o snap

