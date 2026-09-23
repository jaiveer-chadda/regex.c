/// @file main.c

#include <stdio.h>

#include "types/types.h"
#include "parsing/parse.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunused-parameter"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

const char REGEX[] = "a";

const char INPUT[] =
	"If you may please to think I love the King, And through him what's nearest to him, which is Your "	"\n"
	"gracious self, embrace but my direction. If your more ponderous and settled project May suffer "	"\n"
	"alteration, on mine honour, I'll point you where you shall have such receiving As shall become"	"\n"
	" your Highness; where you may Enjoy your mistress, from the whom, I see, There's no disjunction "	"\n"
	"to be made but by, As heavens forfend! your ruin- marry her; And with my best endeavours in your"	"\n"
	" absence Your discontenting father strive to qualify,"
;

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

int main(const int argc, const char *const argv[]) {
	for (const char *c = INPUT; *c != '\0'; c++) putchar(*c);

	putchar('\n');
	return 0;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#pragma clang diagnostic pop

// spell:ignoreRegexp /(?<=\n#.+"-)W|\brx/g
