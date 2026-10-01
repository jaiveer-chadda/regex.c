/// @file main.c

#include <stdio.h>

#include "types/types.h"
#include "match/match.h"
#include "debug/debug.h"
#include "output/print.h"
#include "parsing/parse.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunused-parameter"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

// const char REGEX[] = "^[-\\w]+|[0-9]*@\\w+?.(?<group>Mail|male|meal\\))\\b(\\.(\\x40e)?[a-z]{2,}){1,3}$";
// const char REGEX[] = "([a-z]+)(\\d)?";
const char REGEX[] = "a?0?";
// const char REGEX[] = "[a-z]?";
// const char REGEX[] = "a()5";

const char INPUTS[][64] = {
	"a",
	"a ",
	"a0",
	// "a5",
	// "0 ",
	// "12345",
	// "xxabc",
	// "  12a45",
	// "a5",
	// "12abc34def5",
	// "12abc3p0def5",
	// "xyza1b2c3",
	// "12 45",
	// "abcde  ",
};

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

int main(const int argc, const char *const argv[]) {
	puts(REGEX);

	const rxobj_t regex = rx_compile(REGEX, RX_FLAGS[RXF_GLOBAL].bf | RX_FLAGS[RXF_MULTILINE].bf);

	fputs("regex = ", stdout); print_regex(regex); putchar('\n');

	for (size_t i = 0; i < sizeof(INPUTS) / sizeof(INPUTS[0]); i++) {
		DEBUG_INPUT(i);

		const matches_t matches = rx_match(regex, INPUTS[i]);
		rx_print_matches(matches);
	}

	return 0;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#pragma clang diagnostic pop

// spell:ignoreRegexp /(?<=\n#.+"-)W|\brx|(?<=\n\t*(// )*")[^"]+?(?=",?\n)/g
