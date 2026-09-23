/// @file types/_flags.h

#ifndef FLAGS_H_
#define FLAGS_H_

enum RxFlag {

	/* —— General ——————————————————————————————————————————— */

	RXF_GLOBAL		= 'g', /** Global				– Find all matches instead of just the first. */
	RXF_EXTENDED	= 'x', /** Extended				– Spaces are ignored, and EoL comments are enabled. */

	/* —— Regex Parsing ————————————————————————————————————— */

	RXF_WORDONLY	= 'w', /** Words Only			– Only match entire words. */
	RXF_FIXEDSTR	= 'F', /** Fixed Strings		– Treat the whole pattern as a literal. */
	RXF_UNGREEDY	= 'U', /** Ungreedy	Quantifiers	– Quantifiers are lazy by default. `?` makes them greedy. */
	RXF_MULTILINE	= 'm', /** Multiline Matching	– Match `^` and `$` at line boundaries. */
	RXF_DOLLAREND	= 'D', /** Dollar Marks End		– Match `$` only at the absolute end of the string. */
	RXF_SINGLELINE	= 's', /** Single Line			– The dot `.` widcard matches newlines `\n`. */

	/* —— Input Parsing ————————————————————————————————————— */

	RXF_UNICODE		= 'u', /** Unicode				– Treat multibyte characters as a single character. */
	RXF_IGNORECASE	= 'i', /** Ignore Case			– Match case-insensitively. */

	/* —— Searching Behaviour ——————————————————————————————— */

	RXF_STICKY		= 'y', /** Sticky Search		– Perform a "sticky" search - all matches have to be sequential. */
	RXF_ANCHOR		= 'A', /** Anchor at Start		– First match is at start of text, others have to be sequential. */
	RXF_NOBACKTRACK	= 'N', /** No Backtracking		– Disable backtracking. */
	RXF_RIGHTTOLEFT	= 'R', /** Right to Left		– Match right to left. */

	/* —— Capturing Behaviour ——————————————————————————————— */

	RXF_DUPGROUPS	= 'J', /** Duplicate Groups		– Allow duplicate group names. */
	RXF_NONCAPTURE	= 'n', /** No Capture			– Groups are non-capturing by default. `(?:` makes them capture. */

	/* —— Return Format ————————————————————————————————————— */

	RXF_INVMATCH	= 'V', /** Invert Match			– Return everything that doesn't match. */

	/* —————————————————————————————————————————————————————— */
};

#endif /* !FLAGS_H_ */
