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

#define TOKEN_MATCH_ARGS const size_t idx, matches_t *const matches, const int depth
#define TOKEN_MATCH_PARAMS idx, matches, depth + 1

static inline ssize_t match_token(const token_t *const token, const ssize_t count, TOKEN_MATCH_ARGS);

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

matches_t rx_match(const rxobj_t rx_obj, const char *const str) {
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
		.string = str, .str_len = strlen(str),
	};

	/* ———————————————————————————————————————————————————— */

	// iterate through the test string, trying to find a match starting from each character
	for (size_t idx = 0; idx < matches.str_len; idx++) {
		const ssize_t match_len = match_token(SPREAD_TOKS(rx_obj->tokens), idx, &matches, 0);
		dmatch_len(idx, match_len);

		// if we didn't find a match, then move on, and start trying to find a match starting from the next character
		if (!MATCHED(match_len)) continue;

		/* ———————————————————————————————————————————————————— */
		// -- successfully matched --

		// allocate memory for `matches.arr` as needed
		REALLOC_FOR(matches.arr, matches.len, alloc_count, match_t);
		// add match to `matches` array
		matches.arr[matches.len++] = (match_t){ .idx = idx, .len = match_len };

		/* ———————————————————————————————————————————————————— */

		// the match should never have gone past the end of the string
		assert(idx + match_len <= matches.str_len);

		// increment the index by the match len, so we don't get overlapping matches
		//	but also make sure that `idx` isn't ever decremented - this prevents an infinite number of 0-width matches
		//	-1 at the end, since `idx` will be incremented by 1 anyways, cos of the `for ... idx++`
		idx += MAX(1, match_len) - 1;
	}

	/* ———————————————————————————————————————————————————— */

	return matches;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#ifdef DEBUG_MODE
#	define ENTER_MATCH() dmatch_enter(matches, token, idx, depth)
#	define RETURN(val) do { if (count >= 1) dmatch_return(token, idx, (val), depth + 1); return (val); } while (0)
#else
#	define ENTER_MATCH()
#	define RETURN(val) return (val)
#endif

#define BACKTRACK() RETURN(-1)

/* ———————————————————————————————————————————————————— */

#define MATCH_NEXT_TOKEN(inc_chr) match_token(token + 1, count - 1, idx + (inc_chr), matches, depth)

