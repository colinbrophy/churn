#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "../consts.h"

#define BUF_SZ (456976 + 1000)
#define DIMENSIONS 4
#define ALPHALEN 26

struct letter_freq {
	suint letter;
	suint freq;
};

void* safe_malloc(size_t size)
{
	void* p = malloc(size);
	if (p == NULL)
		error_term("Out of memory\n");
	return p;
}

void error_term(const char* error_msg, ...)
{
	va_list args;

	va_start(args, error_msg);
	vfprintf(stderr, error_msg, args);
	va_end(args);

	exit(EXIT_FAILURE);
}

FILE* safe_fopen(const char* path, const char* mode)
{
	FILE* f;

	f = fopen(path, mode);
	if (f == NULL)
		error_term("ERROR Opening %s", path);

	return f;
}

int main(int argc, char** argv)
{
	FILE* in;
	FILE* out;
	char* buf;
	unsigned int i, x;
	static struct letter_freq freq[DIMENSIONS][ALPHALEN];


	memset(letter_freq, 0, sizeof(letter_freq));

	if (argc != 3)
		error_term("Wrong no of args: %d\n", argc);

	in = safe_fopen(argv[1], "rb");
	out = safe_fopen(argv[2], "wb");

	buf = safe_malloc(BUF_SZ);

	fgets(buf, BUF_SZ, in);
	for (i = 0; buf[i] != '\0'; i++)
		buf[i] -= 'A';

	for (i = 0; i < ALPHALEN; i++) {
		freq[1][i] +=
	}

	fprintf(stderr, "%d\n", i);
	fwrite(buf, i, 1, out);

	fclose(in);
	fclose(out);

	return 0;
}
