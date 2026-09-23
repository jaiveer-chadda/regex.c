/// @file types/_flags.h

#ifndef FLAGS_H_
#define FLAGS_H_

enum RxFlag {
	RXF_GLOBAL		= 'g', RXF_g = 'g', /** Find all matches instead of just the first. */
	RXF_MULTILINE	= 'm', RXF_m = 'm', /** Match `^` and `$` at line boundaries. */
	RXF_IGNORECASE	= 'i', RXF_i = 'i', /** Ignore case. */

	RXF_EXTENDED	= 'x', RXF_x = 'x', /** Spaces are ignored, and EoL comments are enabled. */
	RXF_NAMEDONLY	= 'n', RXF_n = 'n', /** Named captures only. */
	RXF_SINGLELINE	= 's', RXF_s = 's', /** The `.` wildcard matches `\n`. */
	RXF_NOBACKTRACK	= 'N', RXF_N = 'N', /** Disable backtracking. */
	RXF_RIGHTTOLEFT	= 'R', RXF_R = 'R', /** Match right to left. */

	RXF_UNICODE		= 'u', RXF_u = 'u', /** Treat multibyte characters as a single character. */
	RXF_DOLLAREND	= 'D', RXF_D = 'D', /** Match `$` only at string end. */
	RXF_DUPGROUPS	= 'J', RXF_J = 'J', /** Allow duplicate group names. */

	RXF_STICKY		= 'y', RXF_y = 'y', /** ? Perform a "sticky" search. */
	RXF_ANCHOR		= 'A', RXF_A = 'A', /** ? Anchor at search start. */
	RXF_LAZYQUANT	= 'U', RXF_U = 'U', /** ? Lazy quantifiers. */
	RXF_NOCASEFOLD	= 'r', RXF_r = 'r', /** ? Restrict case folding. */
};

#endif /* !FLAGS_H_ */
