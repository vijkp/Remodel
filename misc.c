#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "maindefs.h"

extern remodel_node_t *remodel_head;

int DEBUG_LOG_(char *filename, int ln, const char *format, ...) {
	va_list arg;
	int result;
	char format_new[200];
	
	sprintf(format_new, "remodel: %s +%d: %s", filename, ln, format);
	format = format_new;
	va_start(arg, format);
	result = vprintf(format, arg);
	va_end(arg);
	return result;
}

int LOG(const char *format, ...) {
	va_list arg;
	int result;
	char format_new[200];
	
	sprintf(format_new, "remodel: %s", format);
	format = format_new;
	va_start(arg, format);
	result = vprintf(format, arg);
	va_end(arg);
	return result;
}

void print_usage() {
	LOG("usage: remodel [<target_name>]\n");
	LOG("   when no <target_name> is specified default\n");
	LOG("   target 'DEFAULT' is used.\n");
}

/* Print target list along with dependencies */
void print_target_list() {
	target_t        *temp = target_head->next;
	dependency_t	*dp_temp = NULL;
	while (temp != NULL) {
		DEBUG_LOG("target: %-18s (total_dp:%d) command: %s\n", temp->name,
				temp->total_dp, temp->command);
		dp_temp = temp->dp_head;
		DEBUG_LOG("\tdependencies:\n");
		while(dp_temp != NULL) {
			DEBUG_LOG("\t\t%s\n", dp_temp->name);
			dp_temp = dp_temp->next;
		}
		temp = temp->next;
	}

}

/* Print src file list along with md5 hashes, if present */
void print_srcfile_list() {
	srcfile_t *temp2 = srcfile_head->next;
	DEBUG_LOG("List of source files:\n");
	while (temp2 != NULL) {
		DEBUG_LOG("\tfile: %-18s md5: %s\n", temp2->name,
				temp2->md5hash);
		temp2 = temp2->next;
	}

}

void print_dependency_graph(remodel_node_t *node, int level) {
	remodel_node_t *rm_node = NULL;
	int i;
	char parentname[MAX_FILENAME] = "";
	char pad[200] = "";
	
	if (node->parent) {
		strcpy(parentname, node->parent->name);
	}

	for (i = 0; i < level; i++) {
		strcat(pad, "\t");
	}
	
	switch (node->type) {
		case DP_UNKNOWN:
		case DP_TARGET:
			LOG("remodel node name:%s%s -> %s(%x)\n", pad, parentname, node->name, node->target);
			if (node->child_nodes == 0) {
				goto end;
			}
			level++;
			for (i = 0; i < node->child_nodes; i++) {
				if (node->children[i]) {
					print_dependency_graph(node->children[i], level);
				}
			}
			break;
		case DP_SRC:
		case DP_HEADER:
			LOG("remodel node name:%s%s -> %s(%x)\n", pad, parentname, node->name, node->srcfile);
			goto end;
			break;
	}

end: 
	return;
}
