/* churn.c
 * Uses a churn algorithm to crack ciphers
 * Colin Brophy */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "file_load.h"
#include "init_key.h"
#include "utils.h"

#define CHURNLEN 100
#define FREQ_LEN_FOREACH 32
#define FREQ_TBL_LEN (pow(FREQ_LEN_FOREACH, (FREQ_DIMENSIONS - 1)) * ALPHALEN)

static const char* freq_file  = "data/tet";
static const uint maxkeys = 70000;
static const int min_bestscore_per_char = 9;

const int churn_cipherlen = 110;
const int churn_range[CHURNLEN] = {
	1,1,1,1,1,1,1,2,2,2,2,2,2,2,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5,5,5,5,6,6,6,
	6,6,7,7,7,7,7,8,8,9,9,9,9,10,10,10,10,10,11,11,11,11,12,12,12,12,13,13,
	14,14,15,15,16,16,16,16,17,17,18,18,19,19,20,21,21,22,23,23,24,25,26,
	27,28,29,30,31,33,34,37,39,41,43,48,57,60
};

struct keys {
	int deciph_key;
	int enciph_key;
};

static suint* init_freq_tbl();
static struct buffer* load_cipher_file(const char* file);
static struct buffer* load_cipher_buf(FILE* file);
static void decipher(suint* dest, const suint* src, size_t len,
	const suint* key);
static void print_plain(const suint* plain, size_t len);
static void print_key(const suint* key);
static int uint_to_char(uint n);
static uint char_to_uint(char c);

uint char_to_uint(const char c);
int uint_to_char(uint n);

DEF_GET_SCORE(static, get_score, FREQ_LEN_FOREACH)

int main(int argc, char** argv)
{
	struct buffer* ciph;
	suint* ciphtxt;
	suint* plain;
	suint parent[ALPHALEN];
	suint child[ALPHALEN];
	size_t i;
	suint* freq;

	uint parentscore;
	uint childscore;
	uint bestscore;
	int modifed_churn[CHURNLEN];

	uint nok = 0;
	uint minscore = 0;
	uint keys_since_hit = 0;

	switch (argc) {
	case 1:
		ciph = load_cipher_buf(stdin);
		if (ciph == NULL)
			error_term("Error Loading ciphertext file: stdin\n");
		break;
	case 2:
		ciph = load_cipher_file(argv[1]);
		if (ciph == NULL)
			error_term("Error Loading ciphertext file: %s\n",
				argv[1]);
		if (ciph->len <= 0)
			error_term("Error wrong no chars entered\n");
		break;
	default:
		error_term("Wrong no of args: %d\n", argc - 1);

		/* Stops compiler complaining.
		 * Line never reached */
		return EXIT_FAILURE;
	}

	freq = init_freq_tbl();
	if (freq == NULL)
		error_term("Error Loading: %s\n", freq_file);

	ciphtxt = ciph->data; /* Get rid of annoying casting */

	plain = safe_malloc(sizeof(int) * ciph->len);

	load_inital_key(parent, ciphtxt, ciph->len);

	for (i = 0; i < CHURNLEN; i++) {
		double fract = ciph->len / ((double) churn_cipherlen);
		modifed_churn[i] = round(churn_range[i] * fract);
	}

	minscore = min_bestscore_per_char * ciph->len;
	bestscore = minscore;

	srand(time(NULL));

	decipher(plain, ciphtxt, ciph->len, parent);

	parentscore = get_score(plain, ciph->len, freq);

	for (;;) {
		size_t rand1;
		size_t rand2;
		suint tmp;

		memcpy(child, parent, sizeof(child));

		do {
			rand1 = rand() % ALPHALEN;
			rand2 = rand() % ALPHALEN;
		} while(rand1 == rand2);
		tmp = child[rand1];
		child[rand1] = child[rand2];
		child[rand2] = tmp;

		decipher(plain, ciphtxt, ciph->len, child);
		childscore = get_score(plain, ciph->len, freq);

		rand1 = rand() % CHURNLEN;
		if (childscore > (parentscore - modifed_churn[rand1])) {
			parentscore = childscore;
			memcpy(parent, child, sizeof(child));
		}
		if (childscore > bestscore) {
			print_key(child);
			printf("\nScore = %d\n", childscore);
			printf("Number of keys = %d\n", nok);
			printf("Keys since last hit = %d\n", keys_since_hit);
			print_plain(plain, ciph->len);

			bestscore = childscore;
			keys_since_hit = 0;
		} else if (++keys_since_hit == maxkeys && bestscore > minscore)
				break;
		nok++;
	}

	free(freq);
	free(plain);
	delete_buffer(ciph);
	return EXIT_SUCCESS;
}

/* Loads freq table
 * returns freq table
 * No error codes: terminates if error */
static suint* init_freq_tbl()
{
	struct buffer* buf;
	suint* freq;
	size_t x, y, z, i;
	size_t ind, bufind;

	buf = load_file(freq_file);
	if (buf == NULL)
		return NULL;

	if (buf->len != FREQ_FILE_LEN) {
		freq = NULL;
		goto exit;
	}

#if FREQ_LEN_FOREACH == 32
	freq = malloc(sizeof(suint) * FREQ_TBL_LEN);
	if (freq == NULL)
		goto exit;

	/* Convert the 26x26x26x26 freq table into a 32x32x32x26 table
	 * This is done because 32 is a power of 2 so multiplication is faster
	 * (can be done by bit shifting) */
	for (x = 0; x != ALPHALEN; x++)
		for (y = 0; y != ALPHALEN; y++)
			for (z = 0; z != ALPHALEN; z++)
				for (i = 0; i != ALPHALEN; i++) {
					/* Calculate the indexes
					 * Does this by doing:
					 * l = Array Dimension e.g. 32
					 * xl^3 + yl^2 + zl + i
					 * This is then factorized to make:
					 * l(l(lx + y) + z) + i */
					ind  = (x * FREQ_LEN_FOREACH + y)
						* FREQ_LEN_FOREACH + z;
					ind *= FREQ_LEN_FOREACH;
					ind += i;

					bufind  = (ALPHALEN * x + y)
						* ALPHALEN + z;
					bufind *= ALPHALEN;
					bufind += i;

					freq[ind] = ((char*) buf->data)[bufind];
				}

exit:
	delete_buffer(buf);
	return freq;
#else
	freq = buf->data;

exit:
	free(buf);
	return freq;
#endif
}

