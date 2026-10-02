/// @file parsing/tokens/escape.c

#include <stdlib.h>

#include "parsing/parse.h"
#include "errors/errors.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

token_t rx_tokenise_escape(const char **const chr) {
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

		/* ———————————————————————————————————————————————————— */

		// distinguish character classes by the character used to instantiate them
		[[fallthrough]]; // \T \N \V \F   \d \s \w   \D \S \W   \R \X \C
		case RXX_NOHORTAB: case RXX_NOLINEFD: case RXX_NOVERTAB	: case RXX_NOFORMFD:
		case RXX_DIGIT	 : case RXX_WORD	: case RXX_SPACE	:
		case RXX_NODIGIT : case RXX_NOWORD	: case RXX_NOSPACE	:
		case RXX_NEWLINE : case RXX_ANYCHAR	: case RXX_ONEBYTE	:
			RETURN_TOKEN(RXT_CLASS, **chr);

		/* ———————————————————————————————————————————————————— */

		// set up backreferences with a reference to their name
		[[fallthrough]]; // \1 -> \9
		case RXX_1: case RXX_2: case RXX_3: case RXX_4: case RXX_5: case RXX_6: case RXX_7: case RXX_8: case RXX_9: {
			groupid_t *const groupref = calloc(1, sizeof(groupid_t));
			*groupref = (groupid_t){ .type = GIDT_INT, .id = CHR_TO_INT(**chr) };

			RETURN_TOKEN(RXT_BACKREF, groupref);
		}

		/* ———————————————————————————————————————————————————— */

		// anchors / assertions
		[[fallthrough]]; // \A \G \z \Z   \b \B   \Q \E
		case RXX_STRSTART: case RXX_SEQUENCE: case RXX_STREND: case RXX_STRENDNL:
		case RXX_BOUNDARY: case RXX_NOBOUND:
		case RXX_BEGQUOTE: case RXX_ENDQUOTE:
			RETURN_TOKEN(RXT_ANCHOR, **chr);

		/* ———————————————————————————————————————————————————— */

		// map control characters (usually written as `^Y`) from their `\cY` notation, to their literal interpretations
		case RXX_CONTROL: { // \c
			(*chr)++; // increment the char pointer, so we're looking at the character after `\c`
			// make sure the control character escape is a valid one (`?`, or between `@` and `_`)
			if (!(**chr == '?' || ('@' <= **chr && **chr <= '_'))) error_invalid_escape();
			// `\c?` / `^?` (delete) is a special exception, so hardcode that character in
			//	for the rest of the escapes, they're defined sequentially, starting at `^@` for the literal `\0`
			RETURN_TOKEN(RXT_LITERAL, **chr == '?' ? '\x7f' : **chr - '@');
		}

		/* ———————————————————————————————————————————————————— */

		// the `\x` escape is a multi-character escape, so needs to be specially parsed.
		case RXX_HEXESC: { // \x
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
		}

		/* ———————————————————————————————————————————————————— */

		/// @todo implement
		[[fallthrough]]; // \K \g \k \p \P
		case RXX_RESETPOS: case RXX_NTHGROUP: case RXX_NAMEDGRP: case RXX_PROPERTY: case RXX_NOPROPERTY: {
			error_not_implemented();
		}

		/* ———————————————————————————————————————————————————— */

		// if it doesn't fit any of the special cases, then just return the character after the backslash
		default: RETURN_TOKEN(RXT_LITERAL, **chr);
	}
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
