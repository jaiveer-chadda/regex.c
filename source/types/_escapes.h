/// @file types/_escapes.h

#ifndef ESCAPES_H_
#define ESCAPES_H_

enum RxEscape {

	/* —— C Escape Sequences ———————————————————————————————— */

	RXE_NULL	 = '0', /** `\0 (\x00)` – Nullbyte */
	RXE_ALERT	 = 'a', /** `\a (\x07)` – Alert / Bell */
	RXE_HORTAB	 = 't', /** `\t (\x09)` – Horizontal tab */
	RXE_LINEFD	 = 'n', /** `\n (\x0A)` – Line feed */
	RXE_VERTAB	 = 'v', /** `\v (\x0B)` – Vertical tab */
	RXE_FORMFD	 = 'f', /** `\f (\x0C)` – Form feed */
	RXE_RETURN	 = 'r', /** `\r (\x0D)` – Carriage return */
	RXE_ESCAPE	 = 'e', /** `\e (\x1B)` – Escape character */
	RXE_BSLASH	 = '\\',/** `\  (\x5C)` – Backslash */

	RXE_HEXESC	 = 'x', /** `\xYY` / `\x{YYYY}` – Match the hex digit `YY` (1-2 digits), or `YYYY` (1-6 digits). */

	/* —— C Escape Negation ————————————————————————————————— */

	RXE_NOHORTAB = 'T', /** `\T` = `[^\t]` – Match anything except a `\t` horizontal tab. */
	RXE_NOLINEFD = 'N', /** `\N` = `[^\n]` – Match anything except a `\n` line feed. */
	RXE_NOVERTAB = 'V', /** `\V` = `[^\v]` – Match anything except a `\v` vertical tab. */
	RXE_NOFORMFD = 'F', /** `\F` = `[^\f]` – Match anything except a `\f` form feed. */
	RXE_NOESCAPE = 'E', /** `\E` = `[^\e]` – Match anything except an`\e` escape character. */

	/* —— Groups / Backreferencing —————————————————————————— */

	/** `\1 → \9` – A backreference - match the `1ˢᵗ` through `9ᵗʰ` capture group, respectively. */
	RXE_1='1', RXE_2, RXE_3, RXE_4, RXE_5, RXE_6, RXE_7, RXE_8, RXE_9,
	/**
	 * `\gN` / `\g{N}` / `\g<N>` / `\g'N'` – Match the @b exact text matched and captured by the `Nᵗʰ` capture group.
	 *
	 * `\g{±N}` – Match the @b exact text matched and captured by the group `N` groups ahead (`+`) or behind (`-`).
	 *
	 * `\g<±N>` / `\g'±N'` – Match the expression defined in the group `N` groups ahead (`+`) or behind (`-`).
	 *
	 * `\g{name}` / `\g<name>` / `\g'name'` – Match the @b exact text matched and captured by the capture group `name`.
	 */
	RXE_NTHGROUP = 'g',
	RXE_NAMEDGRP = 'k', /** `\k{name}` / `\k<name>` / `\k'name'` – Match the text matched and captured by `name`. */

	/* —— Anchors ——————————————————————————————————————————— */

	RXE_STRSTART = 'A', /** `\A` – Assert start of the string (`\A...` = `^...` with multiline flag off). */
	RXE_SEQUENCE = 'G', /** `\G` – Assert start of the string, or end of the previous match. */

	RXE_STREND	 = 'z', /** `\z` – Assert end of the string (`...\z` = `...$` with multiline flag off). */
	RXE_STRENDNL = 'Z', /** `\Z` – Assert optional `\n`, then end of string (`\Z` = `\R?$` with multiline flag off). */

	RXE_BOUNDARY = 'b', /** `\b` – Assert word boundary (`...\b` = `...(?<=\w)(?=\W)`). */
	RXE_NOBOUND	 = 'B', /** `\B` – Assert no word boundary (`...\B` = `...((?<=\w)(?=\w)|(?<=\W)(?=\W))`). */

	/* —— Character Classes ————————————————————————————————— */

	RXE_DIGIT	 = 'd', /** `\d` ≈ `[0-9]` – Match any digit. */
	RXE_SPACE	 = 's', /** `\s` ≈ `[ \t\n\v\f\r]` – Match any whitespace character. */
	RXE_WORD	 = 'w', /** `\w` ≈ `[A-Za-z_]` – Match any word character. */

	RXE_NODIGIT	 = 'D', /** `\D` ≈ `[^0-9]` – Match any non-digit. */
	RXE_NOSPACE	 = 'S', /** `\S` ≈ `[^ \t\n\v\f\r]` – Match any non-whitespace character. */
	RXE_NOWORD	 = 'W', /** `\W` ≈ `[^A-Za-z_]` – Match any non-word character. */

	RXE_NEWLINE	 = 'R', /** `\R` = `(\r\n|[\n\v\f\r\x85])` – Match any Unicode newline sequence. */

	/* —— Unicode Classes ——————————————————————————————————— */

	RXE_ANYCHAR	 = 'X', /** `\X` = `(?s:.)` – Match any valid Unicode character, including a newline. */
	RXE_ONEBYTE	 = 'C', /** `\C` = `(?-s:.)` – Match any single data unit - regardless of Unicode settings. */

	/**
	 * `\pX` – Match a unicode character with the
	 *			given [property](https://www.fileformat.info/info/unicode/category/index.htm)`X`.
	 *
	 * `\p{...}` – Match a unicode character with the
	 *			given group of properties or [categories](https://www.regular-expressions.info/unicodecategory.html).
	 */
	RXE_PROPERTY = 'p',
	/**
	 * `\PX` / `\P{...}` – Negation of `\pX` / `\p{...}`.
	 */
	RXE_NOPROPERTY = 'P',

	/* —— Behaviour-Changing ———————————————————————————————— */

	/**
	 * `\Q...\E` – Define a quoted region in which everything, including escape characters, are treated as literals.
	 */
	RXE_BEGQUOTE = 'Q', RXE_ENDQUOTE = 'E',
	RXE_RESETPOS = 'K', /** `\K` – Set this position in the regex as the new start of the match. */

	/* —————————————————————————————————————————————————————— */
};

/// @note order of escapes with multiple forms is: `\xA`, `\x{A}`, `\x<A>`, `\x'A'`

#endif /* !ESCAPES_H_ */
