#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "maindefs.h"
#include "file.h"
#include "misc.h"

extern target_t *target_head;
extern srcfile_t *srcfile_head;

error_t file_process_remodelfile() {
	FILE *fh;
	error_t ret = 0;

	if ((ret = file_get_remodelfile(&fh)) != SUCCESS) {
		con_log("error: 'remodelfile' not found\n");
		goto end;
	}
	if ((ret = file_read_remodelfile(&fh)) != SUCCESS) {
		con_log("error: 'remodelfile' read failed\n");
		goto end;
	}
end:
	if (fh != NULL) fclose(fh);
	return ret;
}

error_t file_get_remodelfile(FILE **remodel_fh) {
	error_t ret = RM_NOFILEENT;
	
	/* Search for 'Remodel' file in the current directory */
	*remodel_fh = fopen("Remodelfile", "r");
	if (*remodel_fh != NULL) {
		con_log("file found 'remodelfile'\n");
		ret = SUCCESS;
		goto found;
	}
	*remodel_fh = fopen("remodelfile", "r");
	if (*remodel_fh != NULL) {
		con_log("file found 'remodelfile'\n");
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
		ret = file_parse_line(line);
		if (ret != SUCCESS) {
			break;
		}
	}
	if (ret != SUCCESS) {
		goto end;
	}
end:
	free(line);
	return ret;	
}

void file_remove_extra_chars(char *line) {
	int count, len, i, j;
	char *buffer;

	len = strlen(line);
	if (len <= 0) {
		return;
	}
	buffer = (char *)malloc((len + 1)*sizeof(char));
	memset(buffer, '\0', len+1);
	for (i = 0, j = 0; i < len; i++) {
		if ((line[i] != ' ') && (line[i] != '\t') && (line[i] != '\n')) {
			if (line[i] >= ' ') {
				buffer[j] = line[i];
				j++;
			}
		}
	}	
	buffer[j+1] = '\0';
	memset(line, '\0', len);
	len = strlen(buffer);
	strncpy(line, buffer, len);
	FREE(buffer);
}

bool file_is_string_empty(char *line) {
	int total = 0;
	int i, len;

	len = strlen(line);
	if (len == 0) {
		return TRUE; 
	}
	for (i=0; i < len; i++) {
		if ((line[i] == ' ') || (line[i] == '\t') || (line[i] == '\n')) {
			total++;
		}
	}
	if(total == len) {
		return TRUE;
	} 
	return FALSE;
}


error_t file_parse_line(char *line) {
	error_t  ret = SUCCESS;
	target_t *node;
	dependency_t *dp_node;
	char	target_name[128];
	char	*dp_string = NULL;
	char	*command_string = NULL;
	size_t	len = 0;
	int		sscanf_out = 0;
	bool	target_found = false;
	bool	dp_string_found = false;
	bool	command_found = false;
	char	*position1 = NULL;
	char	*position2 = NULL;

	memset(target_name, '\0', 128);
	len = strlen(line);
		
	if (file_is_string_empty(line)) {
		goto error; /* Return value is still success */
	}

	dp_string = (char *)malloc(len*sizeof(char));
	command_string = (char *)malloc(len*sizeof(char));
	memset(dp_string, '\0', len);
	memset(command_string, '\0', len);

	position1 = strstr(line, "<-:");
	position2 = strstr(line, ":\"");

	if((position1 == NULL) && (position2 == NULL)) {
		sscanf_out = sscanf(line, "%[^'<']<-%[^':']:%*[^\"]\"%[^\"]\"", 
				target_name, dp_string, command_string);
	} else if((position1 != NULL) && (position2 == NULL)) {
		sscanf_out = sscanf(line, "%[^'<']<-:%*[^\"]\"%[^\"]\"",
				target_name, command_string);
	} else if((position1 == NULL) && (position2 != NULL)) {
		sscanf_out = sscanf(line, "%[^'<']<-%[^':']:\"%[^\"]\"", 
				target_name, dp_string, command_string);
	} else if((position1 != NULL) && (position2 != NULL)) {
		sscanf_out = sscanf(line, "%[^'<']<-:\"%[^\"]\"",
				target_name, command_string);
	}

	file_remove_extra_chars(target_name);
	if (!file_is_string_empty(target_name)) {
		target_found = true;
	}
	file_remove_extra_chars(dp_string);
	if (!file_is_string_empty(dp_string)) {
		dp_string_found = true;
	}
	if (!file_is_string_empty(command_string)) {
		command_found = true;
	}
	if (!(target_found && (dp_string_found || command_found))) {
		len = strlen(line);
		if (line[len-1] == '\n') line[len-1] = ' ';
		con_log("error: parsing failed for the line \"%s\"\n", line); 
		ret = RM_PARSE_FAIL;
		goto error;
	}
	
	/* Build the list of targets */
	node = new_target_node();
	strcpy(node->name, target_name);
	node->command = command_string;

	/* Add to target list */
	node->next = target_head->next;
	target_head->next = node;

	/* Parse dependencies */
	if (dp_string_found) {
		char *dp_name;
		srcfile_t *src_node;
		dp_type_t dp_type;
		dp_name = strtok(dp_string, ",");
		while (dp_name != NULL) {
			dp_node = new_dp_node();
			dp_type = check_dp_type(dp_name);
			
			/* For source files add them to src file list */
			if ((dp_type == DP_SRC) || (dp_type == DP_HEADER)) {
				src_node = new_src_node();
				strcpy(src_node->name, dp_name);
				if(add_src_node(src_node) == RM_FAIL) {
					FREE(src_node);
				}
			}
			dp_node->type = dp_type;
			strcpy(dp_node->name, dp_name);

			/* Update this node to dp_head node of the target */
			if (node->dp_head == NULL) {
				dp_node->next = NULL;
				node->dp_head = dp_node;
			} else {
				dp_node->next = node->dp_head;
				node->dp_head = dp_node;
			}
			dp_name = strtok(NULL, ",");
		}
	} 
	goto end;

error:
	FREE(command_string);
end:	
	FREE(dp_string);
	return ret;
}

error_t file_check_given_target(char *target) {
	error_t  ret = RM_FAIL;
	target_t *node;
	bool     target_found = false;

	node = target_head->next;
	while (node != NULL) {
		if (strcmp(node->name, target) == 0) {
			target_found = true;
			ret = SUCCESS;
			goto end;
		}
		node = node->next;
	}
end:
	return ret;
}

error_t file_update_src_md5info(char *src_name, char *md5hash) {
	error_t   ret = SUCCESS;
	srcfile_t *node = NULL;
	bool	  src_found = false;

	node = srcfile_head->next;
	while ((node != NULL) && (src_found == false)) {
		if(strcmp(node->name, src_name) == 0) {
			src_found = true;
			if (strcmp(node->md5hash, md5hash) != 0) {
				con_log("src file '%s' changed.\n", src_name);
				node->md5_changed = true;
			} else {
				node->md5_changed = false;
			}
		}
		node = node->next;
	}
end:
	return ret;
}
