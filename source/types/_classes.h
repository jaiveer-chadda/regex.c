/// @file types/_classes.h

#ifndef CLASSES_H_
#define CLASSES_H_

#define RX__CLASS_TABLE \
	X(DOT		, '.') \
	X(DIGIT		, 'd') \
	X(SPACE		, 's') \
	X(WORD		, 'w') \
	X(NODIGIT	, 'D') \
	X(NOSPACE	, 'S') \
	X(NOWORD	, 'W') \
	X(NOHORTAB	, 'T') \
	X(NOLINEFD	, 'N') \
	X(NOVERTAB	, 'V') \
	X(NOFORMFD	, 'F') \
	X(NEWLINE	, 'R') \
/**/

#define X(name, chr) RXL_##name = chr,
enum RxClass { RX__CLASS_TABLE RXL_COUNT };
#undef X

#endif /* !CLASSES_H_ */
