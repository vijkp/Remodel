#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "maindefs.h"
	
int debug_log_(char *filename, int ln, const char *format, ...) {
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

int con_log(const char *format, ...) {
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
	con_log("usage: remodel [<target_name>]\n");
	con_log("   when no <target_name> is specified default\n");
	con_log("   target 'DEFAULT' is used.\n");
}

/* Print target list along with dependencies */
void print_target_list() {
	target_t        *temp = target_head->next;
	dependency_t	*dp_temp = NULL;
	while (temp != NULL) {
		debug_log("target: %-18s command: %s\n", temp->name,
				temp->command);
		dp_temp = temp->dp_head;
		debug_log("\tdependencies:\n");
		while(dp_temp != NULL) {
			debug_log("\t\t%s\n", dp_temp->name);
			dp_temp = dp_temp->next;
		}
		temp = temp->next;
	}

}

/* Print src file list along with md5 hashes, if present */
void print_srcfile_list() {
	srcfile_t *temp2 = srcfile_head->next;
	debug_log("List of source files:\n");
	while (temp2 != NULL) {
		debug_log("\tfile: %-18s md5: %s\n", temp2->name,
				temp2->md5hash);
		temp2 = temp2->next;
	}

}
