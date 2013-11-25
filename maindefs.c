#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "maindefs.h"

target_t *new_target_node() {
	target_t *new_node;
	new_node = (target_t *)malloc(sizeof(target_t));
	if (new_node == NULL) {
		return new_node;
	}
	memset(new_node->name, '\0', MAX_FILENAME);
	new_node->dp_head = NULL;
	new_node->command = NULL;
	new_node->next = NULL;
	return new_node;
}

void free_target_node() {

}

dependency_t *new_dp_node() {
	dependency_t *new_node;
	new_node = (dependency_t *)malloc(sizeof(dependency_t));
	if (new_node == NULL) {
		return new_node;
	}
	memset(new_node->name, '\0', MAX_FILENAME);
	new_node->pointer = NULL;
	new_node->next = NULL;
	new_node->type = DP_UNKNOWN;
	return new_node;
}

srcfile_t *new_src_node() {
	srcfile_t *new_node;
	new_node = (srcfile_t *)malloc(sizeof(srcfile_t));
	if (new_node == NULL) {
		return new_node;
	}
	memset(new_node->name, '\0', MAX_FILENAME);
	memset(new_node->md5hash, '\0', MD5_HASHSIZE);
	new_node->next = NULL;
	new_node->is_md5_present = false;
	new_node->need_md5_calc = false;
	return new_node;
}

dp_type_t check_dp_type(char *name) {
	dp_type_t result = DP_TARGET;
	char *extn;

	extn = strstr(name, ".");
	if (extn == NULL) return result;
	if (strcmp(extn, ".c") == 0) return DP_SRC;
	if (strcmp(extn, ".h") == 0) return DP_HEADER;
	if (strcmp(extn, ".cpp") == 0) return DP_SRC;
	return result;
}
