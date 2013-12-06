#include <stdio.h>
#include <stdlib.h>
#include "sched.h"

void sched_init() {
	int a;
	int b;
	int c;
	c = a+b;
printf("%s %d: %s()\n", __FILE__, __LINE__, __FUNCTION__);
}

void sched_run() {
	int hello = 20;
	int world = 10;
	int helloworld;
	helloworld = hello + world;
printf("%s %d: %s()\n", __FILE__, __LINE__, __FUNCTION__);
}

void sched_update() {
	int i;
	int j;
	i = 20;
	j = 30;
printf("%s %d: %s()\n", __FILE__, __LINE__, __FUNCTION__);
}

