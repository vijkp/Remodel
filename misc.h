#ifndef MISC_H
#define MISC_H

#define TRUE    1
#define FALSE   0

typedef enum {
	SUCCESS,		/* Success */
	RM_NOFILEENT,   /* File not found */
	RM_FAIL			/* Fail*/
} error_t;

int debug_log_(char *filename, int ln, const char *format, ...);
int con_log(const char* fmt, ...);

#ifdef DEBUG
	#define debug_log(...) debug_log_(__FILE__, __LINE__, __VA_ARGS__)
#else
	#define debug_log(...)
#endif /* DEBUG */
#endif /* MISC_H */

