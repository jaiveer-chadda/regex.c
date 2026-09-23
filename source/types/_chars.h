/// @file types/_chars.h

#ifndef CHARS_H_
#define CHARS_H_

enum RxChar {
	RXC_INVALID	= -1,	/** An invalid character. */
	RXC_LITERAL	=  0,	/** /`a`/ – A literal character. */

	RXC_SPACE	= ' ',	/** /` `/ – Ignored. */

	RXC_FSLASH	= '/',	/** `/m/r/f` – Delimit a regex. */
	RXC_BSLASH	= '\\',	/** /`\w`/ – Escape the next character. */

	RXC_DOT		= '.',	/** /`.`/ – Match one of any character. */
	RXC_STAR	= '*',	/** /`a*`/ – Match zero or more of a pattern. */
	RXC_PLUS	= '+',	/** /`a+`/ – Match one or more of a pattern. */
	RXC_HASH	= '#',	/** /`# comment`/, /`(?#...)`/ – Begin an EoL comment, or create a comment group construct. */
	RXC_COMMA	= ',',	/** /`{x,y}`/ – Separate a ranged quantifier's arguments. */
	RXC_CARET	= '^',	/** /`\n^`/, /`[^...]`/ – Assert the start of a line/string, or negate a literal construct. */
	RXC_DOLLAR	= '$',	/** /`$\n`/ – Assert the end of a line/string. */
	RXC_LINEBAR	= '|',	/** /`a|b`/ – Match one pattern or another. */
	RXC_NEWLINE	= '\n',	/** /`↩`/ – Mark the end of the line. */

	RXC_LPAREN	= '(', RXC_RPAREN = ')', /** /`(...)`/  – Define a group construct. */
	RXC_LBRACK	= '[', RXC_RBRACK = ']', /** /`[abc]`/  – Define a literal construct. */
	RXC_LBRACE	= '{', RXC_RBRACE = '}', /** /`a{1,2}`/ – Define a range quantifier. */

	RXC_QUMARK	= '?', /** /`(?:`/ */
	RXC_EXMARK	= '!', /** /`(?!`/ */

	RXC_EQUALS	= '=', /** /`(?=`/ */
	RXC_L_THAN	= '<', /** /`(?<=`/ */

	RXC_HYPHEN	= '-', /** /`[a-z]`/ – Define a range of characters in a literal construct. */
};

#endif /* !CHARS_H_ */