#define RETURN_N_CHARS(len_if_match, test_case) do {										\
	/* if it doesn't match, then there's nothing more to do */								\
	/*	therefore we've failed this branch, and we now have to backtrack (de-recurse) */	\
	if (idx >= matches->str_len || !(test_case)) BACKTRACK();								\
	/* now that we've matched this token, check that all the tokens after it also match */	\
	const ssize_t tail_len = MATCH_NEXT_TOKEN((len_if_match));								\
	/* if it does, then return (our length + its length) */									\
	if (MATCHED(tail_len)) RETURN((len_if_match) + tail_len);								\
	else BACKTRACK(); /* and if not, then just return failure as usual */					\
} while (0)

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline ssize_t match_token(const token_t *const token, const ssize_t count, TOKEN_MATCH_ARGS) {
	const char *const str = matches->string;

	if (count ==  0) RETURN( 0);
	if (count == -1) RETURN(-1);

	ENTER_MATCH();

	/* ———————————————————————————————————————————————————— */

	switch (token->type) {

		case RXT_LITERAL: RETURN_N_CHARS(1, str[idx] == (char)token->value);
		case RXT_CLASS	: RETURN_N_CHARS(1, rx_match_class(token->value, str[idx]));
		case RXT_ANCHOR	: RETURN_N_CHARS(0, rx_match_anchor(token->value, idx, matches));

		/* ———————————————————————————————————————————————————— */

		case RXT_BACKREF: {
			const groupid_t *const backref = (groupid_t*)token->value;
			const size_t grp_idx = backref->idx;

			if (grp_idx > matches->len) error_invalid_backref();

			// get the string that was captured by the referenced group
			const char *const captured = matches->captures[grp_idx];
			size_t match_idx = 0;

			// iterate through the test string, starting from the current `idx`
			// and through the captured string, starting from 0
			//	stop when we go past the end of `str`, or when we reach the end of the matched string
			while ((idx + match_idx < matches->str_len) && (str[idx + match_idx] != '\0')) {
				match_idx++; // increment the iterator index
				// if the captured string doesn't match the test string at this index, then return failure
				if (str[idx + match_idx] != captured[match_idx]) BACKTRACK();
				// otherwise, keep iterating until we reach the end of one of the strings
			}

			// now check that everything from here on matches
			const ssize_t tail_len = MATCH_NEXT_TOKEN(match_idx);

			// if it doesn't, return failure
			if (!MATCHED(tail_len)) BACKTRACK();
			// and if it does, return the total length of the match and the tail
			RETURN(match_idx + tail_len);
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_QUANT: {
			const RxQuantToken *const quant = (RxQuantToken*)token->value;
			// initialise `alloc_count` to `quant->lhs`, since in an ideal world, that'll be the minimum number of
			//	iterations that we do
			size_t alloc_count = quant->lhs + 1, rep_count = 0;

			// create an array to hold the lengths of each match, so that we can backtrack
			size_t *lengths = calloc(alloc_count, sizeof(size_t));
			size_t q_idx = idx;

			// greedily match up to `quant->rhs` repetitions and record lengths
			while (rep_count < quant->rhs) {
				// try to match a single repetition
				const ssize_t match_len = match_token(&quant->repeat, 1, q_idx, matches, depth + 1);
				// as soon as a repetition doesn't match, break the loop, and check whether we made it far enough
				if (!MATCHED(match_len)) break;

				// if this repetition _did_ match, however, then allocate space for another length in the `lengths` arr
				REALLOC_FOR(lengths, rep_count + 1, alloc_count, char*);
				// then increment the index by the length of the match, add it to the array, and start looking again
				lengths[++rep_count] = (size_t)(( q_idx += match_len ) - idx);
			}

			// check if we're still within the bounds of the minimum repetition count (the lhs)
			//	if we are, then we haven't done enough iterations - return failure
			if (rep_count < quant->lhs) { free(lengths); BACKTRACK(); }

			// try matching the remaining tokens from max count down to min count
			//	they're being backtracked to in reverse order, cos we're being greedy, so we try and match the largest
			//	count that we can, before trying to match anything smaller
			for (ssize_t i = rep_count; i >= (ssize_t)quant->lhs; i--) {
				// matching against `lengths[i]` means matching `lengths[i]` characters after the `idx` that was
				//	passed to this function
				const ssize_t tail_len = MATCH_NEXT_TOKEN(lengths[i]);

				if (MATCHED(tail_len)) {
					// save the return length here before the `lengths` array is freed
					const ssize_t total_len = lengths[i] + tail_len;

					free(lengths);
					RETURN(total_len);
				}
			}

			// if none of the iterations managed to work, then we really do have to backtrack to before the quantifier
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
			*capt_str = (char*) memcpy(calloc(1, match_len + 1), &str[idx], match_len);

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

				// first, try and match the current char (`str[idx]`) against the current set token
				const ssize_t match_len = match_token(set_token, 1, TOKEN_MATCH_PARAMS);

				// if we found a match, mark the character as being in the set, and break
				if (MATCHED(match_len)) { did_match = true; break; }
			}

			// if we couldn't find a match in inverse mode, it's a success, and in normal mode, a failure
			//	so, in inverse mode, flip the result of the for loop
			if (set->is_inverse) did_match = !did_match;
			RETURN_N_CHARS(1, did_match);
		}

		/* ———————————————————————————————————————————————————— */

		case RXT_RANGE: {
			const RxRangeToken *const range = (RxRangeToken*)token->value;
			// simply check whether a character is between the two sides of the range
			RETURN_N_CHARS(1, range->lhs <= str[idx] && str[idx] <= range->rhs);
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
