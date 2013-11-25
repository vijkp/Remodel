#ifndef MAINDEFS_H
#define MAINDEFS_H

#include <stdlib.h>
#include "type.h"
#include <stdio.h>

struct dependency_t;
struct target_t;
struct src_file_t;

typedef enum {
	DP_TARGET,		/* Dependency is a target */
	DP_src			/* Dependency is a src file */
} dp_type_t; 

typedef struct src_file_ { 
	char filepath[256];			/* Holds file path to a source file */
	char md5hash[32];			/* md5 hash string 128 bits */
	bool is_built;				/* flag to indicate if this file needs to be built */
	struct src_file_ *next;	/* pointer to next file_struct_ for link lists */
} src_file_t;

typedef struct dependency_ {
	dp_type_t	type;
	void		*dp;
	struct dependency_ *next;
} dependency_t;

typedef struct target_ {
	char name[128];
	struct dependency_t *dep_list;
	char *command;
	struct target_ *next;
} target_t;

#endif /* MAINDEFS_H */
