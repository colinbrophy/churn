#ifndef UTILS_H
#define UTILS_H
#include "consts.h"
#include <stdlib.h>
#include <stdio.h>

#define char_error EOF

void error_term(const char* error_msg, ...);
void* safe_malloc(size_t size);

#define DEF_GET_SCORE(prefix, name, len_foreach) \
prefix uint name(const suint* text, size_t len, suint* freq) \
{ \
	uint score; \
	size_t i; \
	size_t index; \
 \
	score = 0; \
	len -= 3; \
 \
	for(i = 0; i < len; i++) { \
		/* Calculate the index \
		 * Does this by doing: \
		 * l = Array Dimension e.g. 32 \
		 * xl^3 + yl^2 + zl + i \
		 * This is then factorized to make: \
		 * l(l(lx + y) + z) + i */ \
		index = text[i] * len_foreach; \
		index = (index + text[i + 1]) * len_foreach; \
		index = (index + text[i + 2]) * len_foreach; \
		index += text[i + 3]; \
 \
		score += freq[index]; \
	} \
 \
	return score; \
}

#endif /* UTILS_H */