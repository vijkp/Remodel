#ifndef FILE_H
#define FILE_H
#include "maindefs.h"
#include "misc.h"

error_t file_process_remodelfile();
error_t file_get_remodelfile(FILE **);
error_t file_read_remodelfile(FILE **);
error_t file_parse_line(char *line);
void    file_remove_extra_chars(char *line);
error_t file_check_given_target(char *target); 
error_t file_mark_all_targets();
error_t file_create_dependecy_graph(char *target_name); 

#endif /* FILE_H */
