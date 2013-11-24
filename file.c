#include <stdio.h>
#include <stdlib.h>

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
		con_log("buffer: %s", line);
	}
	free(line);
	return ret;	
}

