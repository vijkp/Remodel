#ifndef MISC_H
#define MISC_H

#define TRUE    1
#define FALSE   0
#define true    TRUE
#define false   FALSE

typedef enum {
	RM_ERROR = -1,
	RM_SUCCESS,		/* Success */
	RM_NOFILEENT,   /* File not found */
	RM_FAIL,		/* Fail*/
	RM_PARSE_FAIL,  /* Parsing Failed */
					/* insert new error codes here */
	SUCCESS = RM_SUCCESS /* keep this at the end */
} error_t;

int debug_log_(char *filename, int ln, const char *format, ...);
int con_log(const char* fmt, ...);
void print_usage();

#ifdef DEBUG
	#define debug_log(...) debug_log_(__FILE__, __LINE__, __VA_ARGS__)
#else
	#define debug_log(...)
#endif /* DEBUG */

#define FREE(x) { \
		if (x != NULL) free(x);\
	}
#endif /* MISC_H */

