#include "file_load.h"
#include "utils.h"
#include <math.h>
#include <time.h>

#define TEXTLEN 4
#define FREQ_LEN_FOREACH 26
#define FREQ_DIMENSIONS 4
#define CHUNK (1024 * 4)
#define FREQ_TBL_LEN (pow(FREQ_LEN_FOREACH, (FREQ_DIMENSIONS - 1)) * ALPHALEN)

DEF_GET_SCORE(static , get_score ,FREQ_LEN_FOREACH)

static void safe_fgets(char *s, int size, FILE *stream)
{
	fgets(s, size, stream);
	if (ferror(stream)) {
		fprintf(stderr, "Error reading file \n");
		exit(EXIT_FAILURE);
	}
}

#define safe_gets(s, size) safe_fgets(s, size, stdin)

int main()
{
	int x,y,z,i;
	clock_t start;
	suint* freq;
	double time_in_seconds;
	suint text[TEXTLEN] = { 0};
	int tmp = 0;
	double times;
	char* buf;


	buf = safe_malloc(FREQ_TBL_LEN);
	freq = safe_malloc(sizeof(suint) * FREQ_TBL_LEN);

	safe_gets(buf, FREQ_TBL_LEN);
	for (i = 0; buf[i] != '\0'; i++)
		freq[i] =  buf[i] -'A';

	start = clock();
	for (times = 0; times < 500000000.0; times++) {
		for (x = 0; x < ALPHALEN; x++) {
			for (y = 0; y < ALPHALEN; y++) {
				for (z = 0; z < ALPHALEN; z++) {
					for (i = 0; i < ALPHALEN; i++) {
						tmp += get_score(text, TEXTLEN,
							freq);
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

	time_in_seconds = (double)(clock() - start) / (double) CLOCKS_PER_SEC;

	printf("%f\n", time_in_seconds);

	return EXIT_SUCCESS;
}
