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

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define TOKEN_MATCH_ARGS const char *chr, matches_t *const matches, const int depth
#define TOKEN_MATCH_PARAMS chr, matches, depth + 1

static inline ssize_t match_token(const token_t *const token, const ssize_t count, TOKEN_MATCH_ARGS);

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

	const char *chr = string;

	// iterate through the test string, trying to find a match starting from each character
	while (*chr != '\0') {
		const ssize_t match_len = match_token(SPREAD_TOKS(rx_obj->tokens), chr, &matches, 0);
		dmatch_len(chr, match_len);

		// if we didn't find a match, then move on, and start trying to find a match starting from the next character
		if (!MATCHED(match_len)) { chr++; continue; }

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
		//	but also make sure that the char pointer is always moved forward by at least one every time
		//	 this prevents an infinite number of zero-width matches
		chr += MAX(1, match_len);
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

#define MATCH_NEXT_TOKEN(inc_chr) match_token(token + 1, count - 1, (chr + (intptr_t)(inc_chr)), matches, depth + 1)

#define RETURN_SINGLE_CHAR(test_case) do {																\
	/* if it doesn't match, then there's nothing more to do */											\
	/*	therefore we've failed this branch, and we now have to backtrack (de-recurse) */				\
	if (*chr == '\0' || !(test_case)) BACKTRACK();														\
	const ssize_t match_len = 1; /* we matched, and by definition, the length of a single char is 1 */	\
	/*	(yes, ik that defining a constant to only use twice is verbose, but this is clearer to me) */	\
	\
	/* now that we've matched this token, check that all the tokens after it also matches */			\
	const ssize_t tail_len = MATCH_NEXT_TOKEN(match_len);												\
	\
	/* if it does, then return (our length + its length) */												\
	if (MATCHED(tail_len)) RETURN(match_len + tail_len);												\
	else BACKTRACK(); /* and if not, then just return failure as usual */								\
} while (0)

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline ssize_t match_token(const token_t *const token, const ssize_t count, TOKEN_MATCH_ARGS) {
	if (count ==  0) RETURN( 0);
	if (count == -1) RETURN(-1);

	/* ———————————————————————————————————————————————————— */

	switch (token->type) {

		case RXT_LITERAL: RETURN_SINGLE_CHAR(*chr == (char)token->value);
		case RXT_CLASS	: RETURN_SINGLE_CHAR(rx_match_class(token->value, *chr));

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
			const RxQuantToken *const quant = (RxQuantToken*)token->value;
			size_t alloc_count = quant->lhs + 1, rep_count = 0;

			size_t *lengths = calloc(alloc_count, sizeof(size_t));
			const char *pchar = chr;

			// greedily match up to `quant->rhs` repetitions and record lengths
			while (rep_count < quant->rhs) {
				// try to match a single repetition
				const ssize_t match_len = match_token(&quant->repeat, 1, pchar, matches, depth + 1);
				if (!MATCHED(match_len)) break;

				REALLOC_FOR(lengths, rep_count + 1, alloc_count, char*);
				lengths[++rep_count] = (size_t)(( pchar += match_len ) - chr);
			}

			// check if we're still within the bounds of the minimum repetition count (the lhs)
			//	if we are, then we haven't done enough iterations - return failure
			if (rep_count < quant->lhs) { free(lengths); BACKTRACK(); }

			// try matching the remaining tokens from max count down to min count
			for (ssize_t i = rep_count; i >= (ssize_t)quant->lhs; i--) {
				const ssize_t tail_len = MATCH_NEXT_TOKEN(lengths[i]);

				if (MATCHED(tail_len)) {
					const ssize_t total_len = lengths[i] + tail_len;

					free(lengths);
					RETURN(total_len);
				}
			}

			free(lengths);
			BACKTRACK();
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_OR: {
			const RxOrToken *const or_sects = (RxOrToken*)token->value;

			// iterate through each of the sections in the 'or' object
			for (size_t i = 0; i < or_sects->count; i++) {
				const RxTokens *const section = &or_sects->sections[i];

				// match the section against its own tokens first
				const ssize_t match_len = match_token(SPREAD_TOKS(*section), TOKEN_MATCH_PARAMS);
				if (!MATCHED(match_len)) continue;

				// then check if the tokens after this, all match
				const ssize_t tail_len = MATCH_NEXT_TOKEN(match_len);

				// if they do, return success
				if (MATCHED(tail_len)) RETURN(match_len + tail_len);
				// if not, keep checking the rest of the sections
			}

			BACKTRACK(); // none of the 'or' sections matched
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_GROUP: {
			const RxGroupToken *const group = (RxGroupToken*)token->value;

			char **const capt_str = &matches->captures[group->id.idx];
			// save the current capture, in case this match fails
			char *const saved_capt = *capt_str;

			// try and match the group's contents, and get its length, as normal
			const ssize_t match_len = match_token(SPREAD_TOKS(group->tokens), TOKEN_MATCH_PARAMS);
			if (!MATCHED(match_len)) BACKTRACK();

			// provisionally copy the match into the `captures` array, assigning it to the index of this group
			//	also, allocate one more byte than the match's length, so `calloc` can include a nullbyte at the end
			*capt_str = (char*) memcpy(calloc(1, match_len + 1), chr, match_len);

			// try to match all the following tokens
			const ssize_t tail_len = MATCH_NEXT_TOKEN(match_len);

			if (!MATCHED(tail_len)) {
				// free the failed provisional allocation, restore the saved capture, and backtrack
				free(*capt_str);
				*capt_str = saved_capt;
	
				BACKTRACK();
			}

			RETURN(match_len + tail_len);
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_SET: {
			const RxSetToken *const set = (RxSetToken*)token->value;
			bool did_match = false;

			// iterate through all of the set's tokens, checking each one for a match
			for (size_t i = 0; i < set->tokens.len; i++) {
				const token_t *const set_token = &set->tokens.arr[i];

				// first, try and match `chr` against the current set token
				const ssize_t match_len = match_token(set_token, 1, TOKEN_MATCH_PARAMS);

				// if we found a match, mark the character as being in the set, and break
				if (MATCHED(match_len)) { did_match = true; break; }
			}

			// if we couldn't find a match in inverse mode, it's a success, and in normal mode, a failure
			//	so, in inverse mode, flip the result of the for loop
			if (set->is_inverse) did_match = !did_match;
			RETURN_SINGLE_CHAR(did_match);
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_RANGE: {
			const RxRangeToken *const range = (RxRangeToken*)token->value;
			// simply check whether a character is between the two sides of the range
			RETURN_SINGLE_CHAR(range->lhs <= *chr && *chr <= range->rhs);
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_EMPTY: {
			// increment the token for the next match, but not the character pointer
			const ssize_t tail_len = MATCH_NEXT_TOKEN(0);

			if (MATCHED(tail_len)) RETURN(tail_len);
			else BACKTRACK();
		}

		/* ———————————————————————————————————————————————————— */

		[[fallthrough]]; case RXT_INVALID: default: {
			error_impossible_case();
			BACKTRACK(); // unreachable
		}
	}
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
