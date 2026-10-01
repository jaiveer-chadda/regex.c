/// @file match/match.c

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "match.h"
#include "debug/debug.h"
#include "output/print.h"
#include "errors/errors.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define MATCHED(len)	  ((len) != -1L)
#define SPREAD_TOKS(toks) ((toks).arr), ((toks).len)

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define TOKEN_MATCH_ARGS		\
	const char *chr,			\
	matches_t *const matches,	\
	const int depth

static inline ssize_t rx_match_token(const token_t *const token, const ssize_t count, TOKEN_MATCH_ARGS);

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

matches_t rx_match(const rxobj_t rx_obj, const char *const string) {
	dmatch_init(string);

	size_t alloc_count = 0;
	matches_t matches = {
		// allocate memory for the captured string of each of the capturing groups
		.captures = calloc(rx_obj->capture_count, sizeof(char*)),
		// and note down the number of captures there should be
		.num_cap = rx_obj->capture_count,

		// initialise an empty match array
		.arr = NULL, .len = 0,

		// and copy a reference to the string that we're matching,
		//	so that once we have the matches, we know what their contents are
		.string = string,
	};

	/* ———————————————————————————————————————————————————— */

	// iterate through the test string, trying to find a match starting from each character
	for (const char *chr = string; *chr != '\0'; chr++) {
		const ssize_t match_len = rx_match_token(SPREAD_TOKS(rx_obj->tokens), chr, &matches, 0);
		dmatch_len(chr, match_len);

		// if we didn't find a match, then move on, and start trying to find a match starting from the next character
		if (!MATCHED(match_len)) continue;

		/* ———————————————————————————————————————————————————— */
		// -- successfully matched --

		// allocate memory for `matches.arr` as needed
		REALLOC_FOR(matches.arr, matches.len, alloc_count, match_t);
		// add match to `matches` array
		matches.arr[matches.len++] = (match_t){ .idx = (size_t)(chr - string), .len = match_len };

		/* ———————————————————————————————————————————————————— */

		// the match should never have gone past the end of the string
		assert(match_len <= (ssize_t)strnlen(chr, match_len));

		// increment the char pointer by the match len, so we don't get overlapping matches
		chr += (intptr_t)match_len - 1;

		// make sure that the char pointer is always moved forward by at least one every time
		if (match_len == 0) chr++; // this prevents an infinite number of zero-width matches
	}

	/* ———————————————————————————————————————————————————— */

	return matches;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#ifdef DEBUG_MODE
#	define RETURN(val) do { if (count >= 1) dmatch(token, chr, (val), depth); return (val); } while (0)
#else
#	define RETURN(val) return (val)
#endif

#define BACKTRACK() RETURN(-1)

/* ———————————————————————————————————————————————————— */

#define CHECK_SINGLE_CHAR(test_case) do {																\
	/* if it doesn't match, then there's nothing more to do */											\
	/*	therefore we've failed this branch, and we now have to backtrack (de-recurse) */				\
	if (!(test_case)) BACKTRACK();																		\
	const ssize_t match_len = 1; /* we matched, and by definition, the length of a single char is 1 */	\
	/*	(yes, ik that defining a constant to only use twice is verbose, but this is clearer to me) */	\
	\
	/* now that we've matched this token, check that all the tokens after it also matches */			\
	const ssize_t tail_len = rx_match_token(token + 1, count - 1, chr + match_len, matches, depth + 1);	\
	\
	/* if it does, then return (our length + its length) */												\
	if (MATCHED(tail_len)) RETURN(match_len + tail_len);												\
	else BACKTRACK(); /* and if not, then just return failure as usual */								\
} while (0)

/* ————————————————————————————————————————————————————————————————————— */

