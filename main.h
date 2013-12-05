#ifndef MAIN_H
#define MAIN_H

/* Declarations for functions defined in main */
void main_initiate_builds(remodel_node_t *rmnode, char *target);
void main_dispatch_all_leaf_nodes(remodel_node_t *rmnode);
void main_process_response_queue(char *target_name);
void clean_target_list();
void clean_srcfile_list();

#endif /*MAIN_H*/
