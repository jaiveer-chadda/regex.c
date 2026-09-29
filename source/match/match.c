/// @file match/match.c

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "match.h"
#include "output/print.h"
#include "errors/errors.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#ifdef DEBUG_MODE
#	define DEBUG_MATCH(token, pchar, bmatch) do {										\
		printf("\t\33[3%dm'%c' %c= { ", (bmatch) + 1, *(pchar), (bmatch) ? '=' : '!');	\
		print_token((token), false);													\
		printf("\33[3%dm }\33[m \n", (bmatch) + 1);										\
	} while (0)
#	define DEBUG_LEN(len) do { if (len != -1) printf("\t[%zd]\n", (len)); } while (0)
#	define DEBUG_PUTS(str) fputs((str), stdout)
#else
#	define DEBUG_MATCH(token, pchar, bmatch) (void)(token), (void)(pchar), (void)(bmatch)
#	define DEBUG_LEN(len) (void)(len)
#	define DEBUG_PUTS(str) (void)(str)
#endif

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline bool rx_match_token(const token_t token, const char chr);
static inline ssize_t rx_match_from_char(const RxTokens tokens, const char *const from_chr);

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

match_t rx_match(rxobj_t rx_obj, const char *const string) {
	match_t matches = {0};
	const RxTokens tokens = rx_obj->tokens;

	// iterate through the test string, trying to find a match starting from each character
	for (const char *chr = string; *chr != '\0'; chr++) {
		const ssize_t match_len = rx_match_from_char(tokens, chr);
		DEBUG_LEN(match_len);

		// if we didn't find a match, then move on, and start trying to find a match starting from the next character
		if (match_len == -1) continue;

		// -- successfully matched --
		(void)matches; // add match to `matches` array
	}

	(void)rx_match_from_char;
	return (match_t){0};
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline ssize_t rx_match_from_char(const RxTokens tokens, const char *const from_chr) {
	DEBUG_PUTS("     ->");
	size_t ti = 0;

	const char *chr;
	for (chr = from_chr; *chr != '\0'; chr++) {
		const token_t token = tokens.arr[ti];

		// check if this character can be matched by this token
		const bool matched = rx_match_token(token, *chr);
		DEBUG_MATCH(token, chr, matched);

		// if we couldn't find a match, then move on, and start testing from the next character
		if (!matched) return -1;

		// every iteration, check if we've matched something,
		//	if we have, then increment `ti`, so that we can test against the next token
		// if we've reached the end of the tokens
		if (matched && ++ti == tokens.len) break;
	}

	// ensure that the match has been completed - i.e., all tokens have been parsed
	if (ti != tokens.len) return -1;
	// if all tokens _have_ been parsed, then calculate the match's length and return
	return (ssize_t)(chr - from_chr) + 1;
}

#undef token

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline bool rx_match_token(const token_t token, const char chr) {
	switch (token.type) {
		case RXT_LITERAL: return chr == (char)token.value;
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

	return false;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
