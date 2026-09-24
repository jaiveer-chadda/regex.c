/// @file types/_chars.h

#ifndef CHARS_H_
#define CHARS_H_

#define RX__CHAR_TABLE \
	X(LITERAL,  0  )				/** /`a`/				– A literal character.								*/ \
	X(INVALID, -1  )				/**						– An invalid character.								*/ \
	X(SPACE	 , ' ' )				/** /` `/				– Ignored in extended mode.							*/ \
	X(FSLASH , '/' )				/** `/m/r/f`			– Delimit a regex.									*/ \
	X(BSLASH , '\\')				/** /`\w`/				– Escape the next character.						*/ \
	X(DOT	 , '.' )				/** /`.`/				– Match one of any character.						*/ \
	X(STAR	 , '*' )				/** /`a*`/				– Match zero or more of a pattern.					*/ \
	X(PLUS	 , '+' )				/** /`a+`/				– Match one or more of a pattern.					*/ \
	X(HASH	 , '#' )				/** /`# ...`/, /`(?#)`/	– Begin a line comment, or start a comment group.	*/ \
	X(CARET	 , '^' )				/** /`\n^`/, /`[^...]`/	– Assert start of a line/str, or negate a literal.	*/ \
	X(COMMA	 , ',' )				/** /`{x,y}`/			– Separate the arguments of a ranged quantifier.	*/ \
	X(HYPHEN , '-' )				/** /`[a-z]`/			– Define a range of chars in a literal.				*/ \
	X(DOLLAR , '$' )				/** /`$\n`/				– Assert the end of a line/string.					*/ \
	X(LINEBAR, '|' )				/** /`a|b`/				– Match one pattern or another.						*/ \
	X(NEWLINE, '\n')				/** /`↩`/				– Mark the end of the line.							*/ \
	X(LPAREN , '(' ) X(RPAREN, ')')	/** /`(...)`/			– Define a group construct.							*/ \
	X(LBRACK , '[' ) X(RBRACK, ']')	/** /`[abc]`/			– Define a literal construct.						*/ \
	X(LBRACE , '{' ) X(RBRACE, '}')	/** /`a{1,2}`/			– Define a range quantifier.						*/ \
	X(QUMARK , '?' ) X(EXMARK, '!') X(EQUALS, '=' ) X(L_THAN, '<') \
/**/

#define X(name, chr) RXC_##name,
enum RxChar { RX__CHAR_TABLE RXC_COUNT };
#undef X

#endif /* !CHARS_H_ */
