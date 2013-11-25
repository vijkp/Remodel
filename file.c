#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "misc.h"

error_t file_process_remodelfile() {
	FILE *fh;
	error_t ret = 0;

	if ((ret = file_get_remodelfile(&fh)) != SUCCESS) {
		con_log("Remodelfile not found\n");
		return ret;
	}

	if ((ret = file_read_remodelfile(&fh)) != SUCCESS) {
		return ret;
	}

	return ret;
}

error_t file_get_remodelfile(FILE **remodel_fh) {
	error_t ret = RM_NOFILEENT;
	
	/* Search for 'Remodel' file in the current directory */
	*remodel_fh = fopen("Remodelfile", "r");
	if (*remodel_fh != NULL) {
		con_log("'Remodelfile' file found.\n");
		ret = SUCCESS;
		goto found;
	}
	*remodel_fh = fopen("remodelfile", "r");
	if (*remodel_fh != NULL) {
		con_log("'remodelfile' file found.\n");
		ret = SUCCESS;
		goto found;
	}
	return ret;
found:
	return ret;
}

error_t file_read_remodelfile(FILE **fh) {
	char *line = NULL;
	long fsize;
	size_t len=0;
	ssize_t read;
	error_t ret = 0;
	
	while ((read = getline(&line, &len, *fh)) != -1) {
		file_parse_line(line);
	}
	free(line);
	return ret;	
}

void remove_all_white_spaces(char *line) {
	int count, len, i, j;
	char *buffer;

	len = strlen(line);
	if (len <= 0) {
		return;
	}
	buffer = (char *)malloc((len + 1)*sizeof(char));
	for (i = 0, j = 0; i < len; i++) {
		if ((line[i] != ' ') && (line[i] != '\t') && (line[i] != '\n')) {
			buffer[j] = line[i];
			j++;
		}
	}	
	buffer[j+1] = '\0';
	debug_log("len: %d, count: %d, j: %d, buffer: %s, len: %d\n", len, count, j, buffer, strlen(buffer));
	len = strlen(buffer);
	line = (char *)realloc(line, len+1);
	strncpy(line, buffer, len);
	FREE(buffer);
}

void file_parse_line(char *line) {
	char target_name[128];
	char *dep_command_string = NULL;
	char *dependency_string = NULL;
	char *command_string = NULL;
	size_t len;
	char *pos;
	int position;
	
	memset(target_name, 0, 128);
	len = strlen(line);
	pos = strstr(line, "<-");
	if (pos != NULL) {
		position = pos - line;
		strncpy(target_name, line, position);
		dep_command_string = (char *)malloc((len - position+1)*sizeof(char));
		strncpy(dep_command_string, pos + 2, (len - position));
		debug_log("target name: %s\n", target_name);
		debug_log("rest of the string:: %s\n", dep_command_string);
	}
end:
	FREE(dep_command_string);
	FREE(dependency_string);
	FREE(command_string);
}
