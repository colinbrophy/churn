#ifndef UTILS_H
#define UTILS_H
#include "consts.h"
#include <stdlib.h>
#include <stdio.h>

#define char_error EOF

void error_term(const char* error_msg, ...);
void* safe_malloc(size_t size);

#endif /* UTILS_H */