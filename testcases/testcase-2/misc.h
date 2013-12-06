#ifndef MISC_H
#define MISC_H

#include "maindefs.h"

#define TRUE    1
#define FALSE   0
#define true    TRUE
#define false   FALSE


typedef enum {
    RM_ERROR = -1,
    RM_SUCCESS,     /* Success */
    RM_NOFILEENT,   /* File not found */
    RM_FAIL,        /* Fail*/
    RM_PARSE_FAIL,  /* Parsing Failed */
                    /* insert new error codes here */
    SUCCESS = RM_SUCCESS /* keep this at the end */
} error_t;

int DEBUG_LOG_(char *filename, int ln, const char *format, ...);
int LOG(const char* fmt, ...);
void print_usage();
void print_srcfile_list();
void print_target_list();
void print_dependency_graph();
#ifdef DEBUG
    #define DEBUG_LOG(...) DEBUG_LOG_(__FILE__, __LINE__, __VA_ARGS__)
#else
    #define DEBUG_LOG(...)
#endif /* DEBUG */

#define FREE(x)     \
    do {            \
        if (x != NULL) {    \
            free(x);        \
            x = NULL;       \
        }           \
    } while(0)
#endif /* MISC_H */

