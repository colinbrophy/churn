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
#include <assert.h>

#include "file_load.h"
#include "init_key.h"
#include "utils.h"

#define CHURNLEN 100
#define FREQ_LEN_FOREACH 2*2*2*2*2 /* 2 to the power 5*/
#define FREQ_TBL_LEN (pow(FREQ_LEN_FOREACH, (FREQ_DIMENSIONS - 1)) * ALPHALEN)

static const char* freq_file  = "data/tet";
static const int maxkeys = 70000;
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

static void decipher(suint* dest, const suint* src, size_t len,
	const suint* key)
{
	size_t i;

	for (i = 0; i < len; i++) {
		suint sc = src[i];
	    suint sk = key[sc];
		dest[i] = sk;
	}
}

static int get_score2(const suint* freq, int* score_cache, int len,
		const suint* key, int oldscore, const suint* ciph, int key1, int key2) {
	int i;
	for (i = 0; i < len - 3; i++)
		/* Has the letter changed? */
		if (ciph[i] == key1 || ciph[i] == key2) {
			size_t index = key[ciph[i]] * FREQ_LEN_FOREACH * FREQ_LEN_FOREACH * FREQ_LEN_FOREACH;
			index += key[ciph[i + 1]] * FREQ_LEN_FOREACH * FREQ_LEN_FOREACH;
			index += key[ciph[i + 2]] * FREQ_LEN_FOREACH;
			index += key[ciph[i + 3]];
			oldscore += freq[index] - score_cache[i];
			score_cache[i] = freq[index];
		}

	return oldscore;
}

static int get_score(const suint* ciph, size_t len, const suint* freq, const suint* key)
{
	size_t tmp;
	size_t score;
	size_t i;
	size_t index;

	score = 0;
	/* Calculate the index
	 * Does this by doing:
	 * l = Array Dimension e.g. 32
	 * xl^3 + yl^2 + zl + i
	 * This is then factorized to make:
	 * l(l(lx + y) + z) + i */

	index = key[ciph[len - 4]] * FREQ_LEN_FOREACH * FREQ_LEN_FOREACH * FREQ_LEN_FOREACH;
	index += key[ciph[len - 3]] * FREQ_LEN_FOREACH * FREQ_LEN_FOREACH;
	index += key[ciph[len - 2]] * FREQ_LEN_FOREACH;
	index += key[ciph[len - 1]];


	for(i = len - 3; i > 0; i--) {
		score += freq[index];
		index >>= 5; /* 32 bits */
		tmp = ciph[i];
		tmp = key[tmp];
		tmp *= FREQ_LEN_FOREACH * FREQ_LEN_FOREACH * FREQ_LEN_FOREACH;
	    index += tmp;
	}
	score += freq[index];

	index >>= 5; /* 32 bits */
	tmp = ciph[i];
	tmp = key[tmp];
	tmp *= FREQ_LEN_FOREACH * FREQ_LEN_FOREACH * FREQ_LEN_FOREACH;
	index += tmp;
	score += freq[index];

	return score;
}

int main(int argc, char** argv)
{
	struct buffer* ciph;
	suint* ciphtxt;
	suint* plain;
	suint parent[ALPHALEN];
	suint child[ALPHALEN];
	size_t i;
	suint* freq;

	int parentscore;
	int childscore;
	int bestscore;
	int modifed_churn[CHURNLEN];

	int nok = 0;
	int minscore = 0;
	int keys_since_hit = 0;

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

	/* Normalize to length of text */
	for (i = 0; i < CHURNLEN; i++) {
		double fract = ciph->len / ((double) churn_cipherlen);
		modifed_churn[i] = round(churn_range[i] * fract);
	}

	minscore = min_bestscore_per_char * ciph->len;
	bestscore = minscore;

	srand(time(NULL));

	decipher(plain, ciphtxt, ciph->len, parent);

	parentscore = get_score(ciphtxt, ciph->len, freq, parent);

	for (;;) {
		size_t rand1;
		size_t rand2;
		int tmp;

		memcpy(child, parent, sizeof(child));

		/* We swap two randow chars from the parent to form the child */
		do {
			rand1 = rand() % ALPHALEN;
			rand2 = rand() % ALPHALEN;
		} while(rand1 == rand2);
		tmp = child[rand1];
		child[rand1] = child[rand2];
		child[rand2] = tmp;

		childscore = get_score(ciphtxt, ciph->len, freq, child);

		rand1 = rand() % CHURNLEN;
		if (childscore > (parentscore - modifed_churn[rand1])) {
			parentscore = childscore;
			memcpy(parent, child, sizeof(child));

			if (childscore > bestscore) {
				decipher(plain, ciphtxt, ciph->len, child);
				print_key(child);
				printf("\nScore = %d\n", childscore);
				printf("Number of keys = %d\n", nok);
				printf("Keys since last hit = %d\n", keys_since_hit);
				print_plain(plain, ciph->len);

				bestscore = childscore;
				keys_since_hit = 0;
		} else if (++keys_since_hit == maxkeys && bestscore > minscore)
				break;
		}
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
	int i;
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
	assert(isalpha(c));
	return toupper(c) - 'A';
}

static int uint_to_char(uint n)
{
	assert(n <= 25);
	return n + 'A';
}
