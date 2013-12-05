#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "maindefs.h"
#include "misc.h"
#include "threads.h"
#include "queue.h"

extern queue_t  *dispatch_queue;
extern queue_t	*monitor_queue; 
extern queue_t  *response_queue;

extern thread_data_t thread_list[];

error_t spawn_threads(int number) {
	error_t ret = SUCCESS;
	int i = 0;

	for (i = 0; i < number; i++) {
		thread_list[i].tid = i+1;
		pthread_create(&thread_list[i].ptp, NULL,
				(void *)&build_thread, (void *)&thread_list[i]);
	}

	thread_list[number].tid = number+1;
	pthread_create(&thread_list[number].ptp, NULL,
				(void *)&monitor_thread, (void *)&thread_list[number]);

	/* May not need this portion */
	//for (i=0; i < number; i++ ){
	//	pthread_join(thread_list[i].ptp, NULL);
	//}
end:
	return ret;
}


/* 
 * All these threads run the following algorithim
 * 1. Obtain mutex for dispatch_queue
 * 2. Check if the queue is non-empty. If the queue is empty
 * 3. Pick the first element at the from the queue
 * 4. Release the mutex for dispatch_queue
 * 5. Process the node and run the build command.
 * 6. Once the build command is run.
 * 7. Update the build_state in the node.
 * 8. Obtain lock for response_queue. 
 * 9. Add the node to response_queue and release the lock.
 */ 
error_t build_thread(void *data) {
	thread_data_t  *tdata = NULL;
	queue_node_t   *qnode = NULL;
	target_t       *target = NULL;
	remodel_node_t *rmnode = NULL;
	char		   *command = NULL;
	int				tid;
	
	tdata = (thread_data_t *)data;
	if (tdata == NULL) {
		pthread_exit(NULL);
	}
	tid = tdata->tid;
	while(1) {
		rmnode = NULL;
		target = NULL;
		qnode = NULL;
		
		/* Get a node from dispatch_queue */
		pthread_mutex_lock(&dispatch_queue->mtx);
		qnode = queue_get_node(dispatch_queue);
		pthread_mutex_unlock(&dispatch_queue->mtx);

		if (qnode == NULL) {
			continue;
		}
		/* Do the processing on the node. Run the build command */
		rmnode = (remodel_node_t *)qnode->data;
		assert(rmnode->type == DP_TARGET);
		target = rmnode->target;
		assert(target->build_state == RM_BUILD_ONQ);
		
		pthread_mutex_lock(&target->mtx);
		target->build_state = RM_BUILD_RUNNING;
		pthread_mutex_unlock(&target->mtx);

		LOG("thread %d running \"%s\"\n", tid, target->command);
		command = target->command;
				
		/* Run the command now!  */
		system(command);
			
		/* Change the state of the build */
		pthread_mutex_lock(&target->mtx);
		target->build_state = RM_BUILD_DONE;
		pthread_mutex_unlock(&target->mtx);

		/* Done with the command. Now put the node in response_queue */
		pthread_mutex_lock(&response_queue->mtx);
		queue_add_node(response_queue, qnode);
		pthread_mutex_unlock(&response_queue->mtx);
	}
}

error_t monitor_thread(void *data) {
	thread_data_t  *tdata = NULL;
	queue_node_t   *qnode = NULL;
	remodel_node_t *rmnode = NULL;
	target_t       *target = NULL;
	int				tid;
	build_state_t	build_state;
	
	tdata = (thread_data_t *)data;
	if (tdata == NULL) {
		pthread_exit(NULL);
	}
	tid = tdata->tid;
	while(1) {
		rmnode = NULL;
		target = NULL;
		qnode = NULL;
		build_state = RM_BUILD_RUNNING;
		/* Get a node from monitor_queue */
		pthread_mutex_lock(&monitor_queue->mtx);
		qnode = queue_get_node(monitor_queue);
		pthread_mutex_unlock(&monitor_queue->mtx);

		if (qnode == NULL) {
			continue;
		}

		assert(qnode != NULL);
		rmnode = (remodel_node_t *)qnode->data;
		assert(rmnode->type == DP_TARGET);
		target = rmnode->target;
	
		pthread_mutex_lock(&target->mtx);
		build_state = target->build_state;
		pthread_mutex_unlock(&target->mtx);

		if (build_state == RM_BUILD_DONE) {
			/* Build done for the node. Put into response_queue */
			pthread_mutex_lock(&response_queue->mtx);
			queue_add_node(response_queue, qnode);
			pthread_mutex_unlock(&response_queue->mtx);
		} else {
			pthread_mutex_lock(&monitor_queue->mtx);
			queue_add_node(monitor_queue, qnode);
			pthread_mutex_unlock(&monitor_queue->mtx);
			continue;
		}
	}
}


