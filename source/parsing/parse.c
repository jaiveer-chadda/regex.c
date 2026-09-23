/// @file parsing/parse.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "types/types.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define memdup(src, size) memcpy(malloc((size)), (src), (size))

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

struct rx__regex {
	void **symbols;

	const char *string;
	uint64_t flags;
};

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline regex_t rx_init(const char *const string, const uint64_t flags) {
	const regex_t rx_obj = calloc(1, sizeof(struct rx__regex));
	const size_t str_len = strlen(string) + 1;

	*rx_obj = (struct rx__regex){
		.symbols = malloc(str_len),
		.string = memdup(string, str_len),
		.flags = flags,
	};

	return rx_obj;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

regex_t rx_compile(const char *const string, const uint64_t flags) {
	const regex_t rx_obj = rx_init(string, flags);

	for (const char *chr = rx_obj->string; *chr != '\0'; chr++) {
		putchar(*chr);
	}

	putchar('\n');
	return rx_obj;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
