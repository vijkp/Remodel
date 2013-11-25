#ifndef FILE_H
#define FILE_H
#include "maindefs.h"
#include "misc.h"

error_t file_process_remodelfile();
error_t file_get_remodelfile(FILE **);
error_t file_read_remodelfile(FILE **);
error_t file_parse_line(char *line);
void    file_remove_extra_chars(char *line);

#endif /* FILE_H */
