/// @file parsing/tokens/sections.c

#include <stdlib.h>

#include "errors/errors.h"
#include "parsing/parse.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

RxTokens rx_tokenise_sections(const char **const chr, const rxobj_t rx_obj, const char end_chr, const bool is_root) {
	size_t t_alloc_count = 0;
	RxTokens tokens = { .arr = NULL, .len = 0 };

	size_t sec_alloc_count = 0;
	RxOrToken *or_token = NULL;

	// defining `is_first = is_root` is a shorthand to say that we only care about `is_first` if `is_root` is true
	bool is_first = is_root;

	while (true) {
		// this is a special exception that only applies to the very first character parsed in a regex
		//	since I've created the convention that a character is incremented before it's dereferenced,
		//	this function will fail to read the very first character of a regex string, as it'll start on the second
		//	character instead.
		// I've genuinely tried everything to solve it in a less 'hack-y' way, but this seems to be the best solution
		if (!is_first) (*chr)++;
		is_first = false;

		// now do the checks that would usually be done in the `while (...)` clause
		if (**chr == '\0' || **chr == ')') break;

		/* ———————————————————————————————————————————————————— */

		// allocate more memory for the individual tokens if needed
		//	note: on the first iteration, `tokens.arr` will be `NULL`, but `reallocf` will allocate new memory for it
		REALLOC_FOR(tokens.arr, tokens.len, t_alloc_count, token_t);

		// note: the `rx_tokenise_char` function may recurse into itself, as there may be nested groups to be parsed
		const token_t token = rx_tokenise_char(chr, rx_obj);

		/* ———————————————————————————————————————————————————— */

		switch (token.type) {
			case RXT_OR: {
				// firstly, initialise the `RXT_OR` token if it doesn't already exist
				if (or_token == NULL) or_token = calloc(1, sizeof(RxOrToken));

				// before adding anything, make sure there are actually some tokens in the array
				if (tokens.len == 0) {
					// if there aren't, then allocate space for one token
					tokens.arr = reallocf(tokens.arr, ( tokens.len = 1 ) * sizeof(token_t));
					// then assign an empty token as the sole element of the array
					tokens.arr[0] = (token_t){ .type = RXT_EMPTY };
				}

				// then, reallocate memory for the `sections` array, as needed
				REALLOC_FOR(or_token->sections, or_token->count, sec_alloc_count, RxTokens);

				// an `RXT_OR` token will consist of an array of "sections"
				//	each of these sections will be an array of generic `token_t`s
				// therefore, copy the information from the `tokens` array over into the `sections` array
				or_token->sections[or_token->count++] = tokens;

				// then reset all information about the `tokens` array, so it can start being filled again
				t_alloc_count = 0, tokens = (RxTokens){0};

				break;
			}

			/* ———————————————————————————————————————————————————— */

			case RXT_QUANT: {
				// make sure this isn't the first token in the array
				if (tokens.len == 0) error_nothing_to_repeat();

				// get the previous token in the array - this is the one that has the quantifier applied to it
				const token_t prev = tokens.arr[tokens.len - 1];

				// we can only repeat/quantify literals, backreferences, classes, groups, and sets
				if (!( prev.type == RXT_LITERAL
					|| prev.type == RXT_BACKREF
					|| prev.type == RXT_CLASS
					|| prev.type == RXT_GROUP
					|| prev.type == RXT_SET
				)) error_nothing_to_repeat();

				// get the quantifier token created by `rx_tokenise_char`
				RxQuantToken *const qtoken = (RxQuantToken*)token.value;
				// then set the `repeat` field of the quantifier to the previous token
				qtoken->repeat = prev;

				// finally, overwrite the previous token with the new quantifier token
				//	there's no need to increment the count, since the total length hasn't changed
				tokens.arr[tokens.len - 1] = token;

				break;
			}

			/* ———————————————————————————————————————————————————— */

			default: {
				// if the token isn't a special case, then simply add whichever token was found to the `tokens` array
				tokens.arr[tokens.len++] = token;
				break;
			}
		}

		/* ———————————————————————————————————————————————————— */
	}

	/* ——————————————————————————————————————————————————————————————————————————————————————————————————————————— */

	// make sure that we actually parsed the whole thing
	if (**chr != end_chr) {
		if (or_token != NULL) {
			free(or_token);
			if (or_token->sections != NULL) free(or_token->sections);
		}

		if (tokens.arr != NULL) free(tokens.arr);

		error_unterminated_group();
	}

	/* ——————————————————————————————————————————————————————————————————————————————————————————————————————————— */

	// before anything else, check if there are actually any tokens in the array
	if (tokens.len == 0) {
		// if not, make sure there's space for one token
		tokens.arr = reallocf(tokens.arr, ( tokens.len = 1 ) * sizeof(token_t));
		// then assign an empty token as the sole element of the array
		tokens.arr[0] = (token_t){ .type = RXT_EMPTY };
	}

	// if `or_token` was never initialised, then we know there was no linebar in the group
	if (or_token != NULL) {
		// if there _were_ linebars in the group, however...
		// firstly, make sure there's enough space in the sections array
		REALLOC_FOR(or_token->sections, or_token->count, sec_alloc_count, RxTokens);

		// then append whatever's left to the sections array
		//	(this is to make sure that everything after the last linebar is included)
		or_token->sections[or_token->count++] = tokens;
		tokens = (RxTokens){0}; // reset the tokens object, so its ready to be returned

		// then allocate some memory for the tokens array
		tokens.arr = calloc(1, sizeof(token_t));
		// which will be populated by a single token, that being the `RXT_OR` token
		tokens.arr[tokens.len++] = (token_t){ RXT_OR, (any_t)or_token };
	}

	return tokens;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
