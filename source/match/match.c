/// @file match/match.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "match.h"
#include "debug/debug.h"
#include "output/print.h"
#include "errors/errors.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define MATCHED(len) ((len) != -1L)

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline ssize_t rx_match_token(
	const token_t token, const char *chr, matches_t *const matches, const int depth);
static inline ssize_t rx_match_tokens(
	const RxTokens tokens, const char *const chr, matches_t *const matches, const int depth);

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

matches_t rx_match(const rxobj_t rx_obj, const char *const string) {
	dmatch_init(string);

	size_t alloc_count = 0;
	matches_t matches = {
		// allocate memory for the captured string of each of the capturing groups
		.captures = calloc(rx_obj->capture_count, sizeof(char*)),
		.num_cap = rx_obj->capture_count,
		.string = string,
		.arr = NULL,
		.len = 0,
	};

	// iterate through the test string, trying to find a match starting from each character
	for (const char *chr = string; *chr != '\0'; chr++) {
		const ssize_t match_len = rx_match_tokens(rx_obj->tokens, chr, &matches, 0);
		dmatch_len(chr, match_len);

		// if we didn't find a match, then move on, and start trying to find a match starting from the next character
		if (!MATCHED(match_len)) continue;

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

static inline ssize_t rx_match_tokens(
	const RxTokens tokens, const char *chr, matches_t *const matches, const int depth
) {
	const char *const start = chr;
	size_t ti = 0;

	/* ———————————————————————————————————————————————————— */

	while (*chr != '\0') {
		const token_t token = tokens.arr[ti];

		// check if this character can be matched by this token
		const ssize_t match_len = rx_match_token(token, chr, matches, depth + 1);

		// if we couldn't find a match, then move on, and start testing from the next character
		if (!MATCHED(match_len)) return -1;

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
	return (ssize_t)(chr - start);
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#ifdef DEBUG_MODE
#	define RETURN			 goto token_return
#	define RETURN_VALUE(...) do { __VA_OPT__(match_len = (ssize_t)(__VA_ARGS__)); RETURN; } while (0)
#	define RETURN_FAILURE()	 do { match_len = -1; RETURN; } while (0)
#else
#	define RETURN			 return match_len
#	define RETURN_VALUE(...) RETURN __VA_OPT__(= (ssize_t)(__VA_ARGS__))
#	define RETURN_FAILURE()	 return -1
#endif

#define RETURN_SUCCESS		RETURN_VALUE
#define RETURN_BOOL(test)	RETURN_VALUE((test) ? 1 : -1)

#define IS_EMPTY(tks) ((tks).len == 1 && (tks).arr[0].type == RXT_EMPTY)

/* ————————————————————————————————————————————————————————————————————— */

static inline ssize_t rx_match_token(const token_t token, const char *chr, matches_t *const matches, const int depth) {
	ssize_t match_len = -1;

	if (*chr == '\0') RETURN_FAILURE();

	switch (token.type) {

		/* ———————————————————————————————————————————————————— */

		case RXT_LITERAL: RETURN_BOOL(*chr == (char)token.value);
		case RXT_CLASS	: RETURN_BOOL(rx_match_class(token.value, *chr));

		/* ———————————————————————————————————————————————————— */

		case RXT_ANCHOR: {
			const char anchor = (char)token.value;
			RETURN_FAILURE();
			(void)anchor;
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_BACKREF: {
			const groupid_t *const groupref = (groupid_t*)token.value;
			RETURN_FAILURE();
			(void)groupref;
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_QUANT: {
			const RxQuantToken *const quant = (RxQuantToken*)token.value;
			const char *const start = chr, *pchar = chr;

			// try and match the token the maximum number of times specified by the quantifier
			for (size_t count = 0; count < quant->rhs; count++) {
				match_len = rx_match_token(quant->repeat, pchar, matches, depth + 1);

				// if at any point it fails to match...
				if (!MATCHED(match_len)) {
					// check if we're still within the bounds of the minimum repetition count (the lhs)
					//	if we are, then we haven't done enough iterations - return failure
					if (count < quant->lhs) RETURN_FAILURE();
					// if, however, we're trying to match something _after_ we've passed the minimum rep count
					//	then there's nothing to be done - break out of the loop, and return the match's length
					break;
				}

				pchar += match_len; // move the char pointer forward by the length of the match
			}

			RETURN_SUCCESS(pchar - start); // return the number of chars that were (successfully) parsed
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_OR: {
			const RxOrToken *const or_sects = (RxOrToken*)token.value;

			// iterate through each of the sections in the 'or' object
			for (size_t i = 0; i < or_sects->count; i++) {
				// for empty 'or' sects `(|...)` short-circuit the matching func, and return a match w/ a length of 1
				if (IS_EMPTY(or_sects->sections[i])) RETURN_SUCCESS(true);

				match_len = rx_match_tokens(or_sects->sections[i], chr, matches, depth + 1);
				if (MATCHED(match_len)) RETURN_SUCCESS();
			}

			RETURN_FAILURE(); // none of the 'or' sections matched
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_GROUP: {
			const RxGroupToken *const group = (RxGroupToken*)token.value;

			match_len = (
				// if the group is empty, short-circuit the `rx_match_tokens` function, setting the length to 0
				IS_EMPTY(group->tokens)
				// otherwise, find the match & its length as normal
				? 0 : rx_match_tokens(group->tokens, chr, matches, depth + 1)
			);

			if (MATCHED(match_len)) { // if we didn't find a match, don't capture anything
				// copy the match into the `captures` array, assigning it to the index of this group
				//	also, allocate one more byte than the match's length, so `calloc` can include a nullbyte at the end
				matches->captures[group->id.idx] = memcpy(calloc(1, match_len + 1), chr, match_len);
			}

			RETURN_SUCCESS();
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_SET: {
			const RxSetToken *const set = (RxSetToken*)token.value;

			// iterate through all of the set's tokens, checking each one for a match
			for (size_t i = 0; i < set->tokens.len; i++) {
				if (MATCHED(rx_match_token(set->tokens.arr[i], chr, matches, depth + 1))) {
					// if we find a match and we're in inverse mode, then return failure
					// if we find a match and we're _not_ in inverse mode, return success
					RETURN_VALUE(set->is_inverse ? -1 : 1);
				}
			}

			// if we couldn't find a match in inverse mode, it's a success, and in normal mode, a failure
			RETURN_VALUE(set->is_inverse ? 1 : -1);
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_RANGE: {
			const RxRangeToken *const range = (RxRangeToken*)token.value;
			// simply check whether a character is between the two sides of the range
			RETURN_BOOL(range->lhs <= *chr && *chr <= range->rhs);
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_EMPTY: {
			// this shouldn't only ever be accessed directly when the entire regex is empty
			//	each token should have its own way of dealing with `RXT_EMPTY` cases
			RETURN_SUCCESS(true);
		}

		/* ———————————————————————————————————————————————————— */

		[[fallthrough]]; case RXT_INVALID: default: {
			error_impossible_case();
			break; // unreachable
		}
	}

	/* ———————————————————————————————————————————————————— */

	#ifdef DEBUG_MODE
		token_return: {
			dmatch(token, chr, match_len, depth);
			return match_len;
		}
	#else
		return -1;
	#endif
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

// spell:ignoreRegExp /(?<=\n#.+"-)W/g