static int init_cipher(struct buffer* cipher)
{
	char* ciphtxt;
	char* src;
	char* dest;
	char* bufend;
	suint* t;
	size_t i;

	ciphtxt = cipher->data;

	/* Remove nonalpha chars */
	bufend = cipher->len + ciphtxt;
	for (src = dest = cipher->data; src < bufend; src++)
		if (isalpha(*src)) {
			*dest = *src;
			dest++;
		}

	/* Change cipherlen so that it is the processed version */
	cipher->len = dest - ciphtxt;

	t = malloc(cipher->len * sizeof(suint));
	if (t == NULL)
		return EXIT_FAILURE;

	for (i = 0; i < cipher->len; i++) {
		int tmp;
		tmp = char_to_uint(ciphtxt[i]);
		if (tmp == -1)
			goto err;
		t[i] = tmp;
	}

	free(cipher->data);
	cipher->data = t;

	return EXIT_SUCCESS;
err:
	free(t);
	return EXIT_FAILURE;
}

static struct buffer* load_cipher_file(const char* file)
{
	struct buffer* ciph;

	ciph = load_file(file);
	if (ciph == NULL)
		return NULL;

	if (init_cipher(ciph) == EXIT_FAILURE)
		goto err;

	return ciph;

err:
	delete_buffer(ciph);
	return NULL;
}

static struct buffer* load_cipher_buf(FILE* file)
{
	struct buffer* ciph;

	ciph = load_buffer(file);
	if (ciph == NULL)
		return NULL;

	if (init_cipher(ciph) == EXIT_FAILURE)
		goto err;

	return ciph;

err:
	delete_buffer(ciph);
	return NULL;
}

static void decipher(suint* dest, const suint* src, size_t len,
	const suint* key)
{
	size_t i;

	for (i = 0; i < len; i++)
		dest[i] = key[src[i]];
}

static void print_plain(const suint* plain, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++)
		putchar(tolower(uint_to_char(plain[i])));
	putchar('\n');
}

static int keys_cmp(const void* a, const void* b)
{
	const struct keys* ka = a;
	const struct keys* kb = b;
	return (ka->deciph_key - kb->deciph_key);
}

static void print_key(const suint* key)
{
	char buf[ALPHALEN + 1];
	size_t i;
	struct keys tmp[ALPHALEN];


	/* Convert the key, which is in format which is optimized for
	 * deciphering into format that was used for enciphering. This is done
	 * because it means that if there is a keyword for the cipher it will
	 * be visible */
	for (i = 0; i < ALPHALEN; i++) {
		tmp[i].deciph_key = key[i];
		tmp[i].enciph_key = i;
	}
	qsort(tmp, ALPHALEN, sizeof(struct keys), &keys_cmp);

	for (i = 0; i < ALPHALEN; i++)
		buf[i] = uint_to_char(tmp[i].enciph_key);
	buf[ALPHALEN] = '\0';

	printf("Key = %s", buf);
}


static uint char_to_uint(char c)
{
	switch (c) {
	case 'A': case 'a': return 0;
	case 'B': case 'b': return 1;
	case 'C': case 'c': return 2;
	case 'D': case 'd': return 3;
	case 'E': case 'e': return 4;
	case 'F': case 'f': return 5;
	case 'G': case 'g': return 6;
	case 'H': case 'h': return 7;
	case 'I': case 'i': return 8;
	case 'J': case 'j': return 9;
	case 'K': case 'k': return 10;
	case 'L': case 'l': return 11;
	case 'M': case 'm': return 12;
	case 'N': case 'n': return 13;
	case 'O': case 'o': return 14;
	case 'P': case 'p': return 15;
	case 'Q': case 'q': return 16;
	case 'R': case 'r': return 17;
	case 'S': case 's': return 18;
	case 'T': case 't': return 19;
	case 'U': case 'u': return 20;
	case 'V': case 'v': return 21;
	case 'W': case 'w': return 22;
	case 'X': case 'x': return 23;
	case 'Y': case 'y': return 24;
	case 'Z': case 'z': return 25;
	default:
		return -1;
	}
}

static int uint_to_char(uint n)
{
	switch (n) {
	case 0: return 'A';
	case 1: return 'B';
	case 2: return 'C';
	case 3: return 'D';
	case 4: return 'E';
	case 5: return 'F';
	case 6: return 'G';
	case 7: return 'H';
	case 8: return 'I';
	case 9: return 'J';
	case 10: return 'K';
	case 11: return 'L';
	case 12: return 'M';
	case 13: return 'N';
	case 14: return 'O';
	case 15: return 'P';
	case 16: return 'Q';
	case 17: return 'R';
	case 18: return 'S';
	case 19: return 'T';
	case 20: return 'U';
	case 21: return 'V';
	case 22: return 'W';
	case 23: return 'X';
	case 24: return 'Y';
	case 25: return 'Z';
	default:
		return char_error;
	}
}