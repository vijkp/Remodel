#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "maindefs.h"
#include "misc.h"
#include "md5hash.h"

/* for md5 calculations */
#include <openssl/md5.h>

extern srcfile_t *srcfile_head;

error_t md5_calculate_for_sources() {
	error_t ret = SUCCESS;
	srcfile_t *node;
	FILE *fd;
	unsigned char *fbuffer;
	unsigned char md5hash[MD5_DIGEST_LENGTH];
	long fsize;
	char md5string[2*MD5_DIGEST_LENGTH+1];
	
	/* Go through all source files and calculate md5 for each one */
	node = srcfile_head->next;
	while (node != NULL) {
		fd = fopen(node->name, "rb");
		if (fd == NULL) {
			con_log("warning: failed to open '%s'\n", node->name);
		}
		
		fseek(fd, 0, SEEK_END);
		fsize = ftell(fd);
		rewind(fd);
		fbuffer = (unsigned char *)malloc((fsize + 
					1)*(sizeof(unsigned char)));
		fread(fbuffer, 1, fsize, fd);
	
		/* Calculate MD5 */
		MD5(fbuffer, fsize, md5hash);
		md5_to_string(md5hash, md5string);
		strcpy(node->md5hash, md5string);
		debug_log("calculating md5 for %s size: %ld bytes md5: %s\n", 
				node->name, fsize, md5string);
		fclose(fd);
		FREE(fbuffer);	
		node = node->next;
	}
end:
	return ret;
}

void md5_to_string(unsigned char *md, char *result) {
	int i;
	char tmp[2];
	for(i=0; i < MD5_DIGEST_LENGTH; i++) {
		sprintf(tmp, "%02x",md[i]);
		result[2*i] = tmp[0];
		result[2*i+1] = tmp[1];
	}
	result[32] = '\0';
}

