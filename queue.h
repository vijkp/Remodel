/*****************************************************
* Copyright (c) 2013. All rights reserved.
* Author: Vijay Kumar Pasikanti <vijaykp@cs.umass.edu>
* File : queue.h 
*****************************************************/
#ifndef QUEUE_H
#define QUEUE_H

#define MAX_FILENAME 256

typedef _Bool bool;

typedef struct queue_node_ {
    struct queue_node_ *prev;
    struct queue_node_ *next;
    void  *data;
    bool  signal;
} queue_node_t;

typedef struct queue_ {
    char name[MAX_FILENAME];
    queue_node_t *head;
    queue_node_t *tail;
    pthread_mutex_t mtx;
} queue_t;

queue_t*    queue_new(char *name);
void        queue_free(queue_t* queue);
bool        queue_add_node(queue_t* queue, queue_node_t *node);
bool        queue_is_empty(queue_t* queue);

void        queue_print_all_nodes(queue_t* queue);

queue_node_t* queue_node_new();
queue_node_t* queue_get_node(queue_t* queue);
queue_node_t* queue_next_node(queue_t* queue, queue_node_t* node);
queue_node_t* queue_prev_node(queue_t* queue, queue_node_t* node);
void          queue_node_free(queue_node_t* node);

#endif /*QUEUE_H*/
/* version v1.1 */
