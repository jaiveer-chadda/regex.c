/// @file types/_groups.h

#ifndef GROUPS_H_
#define GROUPS_H_

enum RxGroup {
	RXG_REGULAR	 , /** `(...)` – Capturing Group. */

	RXG_NON_CAPT , /** `(?:...)` – Non-Capturing Group. */
	RXG_COMMENT	 , /** `(?#...)` – Comment Group. */
	RXG_SAME_NUM , /** `(?|...)` – Duplicate Group Number. All enclosed groups have the same captured group number. */

	/** `(?>...)`,`(*atomic:...)` – Non-Capturing Atomic Group.
	 * Match the longest possible substring in the group and don't allow later backtracking into this group.
	 */
	RXG_ATOMIC,
	/** `(?*...)`,`(*napla:...)`,`(*non_atomic_positive_lookahead:...)` – Non-Atomic Positive Lookahead.
	 * Lookahead allowing backtracking inside the assertion if the rest of the pattern fails.
	 */
	RXG_NAPLA,
	/** `(?<*...)`,`(*naplb:...)`,`(*non_atomic_positive_lookbehind:...)` – Non-Atomic Positive Lookbehind.
	 *	Lookbehind allowing backtracking inside the assertion if the rest of the pattern fails.
	 */
	RXG_NAPLB,

	RXG_NAMED, /** `(?'name'...)`,`(?<name>...)`,`(?P<name>...)` – Named Capturing Group. */
	RXG_FLAGS, /** `(?flags:...)` – Flag Group. Enable the specified flags within this group. */

	/** `(?=...)`, `(*pla:...)`,`(*positive_lookahead:...)`  – Positive Lookahead.
	 *	Assert that the string after this point is matched by a pattern.
	 */
	RXG_PLA,
	/** `(?!...)`, `(*nla:...)`,`(*negative_lookahead:...)`  – Negative Lookahead.
	 *	Assert that the string after this point is not matched by a pattern.
	 */
	RXG_NLA,
	/** `(?<=...)`,`(*plb:...)`,`(*positive_lookbehind:...)` – Positive Lookbehind.
	 *	Assert that the string before this point is matched by a pattern.
	 */
	RXG_PLB,
	/** `(?<!...)`,`(*nlb:...)`,`(*negative_lookbehind:...)` – Negative Lookbehind.
	 *	Assert that the string before this point is not matched by a pattern.
	 */
	RXG_NLB,
};

/* ——————————————————————————————————————————————————————————————————————————————————————————————————————————————

-v- (...)
-v- (?:...)
-v- (?#...)
-v- (?|...)
-v- (?>...)
-v- (?*...)
-v- (?<*...)
-v- (?'name'...)
-v- (?<name>...)
-v- (?P<name>...)
-v- (?flags:...)
-v- (?=...)
-v- (?!...)
-v- (?<=...)
-v- (?<!...)

—————————————————————————————————————————————————————————————————————————————————————————————————————————————————

-v- (*atomic:...)							[(?>)]
-v- (*pla:...)								[(?=)]
-v- (*nla:...)								[(?!)]
-v- (*plb:...)								[(?<=)]
-v- (*nlb:...)								[(?<!)]
-v- (*napla:...)							[(?*)]
-v- (*naplb:...)							[(?<*)]
-v- (*positive_lookahead:...)				[(?=)]
-v- (*negative_lookahead:...)				[(?!)]
-v- (*positive_lookbehind:...)				[(?<=)]
-v- (*negative_lookbehind:...)				[(?<!)]
-v- (*non_atomic_positive_lookahead:...)	[(?*)]
-v- (*non_atomic_positive_lookbehind:...)	[(?<*)]

—————————————————————————————————————————————————————————————————————————————————————————————————————————————————

-x- (?(1)...|...)
-x- (?(R)...|...)
-x- (?(R#)...|...)
-x- (?(R&name)...|...)
-x- (?(?=...)...|...)
-x- (?(?<=...)...|...)
-x- (?(DEFINE)...)

—————————————————————————————————————————————————————————————————————————————————————————————————————————————————

-x- (?R)
-x- (?1)
-x- (?+1)
-x- (?flags)
-x- (?&name)
-x- (?P=name)
-x- (?P>name)

—————————————————————————————————————————————————————————————————————————————————————————————————————————————————

-x- (*scs:(grouplist)...)					[---]
-x- (*scan_substring:(grouplist)...)		[---]
-x- (*sr:...)								[---]
-x- (*script_run:...)						[---]

—————————————————————————————————————————————————————————————————————————————————————————————————————————————————

-x- (*ACCEPT)								[---]
-x- (*FAIL)									[(?!)]
-x- (*MARK:NAME)							[(*:NAME)]
-x- (*COMMIT)								[---]
-x- (*PRUNE)								[---]
-x- (*SKIP)									[---]
-x- (*THEN)									[---]

-x- (*UTF)									[---]
-x- (*UTF8)									[---]
-x- (*UTF16)								[---]
-x- (*UTF32)								[---]
-x- (*UCP)									[---]

-x- (*CR)									[---]
-x- (*LF)									[---]
-x- (*CRLF)									[---]
-x- (*ANYCRLF)								[---]
-x- (*ANY)									[---]

-x- (*NOTEMPTY)								[---]
-x- (*NOTEMPTY_ATSTART)						[---]

-x- (*NO_JIT)								[---]

-x- (*BSR_ANYCRLF)							[---]
-x- (*BSR_UNICODE)							[---]

-x- (*LIMIT_MATCH=x)						[---]
-x- (*LIMIT_RECURSION=d)					[---]
-x- (*NO_AUTO_POSSESS)						[---]
-x- (*NO_START_OPT)							[---]

————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#endif /* !_GROUPS_H_ */

// spell:ignoreRegExp /(?:\b|_)\w?apl\w\b/gi
