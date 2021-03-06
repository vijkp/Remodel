/*****************************************************
* Copyright (c) 2013. All rights reserved.
* Author: Vijay Kumar Pasikanti <vijaykp@cs.umass.edu>
* File : md5hash.h 
*****************************************************/
#ifndef MD5HASH_H
#define MD5HASH_H

error_t md5_calculate_for_sources();
error_t md5_save_md5_hashes();
error_t md5_load_from_file();
void md5_to_string(unsigned char *md, char *string);
#endif /* MD5HASH_H */
/* version v1.1 */
