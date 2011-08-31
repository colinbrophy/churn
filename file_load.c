/* file_load.c
 * Loads files into a buffer
 */

#include "file_load.h"
#include <errno.h>
#include <stdio.h>

#define CHUNK (1024 * 4)

struct buffer* load_file(const char* filename)
{
	FILE* f;
	struct buffer* buf;

	f = fopen(filename, "rb");
	if (f == NULL)
		return NULL;

	buf = load_buffer(f);

	fclose(f);
	return buf;
}

struct buffer* load_buffer(FILE* file)
{
	int ch;
	unsigned int size = 0;
	struct buffer* buf;

	buf = malloc(sizeof(struct buffer));
	if (buf == NULL)
		return NULL;

	buf->len = 0;
	buf->data = malloc(CHUNK);
	if (buf->data == NULL)
		goto err_bufdata;

	while ((ch = fgetc(file)) != EOF) {
		size++;
		if (size == CHUNK) {
			buf->len += CHUNK;
			buf->data = realloc(buf->data, buf->len + CHUNK);
			if (buf->data == NULL)
				goto err_read;
			size = 0;
		}
		((char*) buf->data)[buf->len + size - 1] = ch;
	}
	if (ferror(file))
		goto err_read;
	buf->len += size;

	return buf;

err_read:
	free(buf->data);
err_bufdata:
	free(buf);
	return NULL;
}

void delete_buffer(struct buffer* buf)
{
	free(buf->data);
	free(buf);
}
