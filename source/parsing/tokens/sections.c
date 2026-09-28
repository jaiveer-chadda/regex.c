/// @file parsing/tokens/sections.c

#include <stdio.h>
#include <stdlib.h>

#include "errors/errors.h"
#include "parsing/parse.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

RxTokens rx_tokenise_sections(const char **const chr, const rxobj_t rx_obj, const char end_chr) {
	size_t t_alloc_count = 0;
	RxTokens tokens = { .arr = NULL, .len = 0 };

	size_t sec_alloc_count = 0;
	RxOrToken *or_token = NULL;

	static bool adjusted = false;

	while (*(++(*chr)) != '\0' && **chr != ')') {

		// this is a special exception that only applies to the very first character parsed in a regex
		//	since I've created the convention that a character is incremented before it's dereferenced,
		//	this function will fail to read the very first character of a regex string, as it'll start on the second
		//	character instead.
		// I've genuinely tried everything to solve it in a less 'hack-y' way, but this seems to be the best solution
		if (!adjusted && *chr == rx_obj->string) (*chr)--;
		adjusted = true;

		/* ———————————————————————————————————————————————————— */

		// allocate more memory for the individual tokens if needed
		//	note: on the first iteration, `tokens.arr` will be `NULL`, but `reallocf` will allocate new memory for it
		REALLOC_FOR(tokens.arr, tokens.len, t_alloc_count, token_t);

		// get the actual tokens from 
		// note: the `rx_tokenise_char` function may recurse into itself, as there may be nested groups to be parsed
		const token_t token = rx_tokenise_char(chr, rx_obj);

		// if the token isn't a `|` token, then simply add whichever token was found to the `tokens` array
		if (token.type != RXT_OR) {
			tokens.arr[tokens.len++] = token;
			continue;
		}

		/* ———————————————————————————————————————————————————— */
		// in the case of an `RXT_OR` token, though...

		// firstly, initialise the `RXT_OR` token if it doesn't already exist
		if (or_token == NULL) or_token = calloc(1, sizeof(RxOrToken));

		// then, reallocate memory for the `sections` array, as needed
		REALLOC_FOR(or_token->sections, or_token->count, sec_alloc_count, RxTokens);

		// an `RXT_OR` token will consist of an array of "sections"
		//	each of these sections will be an array of generic `token_t`s
		// therefore, copy the information from the `tokens` array over into the `sections` array
		or_token->sections[or_token->count++] = tokens;

		// then reset all information about the `tokens` array, so it can start being filled again
		t_alloc_count = 0,
		tokens = (RxTokens){0};
	}

	/* ——————————————————————————————————————————————————————————————————————————————————————————————————————————— */

	// make sure that we actually parsed the whole thing
	if (**chr != end_chr) {
		if (or_token->sections != NULL) free(or_token->sections);
		if (tokens.arr  != NULL) free(tokens.arr);

		error_unterminated_group();
	}

	/* ——————————————————————————————————————————————————————————————————————————————————————————————————————————— */

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
