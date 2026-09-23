/// @file types/_flags.h

#ifndef FLAGS_H_
#define FLAGS_H_

#define RX__FLAG_TABLE \
	/* —— General —————————————————————————————————————————————————————————————————————————————————————————————— */	 \
	X(GLOBAL	 , 'g' /** Global				– Find all matches instead of just the first.					 */) \
	X(EXTENDED	 , 'x' /** Extended				– Spaces are ignored, and EoL comments are enabled.				 */) \
	/* —— Regex Parsing ———————————————————————————————————————————————————————————————————————————————————————— */	 \
	X(WORDONLY	 , 'w' /** Words Only			– Only match entire words.										 */) \
	X(FIXEDSTR	 , 'F' /** Fixed Strings		– Treat the whole pattern as a literal.							 */) \
	X(UNGREEDY	 , 'U' /** Ungreedy Quantifiers	– Quantifiers are lazy by default. `?` makes them greedy.		 */) \
	X(MULTILINE	 , 'm' /** Multiline Matching	– Match `^` and `$` at line boundaries.							 */) \
	X(DOLLAREND	 , 'D' /** Dollar Marks End		– Match `$` only at the absolute end of the string.				 */) \
	X(SINGLELINE , 's' /** Single Line			– The dot `.` widcard matches newlines `\n`.					 */) \
	/* —— Input Parsing ———————————————————————————————————————————————————————————————————————————————————————— */	 \
	X(UNICODE	 , 'u' /** Unicode				– Treat multibyte characters as a single character.				 */) \
	X(IGNORECASE , 'i' /** Ignore Case			– Match case-insensitively.										 */) \
	/* —— Searching Behaviour —————————————————————————————————————————————————————————————————————————————————— */	 \
	X(STICKY	 , 'y' /** Sticky Search		– Perform a "sticky" search - all matches have to be sequential. */) \
	X(ANCHOR	 , 'A' /** Anchor at Start		– First match is at start of text, others have to be sequential. */) \
	X(NOBACKTRACK, 'N' /** No Backtracking		– Disable backtracking.											 */) \
	X(RIGHTTOLEFT, 'R' /** Right to Left		– Match right to left.											 */) \
	/* —— Capturing Behaviour —————————————————————————————————————————————————————————————————————————————————— */	 \
	X(DUPGROUPS	 , 'J' /** Duplicate Groups		– Allow duplicate group names.									 */) \
	/* —— Return Format ———————————————————————————————————————————————————————————————————————————————————————— */	 \
	X(NONCAPTURE , 'n' /** No Capture			– Groups are non-capturing by default. `(?:` makes them capture. */) \
	X(INVMATCH	 , 'V' /** Invert Match			– Return everything that doesn't match.							 */) \
/**/

#define X(name, chr) RXF_##name,
enum RxFlag { RX__FLAG_TABLE RXF_COUNT };
#undef X

#endif /* !FLAGS_H_ */
