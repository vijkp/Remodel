#ifndef FILE_H
#define FILE_H

#include "maindefs.h"
#include "misc.h"

error_t file_process_remodelfile();
error_t file_get_remodelfile(FILE **);
error_t file_read_remodelfile(FILE **);
error_t file_parse_line(char *line);
void    file_remove_extra_chars(char *line);
target_t *file_get_target(char *target_name); 
srcfile_t *file_get_srcfile(char *srcfile_name); 
error_t file_mark_all_targets_for_build(remodel_node_t *);
error_t file_create_dependency_graph(target_t *target); 
error_t file_add_nodes_to_remodel(remodel_node_t *rmnode);
void file_remove_unnecessary_nodes(remodel_node_t *rmnode);
void file_cleanup_nodes_for_unchanged_files(remodel_node_t *rmnode);
void file_update_children_count(remodel_node_t *rmnode);
#endif /* FILE_H */
