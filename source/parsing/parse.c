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
		[[fallthrough]]; case '^': case '$': // ^ $
			RETURN_TOKEN(RXT_ANCHOR, **chr);

		[[fallthrough]]; case '?': case '*': case '+': case '{': // ? * + {
			return rx_tokenise_quant(chr);

		case '\\':
			return rx_tokenise_escape(chr);

		case '|': RETURN_TOKEN(RXT_OR	, NA);
		case '.': RETURN_TOKEN(RXT_DOT	, NA);

		case '(': RETURN_TOKEN(RXT_GROUP, NA);
		case '[': RETURN_TOKEN(RXT_SET	, NA);

		default	: RETURN_TOKEN(RXT_LITERAL, **chr);
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
	RETURN_TOKEN(RXT_QUANT, (any_t)token);
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline token_t rx_tokenise_escape(const char **const chr) {
	// increment the char pointer before dereferencing it
	switch (*(++(*chr))) {
		// translate all non-trivial literal escapes into their literal equivalents
		case RXX_NULL	 : RETURN_TOKEN(RXT_LITERAL, '\0');
		case RXX_ALERT	 : RETURN_TOKEN(RXT_LITERAL, '\a');
		case RXX_HORTAB	 : RETURN_TOKEN(RXT_LITERAL, '\t');
		case RXX_LINEFD	 : RETURN_TOKEN(RXT_LITERAL, '\n');
		case RXX_VERTAB	 : RETURN_TOKEN(RXT_LITERAL, '\v');
		case RXX_FORMFD	 : RETURN_TOKEN(RXT_LITERAL, '\f');
		case RXX_RETURN	 : RETURN_TOKEN(RXT_LITERAL, '\r');
		case RXX_ESCAPE	 : RETURN_TOKEN(RXT_LITERAL,'\33');

		// distinguish character classes by the character used to instantiate them
		[[fallthrough]]; // \T \N \V \F   \d \s \w   \D \S \W   \R \X \C
		case RXX_NOHORTAB: case RXX_NOLINEFD: case RXX_NOVERTAB	: case RXX_NOFORMFD:
		case RXX_DIGIT	 : case RXX_WORD	: case RXX_SPACE	:
		case RXX_NODIGIT : case RXX_NOWORD	: case RXX_NOSPACE	:
		case RXX_NEWLINE : case RXX_ANYCHAR	: case RXX_ONEBYTE	:
			RETURN_TOKEN(RXT_CLASS, **chr);

		// set up backreferences with a reference to their name
		//	this value is temporary tho - I'll make a proper backreference object later
		[[fallthrough]]; // \1 -> \9
		case RXX_1: case RXX_2: case RXX_3: case RXX_4: case RXX_5: case RXX_6: case RXX_7: case RXX_8: case RXX_9:
			RETURN_TOKEN(RXT_BACKREF, CHR_TO_INT(**chr));

		// anchors / assertions
		[[fallthrough]]; // \A \G \z \Z   \b \B   \Q \E
		case RXX_STRSTART: case RXX_SEQUENCE: case RXX_STREND: case RXX_STRENDNL:
		case RXX_BOUNDARY: case RXX_NOBOUND:
		case RXX_BEGQUOTE: case RXX_ENDQUOTE:
			RETURN_TOKEN(RXT_ANCHOR, **chr);

		// map control characters (usually written as `^Y`) from their `\cY` notation, to their literal interpretations
		case RXX_CONTROL: // \c
			(*chr)++; // increment the char pointer, so we're looking at the character after `\c`
			// make sure the control character escape is a valid one (`?`, or between `@` and `_`)
			if (!(**chr == '?' || ('@' <= **chr && **chr <= '_'))) error_invalid_escape();
			// `\c?` / `^?` (delete) is a special exception, so hardcode that character in
			//	for the rest of the escapes, they're defined sequentially, starting at `^@` for the literal `\0`
			RETURN_TOKEN(RXT_LITERAL, **chr == '?' ? '\x7f' : **chr - '@');

		// the `\x` escape is a multi-character escape, so needs to be specially parsed.
		case RXX_HEXESC: // \x
			wchar_t hex_buf = 0;
			uint8_t num_iter = 0;

			const bool is_long = *(*chr + 1) == '{';
			const uint8_t max_iter = is_long ? 4 : 2;

			if (is_long) (*chr)++; // discard the opening brace

			while (num_iter++ < max_iter) {
				(*chr)++;
				if		('0' <= **chr && **chr <= '9') { hex_buf = (hex_buf * 16) + ((**chr - '0')		); }
				else if ('A' <= **chr && **chr <= 'F') { hex_buf = (hex_buf * 16) + ((**chr - 'A') + 10	); }
				else if ('a' <= **chr && **chr <= 'f') { hex_buf = (hex_buf * 16) + ((**chr - 'a') + 10	); }
				else break; // not a hex digit
			}

			// if the escape was just `\x`, without anything after it, then throw an error
			if (num_iter == 0) error_invalid_escape();
			if (is_long) {
				// long escapes must end with a closing brace
				if (**chr != '}') error_invalid_escape();
				// if they do end with a brace, discard it
				(*chr)++;
			}

			// return the character, whose integer value we just calculated, as a literal
			RETURN_TOKEN(RXT_LITERAL, hex_buf);

		/// @todo implement
		[[fallthrough]]; // \K \g \k \p \P
		case RXX_RESETPOS: case RXX_NTHGROUP: case RXX_NAMEDGRP: case RXX_PROPERTY: case RXX_NOPROPERTY:
			error_not_implemented();

		// if it doesn't fit any of the special cases, then just return the character after the backslash
		default: RETURN_TOKEN(RXT_LITERAL, **chr);
	}
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
