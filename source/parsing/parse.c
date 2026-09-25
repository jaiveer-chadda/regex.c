/// @file parsing/parse.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "defs.h"
#include "errors/errors.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

rxobj_t rx_compile(const char *const string, const uint64_t flags) {
	const rxobj_t rx_obj = rx_init(string, flags);
	rx_tokenise(rx_obj);

	return rx_obj;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

/// Allocate `size` bytes of memory, and copy that many bytes from `src`.
#define memdup(src, size) memcpy(malloc((size)), (src), (size))

static inline rxobj_t rx_init(const char *const string, const uint64_t flags) {
	const rxobj_t rx_obj = calloc(1, sizeof(struct rx__regex));
	const size_t str_len = strlen(string) + 1;

	*rx_obj = (struct rx__regex){
		.token_count = 0,
		.tokens = NULL,
		.string = memdup(string, str_len),
		.flags  = flags,
	};

	return rx_obj;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

/// Approximately multiply a number by 1.5, in place.
#define MULT_BY_1_5(var) ((var) += (var) <= 1 ? 1 : (var) >> 1)

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

#define RETURN_TOKEN(...) return (token_t){ __VA_ARGS__ }

static inline token_t rx_tokenise_char(const char **const chr) {
	switch (**chr) {
		[[fallthrough]]; case '?': case '*': case '+': case '{':
			return rx_tokenise_quant(chr);

		case '|': RETURN_TOKEN(RXT_OR	  , NA			);
		case '.': RETURN_TOKEN(RXT_DOT	  , NA			);
		case '^': RETURN_TOKEN(RXT_ASSERT , **chr		);
		case '$': RETURN_TOKEN(RXT_ASSERT , **chr		);
		case'\\': RETURN_TOKEN(RXT_ESCAPE , *(++(*chr))	);

		case '(': RETURN_TOKEN(RXT_GROUP  , NA			);
		case '[': RETURN_TOKEN(RXT_SET	  , NA			);

		default	: RETURN_TOKEN(RXT_LITERAL, **chr		);
	}
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

/// @returns true if the character `chr` represents a digit, and false otherwise.
static inline bool chr_is_dig(const char chr) { return '0' <= (chr) && (chr) <= '9'; }

#define CHR_TO_INT(char_) ((char_) - '0')

static inline token_t rx_tokenise_quant(const char **const chr) {
	int size[2] = {0};

	// translate each of the quantifier types into their `{n,m}` equivalents
	switch (**chr) {
		case '?': size[0] = 0, size[1] = 1	; break; // {0,1}
		case '*': size[0] = 0, size[1] = INF; break; // {0,∞}
		case '+': size[0] = 1, size[1] = INF; break; // {1,∞}

		case '{':
			// iterate through each char, adding its value to the total until we reach a non-digit
			while (chr_is_dig(*(++(*chr)))) size[0] = (size[0] * 10) + CHR_TO_INT(**chr);

			// if the char we ended up with is a closing brace, then we know that the quant was in the form `{n}`
			// if the char isn't a closing brace, and is anything other than a comma, then the quant is invalid
			// if the char _is_ a comma, and the next char after it is a `}`, then the quant was in the form `{n,}`
			if ( **chr		== '}') { size[1] = size[0]	; (*chr)++; break;	} // {2}  -> {2,2}
			if ( **chr		!= ',') { error_invalid_quant();				} // {ab} -> error
			if (*(*chr + 1)	== '}') { size[1] = INF		; (*chr)++; break;	} // {2,} -> {2,∞}

			// repeat the iteration again, this time for the second integer
			while (chr_is_dig(*(++(*chr)))) size[1] = (size[1] * 10) + CHR_TO_INT(**chr);

			// if the char isn't a closing brace, or if `m` is smaller than `n` (given `{n,m}`), the quant is invalid
			if (**chr != '}' || size[0] > size[1]) error_invalid_quant();

			(*chr)++; // finally, increment the char pointer so its pointing at the char after the closing brace
			break;

		default:
			error_impossible_case();
	}

	RxQuantToken *const token = calloc(1, sizeof(RxQuantToken));
	*token = (RxQuantToken){ .lhs = size[0], .rhs = size[1] };

	// finally, check if the character after a quantifier is a question mark (i.e. the quantifier is non-greedy)
	if (*(*chr + 1)	== '?') {
		token->has_qm = true;
		// if it exists, increment the char pointer past the end of the question mark
		(*chr)++;
	}

	// store the pointer to the quant token as an `any_t` - it'll be converted back to a `RxQuantToken*` later
	return (token_t){ RXT_QUANT, (any_t)token };
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
