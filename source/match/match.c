/// @file match/match.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "match.h"
#include "output/print.h"
#include "errors/errors.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#ifdef DEBUG_MODE
#	define DEBUG_MATCH(token, pchar, mlen) do {		\
		const bool bmatch = mlen != -1;				\
		printf("\t[%ld] \33[3%dm'%c' %c= { ",		\
			(pchar) - from_chr,						\
			(bmatch) + 1,							\
			*(pchar),								\
			(bmatch) ? '=' : '!'					\
		);											\
		print_token((token), false);				\
		printf("\33[3%dm }\33[m \n", (bmatch) + 1);	\
	} while (0)
#	define DEBUG_LEN(len) do { if (len != -1) printf("\t    len = [%zd]\n", (len)); } while (0)
#	define DEBUG_PUTS(str) fputs((str), stdout)
#else
#	define DEBUG_MATCH(token, pchar, bmatch) (void)(token), (void)(pchar), (void)(bmatch)
#	define DEBUG_LEN(len) (void)(len)
#	define DEBUG_PUTS(str) (void)(str)
#endif

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline ssize_t rx_match_token(const token_t token, const char *chr);
static inline ssize_t rx_match_from_char(const RxTokens tokens, const char *const from_chr);

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

matches_t rx_match(const rxobj_t rx_obj, const char *const string) {
	matches_t matches = { .arr = NULL, .len = 0, .string = string };
	size_t alloc_count = 0;

	// iterate through the test string, trying to find a match starting from each character
	for (const char *chr = string; *chr != '\0'; chr++) {
		const ssize_t match_len = rx_match_from_char(rx_obj->tokens, chr);
		DEBUG_LEN(match_len);

		// if we didn't find a match, then move on, and start trying to find a match starting from the next character
		if (match_len == -1) continue;

		/* ———————————————————————————————————————————————————— */
		// -- successfully matched --

		// allocate memory for `matches.arr` as needed
		REALLOC_FOR(matches.arr, matches.len, alloc_count, match_t);
		// add match to `matches` array
		matches.arr[matches.len++] = (match_t){ .idx = (size_t)(chr - string), .len = match_len };

		// increment the char pointer by the match len, so we don't get overlapping matches 
		chr += (intptr_t)match_len - 1;
	}

	return matches;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline ssize_t rx_match_from_char(const RxTokens tokens, const char *const from_chr) {
	DEBUG_PUTS(" —————→");
	size_t ti = 0;

	/* ———————————————————————————————————————————————————— */

	const char *chr = from_chr;
	while (*chr != '\0') {
		const token_t token = tokens.arr[ti];

		// check if this character can be matched by this token
		const ssize_t match_len = rx_match_token(token, chr);
		DEBUG_MATCH(token, chr, match_len);

		// if we couldn't find a match, then move on, and start testing from the next character
		if (match_len == -1) return -1;

		/* ———————————————————————————————————————————————————— */

		// if the match was successful, increment the character pointer by
		chr += (intptr_t)match_len;

		// if we've matched something, increment `ti` so that we can test the next char against the next token
		// if we've reached the end of the tokens, we've found a match, so break and return
		if (++ti == tokens.len) break;
	}

	/* ———————————————————————————————————————————————————— */

	// ensure that the match has been completed - i.e., all tokens have been parsed
	if (ti != tokens.len) return -1;
	// if all tokens _have_ been parsed, then calculate the match's length and return
	return (ssize_t)(chr - from_chr);
}

#undef token

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"

#define RETURN_BOOL(test) return ((test) ? 1 : -1)

static inline ssize_t rx_match_token(const token_t token, const char *chr) {
	if (*chr == '\0') return -1;

	switch (token.type) {
		case RXT_LITERAL: RETURN_BOOL(*chr ==  (char)token.value);
		case RXT_CLASS	: RETURN_BOOL(rx_match_class(token.value, *chr));

		case RXT_ANCHOR: {
			const char anchor = (char)token.value;
			return -1;
		}

		case RXT_BACKREF: {
			const groupid_t *const groupref = (groupid_t*)token.value;
			return -1;
		}

		case RXT_QUANT: {
			const RxQuantToken *const quant = (RxQuantToken*)token.value;
			size_t i = 0;

			// try and match the token the maximum number of times specified by the quantifier
			for (; i < quant->rhs; i++) {
				// if at any point it fails to match...
				if (rx_match_token(quant->repeat, chr + i) == -1) {
					// check if we're still within the bounds of the minimum repetition count (the lhs)
					//	if we are, then we haven't done enough iterations - return failure
					if (i < quant->lhs) return -1;
					// if, however, we're trying to match something _after_ we've passed the minimum rep count
					//	then there's nothing to be done - break out of the loop, and return the match's length
					break;
				}
			}

			return (ssize_t)i; // return the number of chars that were (successfully) parsed
		}

		case RXT_OR: {
			const RxOrToken *const or_sections = (RxOrToken*)token.value;
			return -1;
		}

		case RXT_GROUP: {
			const RxGroupToken *const group = (RxGroupToken*)token.value;
			return -1;
		}

		case RXT_SET: {
			const RxSetToken *const set = (RxSetToken*)token.value;

			// iterate through all of the set's tokens, checking each one for a match
			for (size_t i = 0; i < set->tokens.len; i++) {
				if (rx_match_token(set->tokens.arr[i], chr) != -1) {
					// if we find a match and we're in inverse mode, then return failure
					// if we find a match and we're _not_ in inverse mode, return success
					return set->is_inverse ? -1 : 1;
				}
			}

			// if we reached the end in inverse mode, it's a success, and in normal mode, a failure
			return set->is_inverse ? 1 : -1;
		}

		case RXT_RANGE: {
			const RxRangeToken *const range = (RxRangeToken*)token.value;
			RETURN_BOOL(range->lhs <= *chr && *chr <= range->rhs);
		}

		/* ———————————————————————————————————————————————————— */

		[[fallthrough]]; case RXT_INVALID: default:
			error_impossible_case();
			return -1; // unreachable
	}
}

#pragma clang diagnostic pop

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

// spell:ignoreRegExp /(?<=\n#.+"-)W/g
