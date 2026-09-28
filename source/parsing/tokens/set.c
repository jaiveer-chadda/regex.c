/// @file parsing/tokens/set.c

#include <stdlib.h>

#include "tokens.h"
#include "parsing/parse.h"
#include "errors/errors.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

token_t rx_tokenise_set(const char **const chr) {
	// note: `calloc` will initialise `*tokens` to `NULL`, `token_count` to `0`, and `is_inverse` to `false`
	RxSetToken *const set_tk = calloc(1, sizeof(RxSetToken));

	// if the first character in a set is `^`, then mark the set as an inverse set
	if (*(*chr + 1) == '^') {
		(*chr)++; // discard the `^`
		set_tk->is_inverse = true;
	}

	/* ———————————————————————————————————————————————————— */

	size_t alloc_count = 0;

	// iterate through the regex until we find a closing bracket
	while (*(++(*chr)) != ']') {
		// if we reach the end of the string, then we know something's gone wrong
		if (**chr == '\0') error_unterminated_set();

		// reallocate new memory as its needed
		REALLOC_FOR(set_tk->tokens.arr, set_tk->tokens.len, alloc_count, token_t);

		/* ———————————————————————————————————————————————————— */

		// create a token for the parsed char, which will either be a literal (e.g. `A`) or a class (e.g. `\d`)
		token_t token;

		// if the character is a hyphen, and its not the first or last character
		if (**chr == '-' && !(set_tk->tokens.len == 0 || *(*chr + 1) == ']')) {
			// get the previous token that we added, and decrement the token count so that it can be overwritten
			const token_t chr1 = set_tk->tokens.arr[--(set_tk->tokens.len)];
			(*chr)++; // point `chr` at the second character
			const token_t chr2 = rx_tokenise_literal(chr);

			// only accept the chars if they're literals
			//	also, don't allow ranges where the second char is smaller than the first
			if (chr1.type != RXT_LITERAL || chr2.type != RXT_LITERAL || chr1.value > chr2.value) {
				free(set_tk->tokens.arr); free(set_tk);
				error_invalid_range();
			}

			// allocate a range object, and assign it to `token`
			RxRangeToken *const range = malloc(sizeof(RxRangeToken));
			*range = (RxRangeToken){ .lhs = chr1.value, .rhs = chr2.value };

			// this token will overwrite the old token of `chr1`
			token = (token_t){ RXT_RANGE, (any_t)range };

		/* ———————————————————————————————————————————————————— */

		// otherwise, the character should be interpreted as a literal by `rx_tokenise_literal`
		//	however, only accept the result of that function if its a literal or class
		} else {
			token = rx_tokenise_literal(chr);

			if (token.type != RXT_LITERAL && token.type != RXT_CLASS) {
				free(set_tk->tokens.arr); free(set_tk);
				error_invalid_escape();
			}
		}

		/* ———————————————————————————————————————————————————— */

		set_tk->tokens.arr[set_tk->tokens.len++] = token;
	}

	/* ———————————————————————————————————————————————————— */

	// return the whole set, which now contains an array of its individual literals/classes
	RETURN_TOKEN(RXT_SET, set_tk);
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
