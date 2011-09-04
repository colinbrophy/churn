#include "utils.h"

#include <stdarg.h>

void error_term(const char* error_msg, ...)
{
	va_list args;

	fflush(stdout);
	va_start(args, error_msg);
	vfprintf(stderr, error_msg, args);
	va_end(args);

	exit(EXIT_FAILURE);
}

void* safe_malloc(size_t size)
{
	void* p = malloc(size);
	if (p == NULL)
		error_term("Out of memory\n");
	return p;
}

uint char_to_uint(char c)
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

int uint_to_char(uint n)
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

