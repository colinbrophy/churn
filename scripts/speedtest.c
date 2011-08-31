#include "../consts.h"
#include "../file_load.h"
#include <math.h>
#include <time.h>

#define TEXTLEN 4
#define FREQ_LEN_FOREACH 32
#define FREQ_DIMENSIONS 4
#define FREQ_FILE_LEN pow((double)ALPHALEN, (double)FREQ_DIMENSIONS)
#define CHUNK (1024 * 4)
#define FREQ_TBL_LEN (pow(FREQ_LEN_FOREACH, (FREQ_DIMENSIONS - 1)) * ALPHALEN)


const char* freq_file = "../data/tet";


void delete_buffer(struct buffer* buf)
{
	free(buf->data);
	free(buf);
}

struct buffer* load_file(const char* filename)
{
	FILE* f;
	struct buffer* buf;
	int ch;
	unsigned int size = 0;

	f = fopen(filename, "rb");

	if (f == NULL)
		return NULL;

	buf = malloc(sizeof(struct buffer));
	if (buf == NULL)
		goto exit;

#if 0
	/* Find filesize */
	if(fseek(f, 0, SEEK_END) != 0)
		goto error_bufdata;

	buf->len = ftell(f);
	rewind(f);

	buf->data = malloc(buf->len + 1); /* Spare byte used for null terminating */
	if (buf->data == NULL)
		goto error_bufdata;

	if (fread(buf->data, buf->len, 1, f) != 1)
		goto error_read;
#endif

	buf->len = 0;
	buf->data = malloc(CHUNK);
	if (buf->data == NULL)
		goto error_bufdata;

	while ((ch = fgetc(f)) != EOF) {
		size++;
		if (size == CHUNK) {
			buf->len += CHUNK;
			buf->data = realloc(buf->data, buf->len + CHUNK);
			if (buf->data == NULL)
				goto error_read;
			size = 0;
		}
		((char*) buf->data)[buf->len + size -1] = ch;
	}
	if (ferror(f))
		goto error_read;
	buf->len += size;

exit:
	fclose(f);
	return buf;

error_read:
	free(buf->data);

error_bufdata:
	free(buf);
	buf = NULL;
	goto exit;

}

/* Loads freq table
 * returns freq table
 * No error codes: terminates if error */
static suint* create_freq_tbl()
{
	struct buffer* buf;
	suint* freq;
	size_t x, y, z, i;
	size_t ind, bufind;

	buf = load_file(freq_file);
	if (buf == NULL)
		return NULL;
	freq = buf->data;

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
					 * ((x * l + y) * l + z) * l + i */
					ind  = (x * FREQ_LEN_FOREACH + y) * FREQ_LEN_FOREACH + z;
					ind *= FREQ_LEN_FOREACH;
					ind += i;

					bufind  = (ALPHALEN * x + y) * ALPHALEN + z;
					bufind *= ALPHALEN;
					bufind += i;

					freq[ind] = ((char*) buf->data)[bufind];
				}

exit:
	delete_buffer(buf);
	return freq;
#else

exit:
	free(buf);
	return freq;
#endif
}

static uint get_score(const suint* text, size_t len, const suint* freq)
{
	uint score;
	size_t i;
	size_t index;

	score = 0;
	len -= 3;

	for(i = 0; i < len; i++) {
		/* Calculate the index
		 * Does this by doing:
		 * l = Array Dimension e.g. 32
		 * xl^3 + yl^2 + zl + i
		 * This is then factorized to make:
		 * ((x * l + y) * l + z) * l + i */
		index = text[i] * FREQ_LEN_FOREACH;
		index = (index + text[i + 1]) * FREQ_LEN_FOREACH;
		index = (index + text[i + 2]) * FREQ_LEN_FOREACH;
		index += text[i + 3];

		score += freq[index];
	}

	return score;
}

int main()
{
	int x,y,z,i;
	clock_t start;
	suint* freq;
	double time_in_seconds;
	suint text[TEXTLEN] = { 0};
	int tmp = 0;
	int times;

	freq = create_freq_tbl();
	start = clock();

	for (times = 0; times < 1000; times++) {
		for (x = 0; x < ALPHALEN; x++) {
			for (y = 0; y < ALPHALEN; y++) {
				for (z = 0; z < ALPHALEN; z++) {
					for (i = 0; i < ALPHALEN; i++) {
						tmp += get_score(text, TEXTLEN, freq);
						text[3]++;
					}
					text[2]++;
					text[3] = 0;
				}
				text[1]++;
				text[2] = 0;
			}
			text[0]++;
			text[1] = 0;
		}
		text[0] = 0;
	}

	time_in_seconds = (double)(clock() - start);

	printf("%d\n", tmp);
	printf("%f\n", time_in_seconds);

	return EXIT_SUCCESS;
}
