#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "maindefs.h"
#include "misc.h"
#include "threads.h"

extern thread_data thread_list[];

error_t spawn_threads(int number) {
	error_t ret;
	int i = 0;

	for (i = 0; i < number; i++) {
		thread_list[i].tid = i+1;
		pthread_create(&thread_list[i].ptp, NULL,
				(void *)&remodel_thread, (void *)&thread_list[i]);
	}

	for (i=0; i < number; i++ ){
		pthread_join(thread_list[i].ptp, NULL);
	}

end:
	return ret;
}

error_t remodel_thread(void *data) {
	thread_data *tdata;
	tdata = (thread_data *)data;
	LOG("started remodel thread tid:%d\n", tdata->tid);
	int i = 0;

	while(1) {
		if (i == tdata->tid) pthread_exit(0);
		sleep(1);
		LOG("thread tid:%d polling after 3000ms.\n", tdata->tid);
		i++;
	}
}
