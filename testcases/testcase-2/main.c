/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

/* Local headers */
#include "maindefs.h"
#include "misc.h"
#include "file.h"
#include "md5hash.h"
#include "threads.h"
#include "queue.h"
#include "main.h"

/* XXX Thigns to do 
 * XXX 1. multiple targets
 * XXX 2. detect cyclic dependencies
 */

/* Config macros (used only in this file) */
#define BUILD_THREADS 3

/* Globals */
target_t        *target_head;
srcfile_t       *srcfile_head;
remodel_node_t  *remodel_head;
thread_data_t   thread_list[BUILD_THREADS + 1]; /* +1 is for the monitor thread */ 
queue_t         *dispatch_queue, *monitor_queue, *response_queue;

int main(int argc, char **argv) {
    error_t      result;
    double       time_start = 0, time_end = 0, time_taken = 0;
    char         target_name[MAX_FILENAME] = "DEFAULT";
    int          total_threads = BUILD_THREADS;
    target_t     *target = NULL;
    queue_node_t *queue_node = NULL;
    int          i = 0;
        
    /* For performance calculations */
    time_start = get_time_in_sec();

    /* Argumements check */
    if (argc == 1) {
        LOG("building the default target 'DEFAULT'\n");
    } else if (argc == 2) {
        strcpy(target_name, argv[1]);
        DEBUG_LOG("building the given target '%s'\n", target_name);
    } else {
        LOG("error: invalid number of arguments (%d)\n", (argc - 1));
        print_usage();
        goto far_end;
    }

    /* Initialize globals */
    target_head  = new_target_node();
    srcfile_head = new_src_node();
    remodel_head = new_remodel_node();
    for (i = 0; i < (total_threads+1); i++) {
        thread_list[i].tid = -1; 
    }
    if (!(target_head && srcfile_head && remodel_head)) {
        goto end; /* error: system out of memory */
    }

    /* Read and process remodelfile */
    result = file_process_remodelfile();
    if (result != SUCCESS) {
        goto end;
    }

    /* Check if the given target is available in the file */
    target = file_get_target(target_name);
    if (target == NULL) {
        LOG("error: given target '%s' doesn't exist.\n",
                target_name);  
        goto end;
    } else {
        /* check if there is just a command to run */
        if ((target->dp_head == NULL) &&
                (target->command[0] != '\0')) {
            LOG("running the command \"%s\"\n",
                    target->command);
            system(target->command);
            LOG("done!\n");
            goto end;
        }
    }

    /* Calculate MD5s of all the source files. Can be done in parallel? */
    result = md5_calculate_for_sources();
    if (result != SUCCESS) {
        goto end;
    }

    /* Compare and load, sourcefile md5 info into srcfile_t nodes*/
    result = md5_load_from_file();
    if (result != SUCCESS) {
        goto end;
    }

    /* Save <sourcefile> <md5> in a temp file in .remodel/ folder */
    result = md5_save_md5_hashes();
    if (result != SUCCESS) {
        goto end;
    }
    
    /* Initialize queues and threads here */
    dispatch_queue = queue_new("dispatch queue");
    monitor_queue  = queue_new("monitor queue");
    response_queue  = queue_new("response queue");
    result = spawn_threads(total_threads);
    if (result != SUCCESS) {
        goto end;
    }

    /* Create a dependency graph for the given target */
    result = file_create_dependency_graph(target);
    if (result != SUCCESS) {
        goto end;
    }

    /* Mark all the targets that need build */
    result = file_mark_all_targets_for_build(remodel_head);
    if (result == 0) {
        LOG("nothing new to build for the target '%s'.\n", target);
        goto end;
    }

    /* Clean the dependency tree */
    file_cleanup_nodes_for_unchanged_files(remodel_head);

    /* Shedule builds to multiple threads */
    main_initiate_builds(remodel_head, target_name);
    LOG("build done!\n");
end:
    LOG("cleaning up\n");
    /* Clean up the temporary data */
    send_killsignal_to_threads(total_threads);
    clean_queues();
    clean_target_list();
    clean_srcfile_list();
    time_end = get_time_in_sec();
    print_time_taken(time_start, time_end);
far_end:
    return SUCCESS;
}

void main_initiate_builds(remodel_node_t *rmnode, char *target) {
    main_dispatch_all_leaf_nodes(rmnode);
    
    /* Once the dispatch is done. Now check the response queue */
    main_process_response_queue(target);
}

void main_dispatch_all_leaf_nodes(remodel_node_t *rmnode) {
    queue_node_t  *queue_node = NULL;
    build_state_t build_state;
    int i;
    
    assert(rmnode != NULL);
    if (rmnode->child_nodes == 0) {
        assert(rmnode->type == RM_TYPE_TARGET);
        
        /* Change the build state */
        build_state = rmnode->target->build_state;
        if (build_state == RM_BUILD_READY) {
            rmnode->target->build_state = RM_BUILD_ONQ;
        }
        /* Create a new node and add it to a Queue */
        queue_node = queue_node_new();
        queue_node->data = (void *)rmnode;
        if (build_state == RM_BUILD_READY) {
            DISPATCH_NODE(queue_node);
        } else {
            /* this target is already on the queue. Monitor it. */
            MONITOR_NODE(queue_node);
        }
        return;
    }
    /* Find leafs in the child nodes */
    for (i = 0; i < rmnode->child_nodes; i++) {
        main_dispatch_all_leaf_nodes(rmnode->children[i]);
    }
}

