#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "maindefs.h"
#include "misc.h"
#include "md5hash.h"

/* for md5 calculations */
#include <openssl/md5.h>

/* for saving hashes into a file */
#include <sys/stat.h>


extern srcfile_t *srcfile_head;

error_t md5_calculate_for_sources() {
	error_t ret = RM_SUCCESS;
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
			node = node->next;
			continue;
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

error_t md5_save_md5_hashes() {
	error_t   ret = RM_SUCCESS;
	int       mkdir_ret, ret_val;
	FILE      *fh;
	srcfile_t *srcfile_node;

	/* Create .remodel folder */
	mkdir_ret = mkdir(RM_APP_DIR, S_IRWXU | S_IRWXG | S_IRWXO);
	if (mkdir_ret == -1) {
		if (errno == EEXIST) {
			debug_log("directory .remodel already exists\n");
		} else if (errno == ENOSPC) {
			con_log("error: directory creation failed. No enough memory.\n");
			ret = RM_FAIL;
			goto end;
		}
	} else {
		debug_log("directory .remodel created.\n");
	}

	fh = fopen(RM_MD5HASHES_BKP, "ab+");
	if (fh == NULL) {
		con_log("error: file doesn't exist\n");
		goto end;
	}

	/* write all md5 hashes of each source file */
	srcfile_node = srcfile_head->next;
	while (srcfile_node != NULL) {
		debug_log("saving %s file hash to a file.\n", srcfile_node->name);
		fprintf(fh, "%s %s\n", srcfile_node->name, srcfile_node->md5hash);
		srcfile_node = srcfile_node->next;
	}

	ret_val = rename(RM_MD5HASHES_BKP, RM_MD5HASHES);
	if (ret_val == -1) {
		con_log("error: saving md5 hashes failed.\n");
		goto end;
	}

end:
	if (fh != NULL) fclose(fh);
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

