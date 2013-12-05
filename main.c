/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Local headers */
#include "maindefs.h"
#include "misc.h"
#include "file.h"
#include "md5hash.h"
#include "threads.h"
#include "queue.h"

/* XXX Thigns to do 
 * 1. multiple targets
 * 2. detect cyclic dependencies
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
    char         target_name[MAX_FILENAME] = "DEFAULT";
    int          total_threads = BUILD_THREADS;
    target_t     *target = NULL;
    queue_node_t *queue_node = NULL;
    int          i = 0;

    /* Argumements check */
    if (argc == 1) {
        LOG("using the default target 'DEFAULT'\n");
    } else if (argc == 2) {
        strcpy(target_name, argv[1]);
        LOG("using the given target '%s'\n", target_name);
    } else {
        LOG("error: invalid number of arguments (%d)\n", (argc - 1));
        print_usage();
        goto end;
    }

    target_head  = new_target_node();
    srcfile_head = new_src_node();
    remodel_head = new_remodel_node();
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
    if ( target == NULL) {
        LOG("error: nothing to build for the target '%s'\n",
                target_name);  
        goto end;
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

    /* Initialize queues here */
    dispatch_queue = queue_new("dispatch queue");
    monitor_queue  = queue_new("monitor queue");
    response_queue  = queue_new("response queue");

    /* Start all threads (build threads and monitor thread) */
    result = spawn_threads(total_threads);
    if (result != SUCCESS) {
        goto end;
    }

    /* Shedule builds to multiple threads */
    main_initiate_builds(remodel_head, target_name);
end:
    //XXX   clean_up_queues();
    //XXX    free_target_head();
    //XXX    free_srcfile_head();
    //XXX   free_remodel_head();
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
        assert(rmnode->type == DP_TARGET);
        
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
        assert(rmnode->type == DP_TARGET);
        target = rmnode->target;
        assert(target->build_state == RM_BUILD_DONE);
        if (strcmp(target_name, target->name) == 0) {
            /* Done with the build. Break the loop */
            LOG("build completed successfully :)\n");
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

