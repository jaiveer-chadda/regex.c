/// @file main.c

#include <stdio.h>
#include <string.h>

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

typedef char str[];

const char LOREM[] = "Lorem ipsum dolor sit amet consectetur adipiscing elit, sed do eius modi tempora incididunt, "
	"ut labore et dolore magnam aliquam quaerat voluptatem. Ut enim ad minima veniam, quis nostrum exercitationem "
	"ullam corporis suscipit laboriosam, nisi ut aliquid ex ea commodi consequat? Duis aute irure reprehenderit, qui "
	"in ea voluptate velit esse, quam nihil molestiae consequatur, vel illum, qui dolorem eum fugiat, quo voluptas "
	"nulla pariatur?";

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define chrToStr(chr) ((str){ (chr), '\0' })

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline int printMatch(const char *const match, const size_t len, const char *const col__) {
	printf("\33[%sm%.*s%s", col__, (int)len, match, "\33[m");
	fflush(stdout);
	return (int)len;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

int main(const int argc, const char *const argv[]) {
	(void)argc, (void)argv;

	const char pat1 = 'e';
	const str  pat2 = "lo"; const size_t pat2len = sizeof(pat2) - 1;

	for (const char *chr = LOREM; *chr != '\0'; chr++) {
		if (strncmp(chr, pat2, pat2len) == 0) {
			int len = printMatch(chr, pat2len, "33") - 1;

			while (*chr != '\0' && len > 0) chr++, len--;
			if (*chr == '\0') break;

			continue;
		}

		if (*chr == pat1) {
			printMatch(chrToStr(*chr), 1, "31");
			continue;
		}

		putchar(*chr);
	}

	putchar('\n');

	return 0;
}
