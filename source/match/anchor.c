/// @file match/anchor.c

#include "types/types.h"
#include "match/match.h"
#include "errors/errors.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

/// @todo implement multiline flag
#define FLAG_ACTIVE(flag) false // temp

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

ssize_t rx_match_anchor(const char anchor, const size_t idx, const matches_t *const matches) {
	const char *const str = matches->string;
	const size_t str_len = matches->str_len;

	switch (anchor) {

		/* ———————————————————————————————————————————————————— */

		case '^': {
			// if we're in multiline mode, then first check whether the previous char was a newline
			if (FLAG_ACTIVE(MULTILINE) && str[idx-1] == '\n') return true;
			// if the prev char wasn't a newline, or we're not in multiline mode, then fallthrough to
			//	checking if the char is at the start of the string
			else; [[fallthrough]];
		}

		// this case handles (and behaves identically for):
		//	- the `\A`case
		//	- the `^` case, when not in multiline mode
		//	- the `^` case, if the previous character wasn't a newline
		case RXX_STRSTART: return (idx == 0); // \A

		/* ———————————————————————————————————————————————————— */

		case '$': {
			if (FLAG_ACTIVE(MULTILINE) && str[idx+1] == '\n') return true;
			else; [[fallthrough]];
		}

		case RXX_STREND: return (idx == str_len); // \z

		/* ———————————————————————————————————————————————————— */

		case RXX_SEQUENCE: { // \G
			error_not_implemented();
		}

		case RXX_STRENDNL: { // \Z
			// check either that we're at the end of the string,
			//	or that there's a single newline before the end
			return (
				(idx == str_len) ||
				(idx == str_len - 1 && str[idx] == '\n')
			);
		}

		/* ———————————————————————————————————————————————————— */

		[[fallthrough]]; case RXX_BOUNDARY: case RXX_NOBOUND: {
			const bool is_prev_wordc = (idx != 0	  ) && IS_WORDC(str[idx-1]);
			const bool is_curr_wordc = (idx != str_len) && IS_WORDC(str[idx	 ]);

			const bool is_boundary = (is_prev_wordc != is_curr_wordc);
			const bool needs_bound = (anchor == RXX_BOUNDARY);

			return (is_boundary == needs_bound);
		}

		/* ———————————————————————————————————————————————————— */

		default: error_impossible_case();

		/* ———————————————————————————————————————————————————— */
	}

	return false;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
