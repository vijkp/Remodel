#include <stdio.h>
#include <stdarg.h>
#include <string.h>

int debug_log_(char *filename, int ln, const char *format, ...) {
	va_list arg;
	int result;
	char format_new[200];
	
	sprintf(format_new, "%s[%d]: %s", filename, ln, format);
	format = format_new;
	va_start(arg, format);
	result = vprintf(format, arg);
	va_end(arg);
	return result;
}

int con_log(const char *format, ...) {
	va_list arg;
	int result;
	char format_new[200];
	
	sprintf(format_new, "remodel: %s", format);
	format = format_new;
	va_start(arg, format);
	result = vprintf(format, arg);
	va_end(arg);
	return result;
}

