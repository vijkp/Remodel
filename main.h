/*****************************************************
* Copyright (c) 2013. All rights reserved.
* Author: Vijay Kumar Pasikanti <vijaykp@cs.umass.edu>
* File : main.h 
*****************************************************/
#ifndef MAIN_H
#define MAIN_H

/* Declarations for functions defined in main */
void main_initiate_builds(remodel_node_t *rmnode, char *target);
void main_dispatch_all_leaf_nodes(remodel_node_t *rmnode);
void main_process_response_queue(char *target_name);
void clean_target_list();
void clean_srcfile_list();
void clean_queues();
double get_time_in_sec();
void send_killsignal_to_threads();
int calculate_number_of_threads_needed(remodel_node_t *rmnode);

#define print_time_taken(x, y)   \
    do {                            \
        double time_taken;          \
        time_taken = y - x;         \
        if (time_taken > 1000.0) {  \
            time_taken = time_taken/1000.0;\
            /* print in seconds */  \
            LOG("remodel took %.3lf seconds to complete\n", time_taken);\
        } else {                    \
            /* print in msecs */    \
            LOG("remodel took %.0lf milliseconds to complete\n", \
                    time_taken);\
        }                           \
    } while(0)

#endif /*MAIN_H*/
/* version v1.1 */
