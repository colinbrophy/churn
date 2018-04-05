#ifndef INIT_KEY_H
#define INIT_KEY_H

#include "consts.h"

/* Loads the initial key by doing a frequency analysis and sorting them so
 * that they are in order that they would be for plaintext.
 * I.e. if x is the most common letter then x is probbably = e
 */
void load_inital_key(suint* key, const suint* ciph, size_t ciphlen);

#endif /* INIT_KEY_H */
