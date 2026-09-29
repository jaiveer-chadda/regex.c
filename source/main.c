/// @file main.c

#include <stdio.h>

#include "types/types.h"
#include "match/match.h"
#include "output/print.h"
#include "parsing/parse.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunused-parameter"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

// const char REGEX[] = "^[-\\w]+|[0-9]*@\\w+?.(?<group>Mail|male|meal\\))\\b(\\.(\\x40e)?[a-z]{2,}){1,3}$";
const char REGEX[] = "abc";

const char INPUTS[][32] = {
	"abc",
	" abc ",
	"a bc",
	"ab c",
	"abc abc",
	"cba",
};

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

int main(const int argc, const char *const argv[]) {
	const rxobj_t regex = rx_compile(REGEX, RX_FLAGS[RXF_GLOBAL].bf | RX_FLAGS[RXF_MULTILINE].bf);

	printf("regex = \"\33[4m%s\33[m\"\n", REGEX);
	print_regex(regex);

	for (size_t i = 0; i < sizeof(INPUTS) / sizeof(INPUTS[0]); i++) {
		rx_match(regex, INPUTS[i]);
	}

	return 0;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#pragma clang diagnostic pop

// spell:ignoreRegexp /(?<=\n#.+"-)W|\brx/g
