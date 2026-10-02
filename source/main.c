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

const char REGEX[] = "a";
const char INPUTS[][64] = {
	"b",
	"ab c",
	" abc ",

	// "aabc",
	// "a",
	// "a ",
	// "aaa",
	// "a0",
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

	for (size_t i = 0; i < sizeof(INPUTS) / sizeof(INPUTS[0]); i++) {
		DEBUG_INPUT(i);

		const matches_t matches = rx_match(regex, INPUTS[i]);
		rx_print_matches(matches);
	}

	dline(60);
	return 0;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#pragma clang diagnostic pop

// spell:ignoreRegexp /(?<=\n#.+"-)W|\brx|(?<=\n\t*(// )*")[^"]+?(?=",?\n)/g
