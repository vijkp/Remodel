/* Standard headers */
#include <stdio.h>
#include <stdlib.h>

/* Local headers */
#include "maindefs.h"
#include "misc.h"
#include "file.h"

int main(int argc, char **argv) {
	error_t result;
	target_t *head;
	
	result = file_process_remodelfile();
	if (result != SUCCESS) {
		con_log("Process Remodelfile failed\n");
		return result;
	}

	return SUCCESS;
}

