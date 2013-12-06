/*****************************************************
* Copyright (c) 2013. All rights reserved.
* Author: Vijay Kumar Pasikanti <vijaykp@cs.umass.edu>
* File : md5hash.c 
* Description: This file has function related to 
* calculation of md5 hashes of a given file, storing 
* and reading of md5 hashes from disk.
*****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h> /* for md5 calculations */
#include <sys/stat.h>    /* for saving hashes into a file */

/* Local header files */
#include "maindefs.h"
#include "misc.h"
#include "md5hash.h"

/* External variables */
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
            LOG("error: file '%s' doesn't exist\n", node->name);
            ret = RM_FAIL;
            goto end;
            //node = node->next;
            //continue;
        }
        
        fseek(fd, 0, SEEK_END);
        fsize = ftell(fd);
        rewind(fd);
        fbuffer = (unsigned char *)malloc((fsize + 
                    1)*(sizeof(unsigned char)));
        fread(fbuffer, 1, fsize, fd);
    
        /* Calculate MD5 hash */
        MD5(fbuffer, fsize, md5hash);
        md5_to_string(md5hash, md5string);
        strcpy(node->md5hash, md5string);
        node->md5_present = true;
        node->md5_changed = true;
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
    FILE      *fh = NULL;
    srcfile_t *srcfile_node;

    /* Create .remodel folder */
    mkdir_ret = mkdir(RM_APP_DIR, S_IRWXU | S_IRWXG | S_IRWXO);
    if (mkdir_ret == -1) {
        if (errno == EEXIST) {
            DEBUG_LOG("directory .remodel already exists\n");
        } else if (errno == ENOSPC) {
            LOG("error: directory creation failed. Not enough memory.\n");
            ret = RM_FAIL;
            goto end;
        }
    } else {
        DEBUG_LOG("directory .remodel created.\n");
    }

    fh = fopen(RM_MD5HASH_BKPFILE, "ab+");
    if (fh == NULL) {
        LOG("error: cannot_create a file\n");
        ret = RM_FAIL;
        goto end;
    }

    /* write all md5 hashes of each source file */
    srcfile_node = srcfile_head->next;
    while (srcfile_node != NULL) {
        ret = fprintf(fh, "%s %s\n", srcfile_node->name, srcfile_node->md5hash);
        if (ret < 0) {
            LOG("saving %s file hash to a file failed.\n", srcfile_node->name);
        } else {
            ret = SUCCESS;
        }
        srcfile_node = srcfile_node->next;
    }

    ret_val = rename(RM_MD5HASH_BKPFILE, RM_MD5HASH_FILE);
    if (ret_val == -1) {
        LOG("error: saving md5 hashes failed.\n");
        ret = RM_FAIL;
        goto end;
    } else {
        DEBUG_LOG("md5 hashses saved successfully.\n");
    }

end:
    if (fh != NULL) fclose(fh);
    return ret;
}
error_t md5_load_from_file() {
    error_t ret = RM_SUCCESS;
    FILE    *fh = NULL;
    char    src_name[MAX_FILENAME];
    char    md5hash[MD5_HASHSIZE + 1];
    ssize_t read;
    size_t  len;
    char    *line;
    int     nchar = 2;
    
    fh = fopen(RM_MD5HASH_FILE, "r");
    if (fh == NULL) {
        DEBUG_LOG("project is being built for the first time\n");
        goto end;
    }

    DEBUG_LOG("im here %d %d \n", RM_SUCCESS, SUCCESS);
    while ((read = getline(&line, &len, fh)) != -1) {
        nchar = sscanf(line, "%s %s\n", src_name, md5hash);
        if (nchar != 2) { 
            continue;
        }
        DEBUG_LOG("reading md5hash of src file %s\n", src_name);
        ret = file_update_src_md5info(src_name, md5hash);
        if (ret != RM_SUCCESS) {
            goto end;
        }
    }
    if (nchar != 2) {
        DEBUG_LOG("info: metafile .remodel/md5hashes is corrupted\n");
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

/* version v1.1 */
