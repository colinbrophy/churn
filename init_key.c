#include <stdlib.h>

#include "consts.h"

struct letter_freq {
	suint letter;
	suint freq;
};

/* The order  of character in the english langauge with most common
 * first.
 * 0 = A, 1 = B, 2 = C etc.
 */
static const uint freq_order[ALPHALEN] = {
	4,19,14,0,13,8,17,18,7,3,11,2,22,20,12,5,24,6,15,1,21,10,23,16,9,25
};

static int cmp_freq(const void* a, const void* b);
static void freq_analysis(struct letter_freq* freq, const suint* ciph,
	size_t ciphlen);

/* Loads the initial key by doing a frequency analysis and sorting them so
 * that they are in order that they would be for plaintext.
 * I.e. if x is the most common letter then x is probbably = e
 */
void load_inital_key(suint* key, const suint* ciph, size_t ciphlen)
{
	struct letter_freq freq[ALPHALEN];
	int i;

	/* Init letter frequencies */
	for (i = 0; i < ALPHALEN; i++) {
		freq[i].letter = i;
		freq[i].freq = 0;
	}

	freq_analysis(freq, ciph, ciphlen);
	qsort(freq, ALPHALEN, sizeof(struct letter_freq), &cmp_freq);

	for (i = 0; i < ALPHALEN; i++)
		key[freq_order[i]] = freq[i].letter;
}

static void freq_analysis(struct letter_freq* freq, const suint* ciph,
	size_t ciphlen)
{
	size_t i;

	for (i = 0; i < ciphlen; i++)
		freq[ciph[i]].freq++;
}

static int cmp_freq(const void* a, const void* b)
{
	const struct letter_freq* sa = a;
	const struct letter_freq* sb = b;
	return sb->freq - sa->freq;
}
