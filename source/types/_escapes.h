/// @file types/_escapes.h

#ifndef ESCAPES_H_
#define ESCAPES_H_

#define RX__ESCS_TABLE \
	/* —— C Escape Sequences —————————————————————————————————————————————————————————————————————————————————— */	\
	X(NULL		, '0') /** `\0 (\x00)` – Nullbyte																*/	\
	X(ALERT		, 'a') /** `\a (\x07)` – Alert / Bell															*/	\
	X(HORTAB	, 't') /** `\t (\x09)` – Horizontal tab															*/	\
	X(LINEFD	, 'n') /** `\n (\x0A)` – Line feed																*/	\
	X(VERTAB	, 'v') /** `\v (\x0B)` – Vertical tab															*/	\
	X(FORMFD	, 'f') /** `\f (\x0C)` – Form feed																*/	\
	X(RETURN	, 'r') /** `\r (\x0D)` – Carriage return														*/	\
	X(ESCAPE	, 'e') /** `\e (\x1B)` – Escape character														*/	\
	X(HEXESC	, 'x') /** `\xYY` / `\x{YYYY}` – Match the hex digit `YY` (1-2 digits), or `YYYY` (1-6 digits).	*/	\
	/* —— C Escape Negation ——————————————————————————————————————————————————————————————————————————————————— */	\
	X(NOHORTAB	, 'T') /** `\T` = `[^\t]` – Match anything except a `\t` horizontal tab.						*/	\
	X(NOLINEFD	, 'N') /** `\N` = `[^\n]` – Match anything except a `\n` line feed.								*/	\
	X(NOVERTAB	, 'V') /** `\V` = `[^\v]` – Match anything except a `\v` vertical tab.							*/	\
	X(NOFORMFD	, 'F') /** `\F` = `[^\f]` – Match anything except a `\f` form feed.								*/	\
	X(NOESCAPE	, 'E') /** `\E` = `[^\e]` – Match anything except an`\e` escape character.						*/	\
	/* —— Groups / Backreferencing ———————————————————————————————————————————————————————————————————————————— */	\
	/** `\1 → \9` – A backreference - match the `1ˢᵗ` through `9ᵗʰ` capture group, respectively.				*/	\
	X(1,'1') X(2,'2') X(3,'3') X(4,'4') X(5,'5') X(6,'6') X(7,'7') X(8,'8') X(9,'9')								\
	/**
	 * `\gN` / `\g{N}` / `\g<N>` / `\g'N'` – Match the @b exact text matched and captured by the `Nᵗʰ` capture group.
	 *
	 * `\g{±N}` – Match the @b exact text matched and captured by the group `N` groups ahead (`+`) or behind (`-`).
	 *
	 * `\g<±N>` / `\g'±N'` – Match the expression defined in the group `N` groups ahead (`+`) or behind (`-`).
	 *
	 * `\g{name}` / `\g<name>` / `\g'name'` – Match the @b exact text matched and captured by the capture group `name`.
	 */ \
	X(NTHGROUP	, 'g')																								\
	X(NAMEDGRP	, 'k') /** `\k{name}` / `\k<name>` / `\k'name'` – Match the text matched and captured by `name`.*/	\
	/* —— Anchors ————————————————————————————————————————————————————————————————————————————————————————————— */	\
	X(STRSTART	, 'A') /** `\A` – Assert start of the string (`\A...` = `^...` with multiline flag off).		*/	\
	X(SEQUENCE	, 'G') /** `\G` – Assert start of the string, or end of the previous match.						*/	\
	X(STREND	, 'z') /** `\z` – Assert end of the string (`...\z` = `...$` with multiline flag off).			*/	\
	X(STRENDNL	, 'Z') /** `\Z` – Assert optional `\n`, then end of str (`\Z` = `\R?$` with multiline flag off).*/	\
	X(BOUNDARY	, 'b') /** `\b` – Assert word boundary (`...\b` = `...(?<=\w)(?=\W)`).							*/	\
	X(NOBOUND	, 'B') /** `\B` – Assert no word boundary (`...\B` = `...((?<=\w)(?=\w)|(?<=\W)(?=\W))`).		*/	\
	/* —— Character Classes ——————————————————————————————————————————————————————————————————————————————————— */	\
	X(DIGIT		, 'd') /** `\d` ≈ `[0-9]` – Match any digit.													*/	\
	X(SPACE		, 's') /** `\s` ≈ `[ \t\n\v\f\r]` – Match any whitespace character.								*/	\
	X(WORD		, 'w') /** `\w` ≈ `[A-Za-z_]` – Match any word character.										*/	\
	X(NODIGIT	, 'D') /** `\D` ≈ `[^0-9]` – Match any non-digit.												*/	\
	X(NOSPACE	, 'S') /** `\S` ≈ `[^ \t\n\v\f\r]` – Match any non-whitespace character.						*/	\
	X(NOWORD	, 'W') /** `\W` ≈ `[^A-Za-z_]` – Match any non-word character.									*/	\
	X(NEWLINE	, 'R') /** `\R` = `(\r\n|[\n\v\f\r\x85])` – Match any Unicode newline sequence.					*/	\
	/* —— Unicode Classes ————————————————————————————————————————————————————————————————————————————————————— */	\
	X(ANYCHAR	, 'X') /** `\X` = `(?s:.)` – Match any valid Unicode character, including a newline.			*/	\
	X(ONEBYTE	, 'C') /** `\C` = `(?-s:.)` – Match any single data unit - regardless of Unicode settings.		*/	\
	/**
	 * `\pX` – Match a unicode character with the
	 *			given [property](https://www.fileformat.info/info/unicode/category/index.htm)`X`.
	 *
	 * `\p{...}` – Match a unicode character with the
	 *			given group of properties or [categories](https://www.regular-expressions.info/unicodecategory.html).
	 */ \
	X(PROPERTY	, 'p')																								\
	X(NOPROPERTY, 'P') /** `\PX` / `\P{...}` – Negation of `\pX` / `\p{...}`.									*/	\
	/* —— Behaviour-Changing —————————————————————————————————————————————————————————————————————————————————— */	\
	/** `\Q...\E` – Define a quoted region in which everything, including esc chars, are treated as literals.	*/	\
	X(BEGQUOTE	, 'Q') X(ENDQUOTE, 'E')																				\
	X(RESETPOS	, 'K') /** `\K` – Set this position in the regex as the new start of the match.					*/	\
/**/

#define X(name, chr) RXE_##name,
enum RxEscape { RX__ESCS_TABLE RXE_COUNT };
#undef X

#endif /* !ESCAPES_H_ */
