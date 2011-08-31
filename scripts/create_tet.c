#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define BUF_SZ (456976 + 1000)


void error_term(const char* error_msg, ...)
{
	va_list args;

	va_start(args, error_msg);
	vfprintf(stderr, error_msg, args);
	va_end(args);

	exit(EXIT_FAILURE);
}
int main(int argc, char** argv)
{
	FILE* in;
	FILE* out;
	char* buf;
	unsigned int i;
	
	if (argc != 3)
		error_term("Wrong no of args: %d\n", argc);
	in = fopen(argv[1], "rb");
	out = fopen(argv[2], "wb");
	if (in == NULL)
		error_term("ERROR Opening %s", argv[1]);
	if (out == NULL)
		error_term("ERROR Opening %s", argv[2]);

	buf = malloc(BUF_SZ);

	fgets(buf, BUF_SZ, in);
	for (i = 0; buf[i] != '\0'; i++)
		buf[i] -= 'A';

	fprintf(stderr, "%d\n", i);
	fwrite(buf, i, 1, out);

	fclose(in);
	fclose(out);

	return 0;
}
