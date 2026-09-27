/// @file parsing/tokens/quantifier.c

#include <stdlib.h>

#include "parsing/parse.h"
#include "errors/errors.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

/// @returns true if the character `chr` represents a digit, and false otherwise.
static inline bool chr_is_dig(const char chr) { return '0' <= (chr) && (chr) <= '9'; }

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

token_t rx_tokenise_quant(const char **const chr) {
	int size[2] = {0};

	// translate each of the quantifier types into their `{n,m}` equivalents
	switch (**chr) {
		case '?': size[0] = 0, size[1] = 1	; break; // {0,1}
		case '*': size[0] = 0, size[1] = INF; break; // {0,∞}
		case '+': size[0] = 1, size[1] = INF; break; // {1,∞}

		/* ———————————————————————————————————————————————————— */

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
			break;

		default:
			error_impossible_case();
	}

	/* ———————————————————————————————————————————————————— */

	RxQuantToken *const token = calloc(1, sizeof(RxQuantToken));
	*token = (RxQuantToken){ .lhs = size[0], .rhs = size[1] };

	// finally, check if the character after a quantifier is a question mark (i.e. the quantifier is non-greedy)
	if (*(*chr + 1)	== '?') {
		token->has_qm = true;
		// if it exists, increment the char pointer past the end of the question mark
		(*chr)++;
	}

	// store the pointer to the quant token as an `any_t` - it'll be converted back to a `RxQuantToken*` later
	RETURN_TOKEN(RXT_QUANT, token);
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
