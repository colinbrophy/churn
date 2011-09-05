#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "consts.h"
#include "utils.h"

#define BUF_SZ (456976 + 1000)
#define DIMENSIONS 4

struct letter_freq {
	suint letter;
	suint freq;
};

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
	unsigned int i;
	static struct letter_freq freq[DIMENSIONS][ALPHALEN];


	memset(freq, 0, sizeof(struct letter_freq));

	if (argc != 3)
		error_term("Wrong no of args: %d\n", argc);

	in = safe_fopen(argv[1], "rb");
	out = safe_fopen(argv[2], "wb");

	buf = safe_malloc(BUF_SZ);

	fgets(buf, BUF_SZ, in);
	for (i = 0; buf[i] != '\0'; i++)
		buf[i] -= 'A';

	for (i = 0; i < ALPHALEN; i++) {
		freq[1][i].letter += 0;
	}

	fprintf(stderr, "%d\n", i);
	fwrite(buf, i, 1, out);

	fclose(in);
	fclose(out);

	return 0;
}