void main_process_response_queue(char *target_name) {
    queue_node_t   *queue_node = NULL;
    remodel_node_t *rmnode = NULL;
    remodel_node_t *parent = NULL;
    target_t       *target = NULL;

    while (1) {
        rmnode = NULL;
        parent = NULL;
        target = NULL;
        queue_node = NULL;

        pthread_mutex_lock(&response_queue->mtx);
        queue_node = queue_get_node(response_queue);
        pthread_mutex_unlock(&response_queue->mtx);

        if (queue_node == NULL) {
            continue;
        }
        assert(queue_node != NULL);
        rmnode = (remodel_node_t *)queue_node->data;
        assert(rmnode->type == RM_TYPE_TARGET);
        target = rmnode->target;
        assert(target->build_state == RM_BUILD_DONE);
        if (strcmp(target_name, target->name) == 0) {
            /* Done with the build. Clean up and break the loop */
            DEBUG_LOG("build completed successfully :)\n");
            FREE(queue_node);
            FREE(rmnode);
            break;
        }

        /* Get the parent node and update the child_node count */
        assert(rmnode->parent != NULL);
        parent = rmnode->parent;
        parent->child_nodes--;

        /* Free the current queue node and remodel node */
        FREE(rmnode);
        FREE(queue_node);

        /* If all child nodes are built dispatch the parent */
        rmnode = parent;
        assert(rmnode->child_nodes >= 0);
        if (rmnode->child_nodes == 0) {
            queue_node = queue_node_new();
            queue_node->data = (void *)rmnode;
            if (rmnode->target->build_state == RM_BUILD_READY) {
                rmnode->target->build_state = RM_BUILD_ONQ;
                DISPATCH_NODE(queue_node);
            } else {
                /* this target is already on the queue. Monitor it. */
                MONITOR_NODE(queue_node);
            }
        }
    }    
}

void clean_target_list() {
    target_t *tmp;
    target_t *current;
    dependency_t *dtmp;
    dependency_t *dcurrent;
    
    if (target_head == NULL) {
        return;
    }
    current = target_head;
    while (current != NULL) {
        tmp = current->next;
        if (current->dp_head != NULL) {
            dcurrent = current->dp_head;
            while (dcurrent !=NULL) {
                dtmp = dcurrent->next;
                DEBUG_LOG("freeing depdendency %s\n", dcurrent->name);
                FREE(dcurrent);
                dcurrent = dtmp;
            }
        }
        DEBUG_LOG("freeing target %s\n", current->name);
        current = tmp;
    }
    DEBUG_LOG("freeing target_head\n");
    FREE(target_head);
}

void clean_srcfile_list() {
    srcfile_t *tmp;
    srcfile_t *current;

    if (srcfile_head == NULL) {
        return;
    }
    current = srcfile_head->next;
    while (current != NULL) {
        tmp = current->next;
        DEBUG_LOG("freeing srcfile %s\n", current->name); 
        FREE(current);
        current = tmp;
    }
    DEBUG_LOG("freeing srcfile_head\n");
    FREE(srcfile_head);
}

void clean_queues() {
    FREE(dispatch_queue);
    FREE(monitor_queue); 
    FREE(response_queue);
}

double get_time_in_sec() {
    double time_sec;
    struct timeval time;
    gettimeofday(&time, NULL);
    time_sec = time.tv_sec*1000 + (time.tv_usec/1000.0);
    return time_sec;
}

void send_killsignal_to_threads(int total_threads) {
    int threads_alive = 0;
    queue_node_t *queue_node = NULL;
    int i;

    for (i = 0; i < (total_threads); i++) {
        if (thread_list[i].tid != -1) {
            threads_alive++;
            /* for each alive thread send a signal to kill */
            queue_node = queue_node_new();
            queue_node->signal = true;
            DISPATCH_NODE(queue_node);
        }
    }
    /* If no threads are created then return*/
    if (threads_alive == 0) {
        return;
    }
    /* send one to monitor thread */
    threads_alive++;
    queue_node = queue_node_new();
    queue_node->signal = true;
    MONITOR_NODE(queue_node);

    /* now wait for the threads to respond */
    while (threads_alive > 0) {
        pthread_mutex_lock(&response_queue->mtx);
        queue_node = queue_get_node(response_queue);
        pthread_mutex_unlock(&response_queue->mtx);
        if (queue_node == NULL) {
            continue;
        }
        if (queue_node->signal) {
            /* free the qnode and reduce the live thread count */
            DEBUG_LOG("kill signal ack received.\n");
            FREE(queue_node);
            threads_alive--;
        }
    }
}
