/// @file match/class.c

#include "match.h"
#include "types/types.h"
#include "errors/errors.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

bool rx_match_class(const char class, const char chr) {
	switch (class) {
		case RXL_DOT		: return true; // return chr != '\n';

		case RXL_DIGIT		: return IS_DIGIT(chr);
		case RXL_SPACE		: return IS_SPACE(chr);
		case RXL_WORD		: return IS_WORDC(chr);

		case RXL_NODIGIT	: return !IS_DIGIT(chr);
		case RXL_NOSPACE	: return !IS_SPACE(chr);
		case RXL_NOWORD		: return !IS_WORDC(chr);

		case RXL_NOHORTAB	: return chr != '\t';
		case RXL_NOLINEFD	: return chr != '\n';
		case RXL_NOVERTAB	: return chr != '\v';
		case RXL_NOFORMFD	: return chr != '\f';

		/// @todo this isn't exactly correct, so I'll need to fix it when I implement unicode functionality
		case RXL_NEWLINE	: return chr == '\n';

		default: error_impossible_case();
	}

	return false; // unreachable
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
