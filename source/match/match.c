/// @file match/match.c

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "match.h"
#include "output/print.h"
#include "errors/errors.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline bool rx_match_token(const token_t token, const char chr);
static inline ssize_t rx_match_from_char(const RxTokens tokens, const char *const from_chr);

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

match_t rx_match(rxobj_t rx_obj, const char *const string) {
	match_t matches = {0};
	const RxTokens tokens = rx_obj->tokens;

	for (const char *chr = string; *chr != '\0'; chr++) {
		const ssize_t match_len = rx_match_from_char(tokens, chr);

		if (match_len != -1) {
			/* successfully matched */
			/* add match to `matches` array */
			(void)matches;
		}
	}

	return (match_t){0};
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define token (tokens.arr[ti])

static inline ssize_t rx_match_from_char(const RxTokens tokens, const char *const from_chr) {
	const char *chr;
	size_t ti = 0;

	for (chr = from_chr; *chr != '\0'; chr++) {
		const bool matched = rx_match_token(token, *chr);

		if (!matched) return -1;
		if (matched && ++ti == tokens.len) break;
	}

	if (ti != tokens.len) return -1;

	return (ssize_t)(chr - from_chr) + 1;
}

#undef token

/*
	for (size_t ti = 0; ti != tokens.len; ti++) {
		size_t lti = ti; // local `ti`
		//
		for (; *chr != '\0'; chr++) {
			const token_t token = tokens.arr[lti];
			const bool matched = rx_match_token(token, *chr);
			//
			DEBUG_MATCH(token, chr, matched);
			//
			if (matched) {
				if (lti++ >= tokens.len - 1) goto break_all;
			}
		}
		//
		putchar('\n');
	}
*/


/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline bool rx_match_token(const token_t token, const char chr) {
	bool did_match = false;

	switch (token.type) {
		case RXT_LITERAL:
			did_match = chr == (char)token.value;
			break;

		case RXT_CLASS	: break;
		case RXT_OR		: break;
		case RXT_ANCHOR	: break;
		case RXT_BACKREF: break;
		case RXT_GROUP	: break;
		case RXT_SET	: break;
		case RXT_QUANT	: break;

		[[fallthrough]]; case RXT_RANGE: case RXT_INVALID: default:
			error_impossible_case();
	}

	return did_match;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
