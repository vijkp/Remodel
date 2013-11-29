#ifndef THREADS_H
#define THREADS_H

#include <stdlib.h>
#include "maindefs.h"

typedef struct thread_data_ {
	pthread_t ptp;
	int tid;
	char message[10];
} thread_data;

error_t spawn_threads(int number);
error_t remodel_thread();

#endif /* THREADS_H */