static inline ssize_t rx_match_token(const token_t *const token, const ssize_t count, TOKEN_MATCH_ARGS) {
	if (count ==  0) RETURN( 0);
	if (count == -1) RETURN(-1);

	/* ———————————————————————————————————————————————————— */

	switch (token->type) {

		case RXT_LITERAL: CHECK_SINGLE_CHAR(*chr == (char)token->value);
		case RXT_CLASS	: CHECK_SINGLE_CHAR(rx_match_class(token->value, *chr));

		/* ———————————————————————————————————————————————————— */

		case RXT_ANCHOR: {
			const char anchor = (char)token->value;
			error_not_implemented(); (void)anchor;
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_BACKREF: {
			const groupid_t *const groupref = (groupid_t*)token->value;
			error_not_implemented(); (void)groupref;
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_QUANT: {
			//r)NOT IMPLEMENTED
			const RxQuantToken *const quant = (RxQuantToken*)token->value;
			const char *const start = chr, *pchar = chr;

			// try and match the token the maximum number of times specified by the quantifier
			size_t count = 0;
			for (; count < quant->rhs; count++) {
				const ssize_t match_len = -1 /* rx_match_token(quant->repeat, pchar, matches, depth + 1) */;

				// if at any point it fails to match, break
				if (!MATCHED(match_len)) break;

				// check that moving the pointer forward won't move it past the end of the string
				if (match_len > (ssize_t)strnlen(pchar, match_len)) BACKTRACK();

				pchar += match_len; // move the char pointer forward by the length of the match
			}

			// check if we're still within the bounds of the minimum repetition count (the lhs)
			//	if we are, then we haven't done enough iterations - return failure
			if (count < quant->lhs) BACKTRACK();

			// if, however, we're trying to match something _after_ we've passed the minimum rep count
			//	then there's nothing to be done - just return the match's length
			RETURN(pchar - start); // return the number of chars that were (successfully) parsed
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_OR: {
			const RxOrToken *const or_sects = (RxOrToken*)token->value;

			// iterate through each of the sections in the 'or' object
			for (size_t i = 0; i < or_sects->count; i++) {
				// match the section against its own tokens first
				const ssize_t match_len = rx_match_token(SPREAD_TOKS(or_sects->sections[i]), chr, matches, depth + 1);
				if (!MATCHED(match_len)) continue;

				// then check if the tokens after this, all match
				const ssize_t tail_len = rx_match_token(token + 1, count - 1, chr + match_len, matches, depth + 1);

				// if they do, return success
				if (MATCHED(tail_len)) RETURN(match_len + tail_len);
				// if not, keep checking the rest of the sections
			}

			BACKTRACK(); // none of the 'or' sections matched
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_GROUP: {
			//r)NOT IMPLEMENTED
			const RxGroupToken *const group = (RxGroupToken*)token->value;

			const ssize_t match_len = (
				// if the group is empty, short-circuit the `rx_match_token` function, setting the length to 0
				false /* IS_EMPTY(group->tokens) */ ? 0
				// otherwise, find the match & its length as normal
				: rx_match_token(SPREAD_TOKS(group->tokens), chr, matches, depth + 1)
			);

			if (MATCHED(match_len)) { // if we didn't find a match, don't capture anything
				// copy the match into the `captures` array, assigning it to the index of this group
				//	also, allocate one more byte than the match's length, so `calloc` can include a nullbyte at the end
				matches->captures[group->id.idx] = memcpy(calloc(1, match_len + 1), chr, match_len);
			}

			RETURN(match_len);
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_SET: {
			const RxSetToken *const set = (RxSetToken*)token->value;

			// iterate through all of the set's tokens, checking each one for a match
			for (size_t i = 0; i < set->tokens.len; i++) {
				const ssize_t tail_len = rx_match_token(token + 1, count - 1, chr, matches, depth + 1);

				if (!MATCHED(tail_len)) continue;
				const ssize_t match_len = 1;

				// if we find a match and we're in inverse mode, then return failure
				if (set->is_inverse) BACKTRACK();
				// if we find a match and we're _not_ in inverse mode, return success
				else RETURN(match_len + tail_len);
			}

			// if we couldn't find a match in inverse mode, it's a success, and in normal mode, a failure
			CHECK_SINGLE_CHAR(set->is_inverse);
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_RANGE: {
			const RxRangeToken *const range = (RxRangeToken*)token->value;
			// simply check whether a character is between the two sides of the range
			CHECK_SINGLE_CHAR(range->lhs <= *chr && *chr <= range->rhs);
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_EMPTY: {
			// this should only ever be accessed directly when the entire regex is empty
			//	each token should have its own way of dealing with `RXT_EMPTY` cases
			warning("accessed `RXT_EMPTY` directly");
			CHECK_SINGLE_CHAR(true);
		}

		/* ———————————————————————————————————————————————————— */

		[[fallthrough]]; case RXT_INVALID: default: {
			error_impossible_case();
			BACKTRACK(); // unreachable
		}
	}
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
