/// @file parsing/parse.c

#include <stdlib.h>
#include <string.h>

#include "tokens/tokens.h"
#include "errors/errors.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline rxobj_t rx_init(const char *const string, const uint64_t flags);
static inline void	  rx_tokenise(const rxobj_t rx_obj);
static inline token_t rx_tokenise_char(const char **const chr);

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

rxobj_t rx_compile(const char *const string, const uint64_t flags) {
	const rxobj_t rx_obj = rx_init(string, flags);
	rx_tokenise(rx_obj);

	return rx_obj;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

/// Allocate `size` bytes of memory, and copy that many bytes from `src`.
#define memdup(src, size) memcpy(malloc((size)), (src), (size))

static inline rxobj_t rx_init(const char *const string, const uint64_t flags) {
	const rxobj_t rx_obj = calloc(1, sizeof(struct rx__regex));
	const size_t str_len = string == NULL ? 0 : strlen(string) + 1;

	*rx_obj = (struct rx__regex){
		.token_count = 0,
		.tokens = NULL,
		.string = string == NULL ? NULL : memdup(string, str_len),
		.flags  = flags,
	};

	return rx_obj;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline void rx_tokenise(const rxobj_t rx_obj) {
	size_t alloc_count = 0;

	for (const char *chr = rx_obj->string; *chr != '\0'; chr++) {
		// reallocate new memory as its needed
		//	on the first iteration, `rx_obj->tokens` will be `NULL`, but `reallocf` will allocate new memory for it
		if (rx_obj->token_count + 1 > alloc_count) {
			rx_obj->tokens = reallocf(rx_obj->tokens, MULT_BY_1_5(alloc_count) * sizeof(token_t));
		}

		rx_obj->tokens[rx_obj->token_count++] = rx_tokenise_char(&chr);
	}
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline token_t rx_tokenise_char(const char **const chr) {
	switch (**chr) {
		[[fallthrough]]; case '^': case '$': // ^ $
			RETURN_TOKEN(RXT_ANCHOR, **chr);

		[[fallthrough]]; case '?': case '*': case '+': case '{': // ? * + {
			return rx_tokenise_quant(chr);

		[[fallthrough]]; /* case ')': */ case ']': case '}': // ) ] }
			// none of these should ever be encountered on their own
			//	they should all be handled by their own individual functions
			error_invalid_quant();

		case '|': RETURN_TOKEN(RXT_OR	, NA);
		case '.': RETURN_TOKEN(RXT_DOT	, NA);

		case '(': return rx_tokenise_group(chr);
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
