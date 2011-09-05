#include "utils.h"

#include <stdarg.h>

void error_term(const char* error_msg, ...)
{
	va_list args;

	fflush(stdout);
	va_start(args, error_msg);
	vfprintf(stderr, error_msg, args);
	va_end(args);

	exit(EXIT_FAILURE);
}

void* safe_malloc(size_t size)
{
	void* p = malloc(size);
	if (p == NULL)
		error_term("Out of memory\n");
	return p;
}
