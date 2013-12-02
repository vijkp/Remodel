#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "maindefs.h"


/* external variables */
extern srcfile_t *srcfile_head;

target_t *new_target_node() {
	target_t *new_node;
	new_node = (target_t *)malloc(sizeof(target_t));
	if (new_node == NULL) {
		LOG("error: system out of memory.\nremodel failed.\n");
		return new_node;
	}
	memset(new_node->name, '\0', MAX_FILENAME);
	new_node->dp_head  = NULL;
	new_node->command  = NULL;
	new_node->next     = NULL;
	new_node->is_built = true;
	new_node->total_dp = 0;
	return new_node;
}

void free_target_node() {

}

dependency_t *new_dp_node() {
	dependency_t *new_node;
	new_node = (dependency_t *)malloc(sizeof(dependency_t));
	if (new_node == NULL) {
		LOG("error: system out of memory.\nremodel failed.\n");
		return new_node;
	}
	memset(new_node->name, '\0', MAX_FILENAME);
	new_node->pointer = NULL;
	new_node->next = NULL;
	new_node->type = DP_UNKNOWN;
	return new_node;
}

remodel_node_t *new_remodel_node() {
	remodel_node_t *new_node;
	new_node = (remodel_node_t *)malloc(sizeof(remodel_node_t));
	if (new_node == NULL) {
		LOG("error: system out of memory.\nremodel failed.\n");
		return new_node;
	}
	new_node->type = DP_UNKNOWN;
	new_node->parent = NULL;
	new_node->children = NULL;
	new_node->child_nodes = 0;
}

srcfile_t *new_src_node() {
	srcfile_t *new_node;
	new_node = (srcfile_t *)malloc(sizeof(srcfile_t));
	if (new_node == NULL) {
		LOG("error: system out of memory.\nremodel failed.\n");
		return new_node;
	}
	memset(new_node->name, '\0', MAX_FILENAME);
	memset(new_node->md5hash, '\0', MD5_HASHSIZE);
	new_node->next = NULL;
	new_node->md5_present = false;
	new_node->md5_changed = false;
	return new_node;
}

error_t add_src_node(srcfile_t *src_node) {
	error_t ret = RM_FAIL;
	srcfile_t *tempnode;
	int src_found = false;
	
	tempnode = srcfile_head->next;
	while((tempnode != NULL) && (src_found == false)) {
		if (strcmp(tempnode->name, src_node->name) == 0) {
			src_found = true;
		}
		tempnode = tempnode->next;
	}
	if (src_found == false) {
		src_node->next = srcfile_head->next;
		srcfile_head->next = src_node;
		//DEBUG_LOG("src file '%s' added to src list.\n",
		//		src_node->name);
		ret = RM_SUCCESS;
		goto end;
	} else {
		//DEBUG_LOG("src file '%s' exists in the list\n", 
		//		src_node->name);
	}
end:
	return ret;
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

