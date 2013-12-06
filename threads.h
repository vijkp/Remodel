/*****************************************************
* Copyright (c) 2013. All rights reserved.
* Author: Vijay Kumar Pasikanti <vijaykp@cs.umass.edu>
* File : threads.h 
*****************************************************/
#ifndef THREADS_H
#define THREADS_H

#include <stdlib.h>
#include "maindefs.h"

typedef struct thread_data_ {
    pthread_t ptp;
    int tid;
    char message[10];
} thread_data_t;

error_t spawn_threads(int number);
error_t build_thread();
error_t monitor_thread();

#endif /* THREADS_H */
