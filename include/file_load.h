/* file_load.h
 * Loads files into a buffer
 */
#ifndef FILE_LOAD_H
#define FILE_LOAD_H

#include <stdlib.h>
#include <stdio.h>

struct buffer {
	void *data;
	size_t len; /* length in C bytes */
};

/* Loads file of name 'filename' into buffer in memory
 * Returns NULL if fails */
struct buffer* load_file(const char* filename);

/* Loads 'file' into buffer
 * Returns NULL if fails */
struct buffer* load_buffer(FILE* file);

/* Deletes buffer loaded by "load_file" or 'load_buffer' function */
void delete_buffer(struct buffer* buf);

#endif /* FILE_LOAD_H */
