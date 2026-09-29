/// @file match/class.c

#include "match.h"
#include "types/types.h"
#include "errors/errors.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define IN_RANGE(c, min, max) ((min) <= (c) && (c) <= (max))

#define IS_LOWER(c) IN_RANGE(c, 'a', 'z')
#define IS_UPPER(c) IN_RANGE(c, 'A', 'Z')
#define IS_DIGIT(c) IN_RANGE(c, '0', '9')

#define IS_WORDC(c) (IS_LOWER(c) || IS_UPPER(c) || IS_DIGIT(c) || c == '_')
#define IS_SPACE(c) ((c) == ' ' || (c) == '\n' || (c) == '\t' || (c) == '\v' || (c) == '\f' || (c) == '\r')

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
