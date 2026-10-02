/// @file parsing/parse.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens/tokens.h"
#include "errors/errors.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline rxobj_t rx_init(const char *const string, const uint64_t flags);
static inline void rx_tokenise(const rxobj_t rx_obj);

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

rxobj_t rx_compile(const char *const string, const uint64_t flags) {
	const rxobj_t rx_obj = rx_init(string, flags);
	rx_tokenise(rx_obj);

	return rx_obj;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline rxobj_t rx_init(const char *const string, const uint64_t flags) {
	const rxobj_t rx_obj = calloc(1, sizeof(struct rx__regex));

	*rx_obj = (struct rx__regex){
		.tokens = (RxTokens){ .arr = NULL, .len = 0 },
		.group_count = 0, .capture_count = 0,
		.string = strdup(string),
		.flags  = flags,
	};

	return rx_obj;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline void rx_tokenise(const rxobj_t rx_obj) {
	const char **const chr = &rx_obj->string;
	rx_obj->tokens = rx_tokenise_sections(chr, rx_obj, '\0', true);
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

token_t rx_tokenise_char(const char **const chr, const rxobj_t rx_obj) {
	switch (**chr) {
		[[fallthrough]]; case '^': case '$': // ^ $
			RETURN_TOKEN(RXT_ANCHOR, **chr);

		[[fallthrough]]; case '?': case '*': case '+': case '{': // ? * + {
			return rx_tokenise_quant(chr);

		[[fallthrough]]; case ')': case ']': case '}': // ) ] }
			// none of these should ever be encountered on their own
			//	they should all be handled by their own individual functions
			error_general();

		case '.': RETURN_TOKEN(RXT_CLASS, '.');

		case '|': RETURN_TOKEN(RXT_OR, NA);

		case '(': return rx_tokenise_group(chr, rx_obj);
		case '[': return rx_tokenise_set(chr);

		default	: return rx_tokenise_literal(chr);
	}
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

token_t rx_tokenise_literal(const char **const chr) {
	return (**chr == '\\') ? rx_tokenise_escape(chr) : (token_t){ RXT_LITERAL, **chr };
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

// spell:ignoreRegExp /(?:\b|_)\w?apl\w\b/gi
