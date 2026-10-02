/// @file output/matches.c

#include <stdio.h>
#include <assert.h>

#include "print.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define RESET		"\33[m"
#define COL_MATCH	"\33[1;4;91m"
#define COL_STRING	"\33[38;5;147m"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline void rx_print_match(const match_t match, const char *const string);

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

void rx_print_matches(const matches_t matches) {
	printf("\n'\33[38;5;194m%s"RESET"' (%zu match%s)\n", matches.string, matches.len, matches.len == 1 ? "" : "es");

	for (size_t i = 0; i < matches.len; i++) {
		rx_print_match(matches.arr[i], matches.string);
	}

	for (size_t i = 0; i < matches.num_cap; i++) {
		const char *const capture = matches.captures[i];
		if (capture == NULL) continue;
		printf("group [%zu] = '%s'\n", i, capture);
	}
	// putchar('\n');
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline void rx_print_match(const match_t match, const char *const string) {
	printf(
		"'"
		"%s%.*s%s"
		"%s%.*s%s"
		"%s%s%s"
		"' (idx %zu -> %zu : %zu chars)\n",

		COL_STRING,	(int)match.idx,	string,							RESET,
		COL_MATCH ,	(int)match.len,	string + match.idx,				RESET,
		COL_STRING,					string + match.idx + match.len,	RESET,

		match.idx,	match.idx + match.len,	match.len
	);
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
