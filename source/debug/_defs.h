/// @file debug/_defs.h

#ifndef DEFS_H_
#define DEFS_H_

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define RESET "\33[m"

#define ANSI(code)	"\33[" code "m"
#define ANSI8(code) "\33[38;5;" #code "m"

#define DIM ANSI("2")
#define D(str) DIM str ANSI("22")

#define COLOURS ((unsigned[]){ 203, 215, 220, 227, 191, 155, 84, 86, 81, 75, 105, 141, 177, 213, 211 })
// #define COLOURS ((unsigned[]){ 203, 215, 227, 155, 86, 75, 141, 213})
#define COL_COUNT ((int)(sizeof(COLOURS) / sizeof(COLOURS[0])))

#define GET_COL(idx) (COLOURS[(idx) % COL_COUNT])

/* ———————————————————————————————————————————— */

#define MAX_DEPTH 8

/* ———————————————————————————————————————————— */

#define REL_PATH(file) (char *)(strstr((char *)(file), "source/") + (int)strlen("source/"))

/* ———————————————————————————————————————————— */

#define SP " "

#define NAME_LEN 8
#define TIME_LEN (int)(sizeof("19:47:42") - 1)
#define FUNC_LEN 12
#define FILE_LEN 24
#define LNNO_LEN 3

#define TOTAL_LEN (int)(\
	NAME_LEN + 1 +		\
	TIME_LEN + 1 +		\
	FUNC_LEN + 1 +		\
	FILE_LEN + 1 +		\
	LNNO_LEN + 1 +		\
	sizeof(				\
		SP "[]" "[]"	\
		SP "@"  "()"	\
	) - 1				\
)

/* ———————————————————————————————————————————— */

#define	 PERR(...) do { fprintf(stderr, __VA_ARGS__); fflush(stderr); } while (0)
#define	PSERR(str) do { fputs(str, stderr);			  fflush(stderr); } while (0)
#define eputc(chr)	putc(chr, stderr)

#define eprintf	PERR
#define eputs	PSERR

#define STACK_MAX 128

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#endif /* !DEFS_H_ */

// spell:ignore LNNO
