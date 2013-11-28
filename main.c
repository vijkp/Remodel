/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Local headers */
#include "maindefs.h"
#include "misc.h"
#include "file.h"
#include "md5hash.h"

/* Globals */
target_t  *target_head;
srcfile_t *srcfile_head;

int main(int argc, char **argv) {
	error_t result;
	char target_name[128] = "DEFAULT"; 
	
	/* Argumements check */
	if (argc == 1) {
		con_log("using the default target 'DEFAULT'\n");
	} else if (argc == 2) {
		strcpy(target_name, argv[1]);
		con_log("using the given target '%s'\n", target_name);
	} else {
		con_log("error: invalid number of arguments (%d)\n", (argc - 1));
		print_usage();
		goto end;
	}

	/* Read and process remodelfile */
	target_head  = new_target_node();
	srcfile_head = new_src_node();
	result = file_process_remodelfile();
	if (result != SUCCESS) {
			goto end;
	}

	/* Check if the given target is available in the file */
	result = file_check_given_target(target_name);
	if (result != SUCCESS) {
	con_log("error: nothing to build for the target '%s'\n", target_name);  
	goto end;
	}

	/* 
	 * Calculate reverse dependency for each source file. Eg.
	 * srcfile.c -> <target1>, <target2>
	 * srcfile.h -> <target1>, <target3>
	 */
	
	/* Calculate MD5s of all the source files. Can be done in parallel? */
	result = md5_calculate_for_sources();
	if (result != SUCCESS) {
		goto end;
	}

	/* Write all source files' md5 hashes to a tmp file */

	/* Mark all the targets that need build */

	/* Build all the targets. Can be done in parallel? */

	/* print target list */
	target_t        *temp = target_head->next;
	dependency_t *dp_temp = NULL;
	while (temp != NULL) {
		con_log("target: %-18s command: %s\n", temp->name,
				temp->command);
		dp_temp = temp->dp_head;
		con_log("\tdependencies:\n");
		while(dp_temp != NULL) {
			con_log("\t\t%s\n", dp_temp->name);
			dp_temp = dp_temp->next;
		}
		temp = temp->next;
	}

	/* print src file list */
	srcfile_t *temp2 = srcfile_head->next;
	con_log("List of source files:\n");
	while (temp2 != NULL) {
		con_log("\tfile: %-18s md5: %s\n", temp2->name,
				temp2->md5hash);
		temp2 = temp2->next;
	}

end:
	//free_head_target();
	return SUCCESS;
}

