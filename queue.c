/*****************************************************
* Copyright (c) 2013. All rights reserved.
* Author: Vijay Kumar Pasikanti <vijaykp@cs.umass.edu>
* File : queue.c 
* Description: Contains queue implementation used in
* this project. 
*****************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "maindefs.h"
#include "misc.h"
#include "queue.h"

queue_t* queue_new(char *name) {
    queue_t *queue;
    queue = (queue_t *)malloc(sizeof(queue_t));
    if (queue == NULL) {
        LOG("error: creation of queue failed.\n");  
        return queue;
    } else {
        strcpy(queue->name, name);
        queue->head = NULL;
        queue->tail = NULL;
        pthread_mutex_init(&queue->mtx, NULL);
    }
    return queue;
}

void queue_free(queue_t *queue) {
    queue_node_t *node;
    while(!queue_is_empty(queue)) {
        node = queue_get_node(queue);
        queue_node_free(node);
    }
    FREE(queue);
}

bool queue_add_node(queue_t* queue, queue_node_t *node) {
    /* check if the queue exists */
    if (queue == NULL) {
        LOG("error: node add to a queue failed: queue doesn't exist.\n");
        return false;
    }
    
    /* check if the node is valid */
    if (node == NULL) {
        LOG("error: node add to a queue failed: node doesn't exist.\n");
        return false;
    }
    if (queue_is_empty(queue)) {
        /* first node to the queue */
        queue->head = node;
        queue->tail = node;
        node->next = NULL;
        node->prev = NULL;
    } else {
        node->next = queue->head;
        queue->head->prev = node;
        queue->head = node;
        node->prev = NULL;
    }
    return true;
}

bool queue_is_empty(queue_t *queue) {
    if (queue == NULL) {
        return true;
    }

    if ((queue->head == NULL) && (queue->tail == NULL)) {
        return true;
    }

    if ((queue->head == NULL) || (queue->head == NULL)) {
        LOG("queue: '%s' has something wrong.\n", queue->name);
        return true;
    }
    return false;
}

queue_node_t* queue_get_node(queue_t *queue) {
    queue_node_t *node = NULL;

    if (queue == NULL) {
        LOG("error: queue doesn't exist.\n");
        return NULL;
    }
    if(queue_is_empty(queue)) {
        return NULL;
    }
    node = queue->tail;
    queue->tail = node->prev;
    if (node->prev) {
        node->prev->next = NULL;
    } else {
        /* head is pointing to the same node we removed */
        queue->head = NULL;
    }
    return node;    
}

void queue_print_all_nodes(queue_t *queue) {
    queue_node_t *node = NULL;
    remodel_node_t *rmnode = NULL;

    if (queue == NULL) {
        LOG("error: queue doesn't exist.\n");
        return;
    }
    LOG("queue: '%s' head %x tail %x ", queue->name, 
            queue->head, queue->tail);
    if (queue_is_empty(queue)) {
        printf("\n");
        return;
    }
    node = queue->head;
    while (node != NULL) {
        rmnode = (remodel_node_t *)node->data;
        printf("%s -> ", rmnode->name);
        node = node->next;
    }
    printf("\n");

}

queue_node_t* queue_node_new() {
    queue_node_t *node = NULL;
    
    node = (queue_node_t *)malloc(sizeof(queue_node_t));
    if (node == NULL) {
        LOG("error: memory allocation failed.\n");
        return node;
    }
    node->prev = NULL;
    node->next = NULL;
    node->data = NULL;
    node->signal = false;
    return node;
}

void queue_node_free(queue_node_t* node){
    FREE(node);
}

queue_node_t* queue_next_node(queue_t* queue, queue_node_t* node) {
    if (queue == NULL) {
        LOG("error: queue doesn't exist.\n");
        return;
    }
    if (node == NULL) {
        LOG("error: given node doesn't exist.\n");
        return;
    }
    return node->next;
}

queue_node_t* queue_prev_node(queue_t* queue, queue_node_t* node) {
    if (queue == NULL) {
        LOG("error: queue doesn't exist.\n");
        return;
    }
    if (node == NULL) {
        LOG("error: given node doesn't exist.\n");
        return;
    }
    return node->prev;
}

