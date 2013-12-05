#ifndef MAINDEFS_H
#define MAINDEFS_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "misc.h"

#ifndef DEBUG
    #define NDEBUG
#endif

struct dependency_;
struct target_;
struct srcfile_;

#define MAX_FILENAME 256
#define MD5_HASHSIZE 32
#define RM_APP_DIR   ".remodel"
#define RM_MD5HASH_FILE ".remodel/md5hashes"
#define RM_MD5HASH_BKPFILE    ".remodel/md5hashes.bkp"

typedef _Bool bool;

typedef enum {
    DP_UNKNOWN,     /* Neither target nor source */
    DP_TARGET,      /* Dependency is a target */
    DP_SRC,         /* Dependency is a src file */
    DP_HEADER       /* Header file */
} dp_type_t; 

typedef enum {
    RM_BUILD_DONE,      /* target/file is built. No build needed */
    RM_BUILD_READY,     /* target/file is ready to build. Pick and build */
    RM_BUILD_ONQ,       /* target/file in the queue for build threads to pick up */
    RM_BUILD_RUNNING    /* target/file is being built */
} build_state_t;

typedef struct srcfile_ { 
    char name[MAX_FILENAME];    /* Holds file path to a source file */
    char md5hash[MD5_HASHSIZE+1];   /* md5 hash string 128 bits */
    bool md5_present;               /* md5 hash calculation needed */
    bool md5_changed;           /* md5 calc needed or not */    
    struct srcfile_ *next;      /* pointer to next file_struct_ */
                                /* for link lists */
} srcfile_t;

typedef struct dependency_ {
    char      name[MAX_FILENAME];
    void      *pointer;
    dp_type_t type;
    struct dependency_ *next;
} dependency_t;

typedef struct target_ {
    char                name[MAX_FILENAME];
    char                *command;
    struct dependency_  *dp_head;
    struct target_      *next;
    build_state_t       build_state;
    int                 total_dp;
    int                 changed_dp;
} target_t;

typedef struct remodel_node_ {
    char       name[MAX_FILENAME];
    dp_type_t  type;
    target_t   *target;
    srcfile_t  *srcfile;
    struct  remodel_node_ *parent; /* points to the parent node */
    struct  remodel_node_ **children;  /* points to the first child */
    int     child_nodes;
} remodel_node_t;

/* External variables */
extern target_t  *target_head;
extern srcfile_t *srcfile_head;

target_t     *new_target_node();
dependency_t *new_dp_node();
srcfile_t    *new_src_node();
dp_type_t    check_dp_type(char *name); 
error_t      add_src_node(srcfile_t *src_node);
remodel_node_t *new_remodel_node();

/* for definitions in main */
void main_initiate_builds(remodel_node_t *rmnode, char *target);
void main_dispatch_all_leaf_nodes(remodel_node_t *rmnode);
void main_process_response_queue(char *target_name);

/* useful macros */
#define DISPATCH_NODE(node) \
    do {                                            \
        pthread_mutex_lock(&(dispatch_queue->mtx)); \
        queue_add_node(dispatch_queue, queue_node);     \
        pthread_mutex_unlock(&(dispatch_queue->mtx));\
    } while(0)

#define SEND_RESPONSE(node) \
    do {                                            \
        pthread_mutex_lock(&(response_queue->mtx)); \
        queue_add_node(response_queue, node);       \
        pthread_mutex_unlock(&(response_queue->mtx));\
    } while(0)

#define MONITOR_NODE(node) \
    do {                                            \
        pthread_mutex_lock(&(monitor_queue->mtx)); \
        queue_add_node(monitor_queue, node);        \
        pthread_mutex_unlock(&(monitor_queue->mtx));\
    } while(0)


#endif /* MAINDEFS_H */
