/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Local headers */
#include "maindefs.h"
#include "misc.h"
#include "file.h"
#include "md5hash.h"
#include "threads.h"

/* Config macros (used only in this file) */
#define TOTAL_THREADS 5

/* Globals */
target_t		*target_head;
srcfile_t		*srcfile_head;
remodel_node_t	*remodel_head;
thread_data		thread_list[TOTAL_THREADS];

int main(int argc, char **argv) {
	error_t  result;
	char     target_name[MAX_FILENAME] = "DEFAULT";
	int		 total_threads = TOTAL_THREADS;
	target_t *target = NULL;
	
	/* Argumements check */
	if (argc == 1) {
		LOG("using the default target 'DEFAULT'\n");
	} else if (argc == 2) {
		strcpy(target_name, argv[1]);
		LOG("using the given target '%s'\n", target_name);
	} else {
		LOG("error: invalid number of arguments (%d)\n", (argc - 1));
		print_usage();
		goto end;
	}

	target_head  = new_target_node();
	srcfile_head = new_src_node();
	remodel_head = new_remodel_node();
	if (!(target_head && srcfile_head && remodel_head)) {
		goto end; /* error: system out of memory */
	}

	/* Read and process remodelfile */
	result = file_process_remodelfile();
	if (result != SUCCESS) {
			goto end;
	}

	/* Check if the given target is available in the file */
	target = file_get_target(target_name);
	if ( target == NULL) {
		LOG("error: nothing to build for the target '%s'\n",
				target_name);  
		goto end;
	}

	/* 
	 * XXX Calculate reverse dependency for each source file. Eg.
	 * srcfile.c -> <target1>, <target2>
	 * srcfile.h -> <target1>, <target3>
	 */
	
	/* Calculate MD5s of all the source files. Can be done in parallel? */
	result = md5_calculate_for_sources();
	if (result != SUCCESS) {
		goto end;
	}

	/* Compare and load, sourcefile md5 info into srcfile_t nodes*/
	result = md5_load_from_file();
	if (result != SUCCESS) {
		goto end;
	}

	/* Save <sourcefile> <md5> in a temp file in .remodel/ folder */
	result = md5_save_md5_hashes();
	if (result != SUCCESS) {
		goto end;
	}

	/* Create a dependency graph for the given target */
	result = file_create_dependency_graph(target);
	if (result != SUCCESS) {
		goto end;
	}

	/* Mark all the targets that need build */
	result = file_mark_all_targets();
	if (result != SUCCESS) {
		goto end;
	}

	/* Spawn 5 threads to run the commands */
	result = spawn_threads(total_threads);
	if (result != SUCCESS) {
		goto end;
	}
	

	/* XXX Build all the targets. Can be done in parallel? */
		

	/* XXX remove print functions later */
	//print_srcfile_list();
	//print_target_list();
	print_dependency_graph(remodel_head, 0);

end:
//XXX	free_target_head();
//XXX	free_srcfile_head();
//XXX   free_remodel_head();
	return SUCCESS;
}

