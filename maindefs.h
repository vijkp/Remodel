#ifndef MAINDEFS_H
#define MAINDEFS_H

#include <stdlib.h>
#include <stdio.h>

#include "misc.h"
#include "type.h"

struct dependency_;
struct target_;
struct srcfile_;

#define MAX_FILENAME 256
#define MD5_HASHSIZE 32

typedef enum {
	DP_UNKNOWN,     /* Default value. If unknown, figure out */
	DP_TARGET,		/* Dependency is a target */
	DP_SRC,			/* Dependency is a src file */
	DP_HEADER		/* Header file */
} dp_type_t; 

typedef struct srcfile_ { 
	char name[MAX_FILENAME];	/* Holds file path to a source file */
	char md5hash[MD5_HASHSIZE];	/* md5 hash string 128 bits */
	bool is_md5_present;				/* md5 hash calculation needed */
	bool need_md5_calc;			/* md5 calc needed or not */	
	struct srcfile_ *next;	    /* pointer to next file_struct_ */
		                        /* for link lists */
} srcfile_t;

typedef struct dependency_ {
	char	  name[MAX_FILENAME];
	void	  *pointer;
	dp_type_t type;
	struct dependency_ *next;
} dependency_t;

typedef struct target_ {
	char   name[MAX_FILENAME];
	char   *command;
	struct dependency_  *dp_head;
	struct target_      *next;
} target_t;

target_t     *new_target_node();
dependency_t *new_dp_node();
srcfile_t    *new_src_node();
dp_type_t	 check_dp_type(char *name); 
#endif /* MAINDEFS_H */