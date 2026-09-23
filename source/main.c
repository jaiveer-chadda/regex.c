/// @file main.c

#include <stdio.h>
#include "types/types.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

const char LOREM[] = "Lorem ipsum dolor sit amet consectetur adipiscing elit, sed do eius modi tempora incididunt, "
	"ut labore et dolore magnam aliquam quaerat voluptatem. Ut enim ad minima veniam, quis nostrum exercitationem "
	"ullam corporis suscipit laboriosam, nisi ut aliquid ex ea commodi consequat? Duis aute irure reprehenderit, qui "
	"in ea voluptate velit esse, quam nihil molestiae consequatur, vel illum, qui dolorem eum fugiat, quo voluptas "
	"nulla pariatur?";

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

int main(const int argc, const char *const argv[]) {
	(void)argc, (void)argv;

	const char *const string  = LOREM;
	const char *const pattern = "e[ltd]";

	const char *pat_ptr = pattern;
	(void)pat_ptr;

	for (const char *str_ptr = string; *str_ptr != '\0'; str_ptr++) {
		putchar(*str_ptr);
	}

	putchar('\n');
	return 0;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
