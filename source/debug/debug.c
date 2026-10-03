/// @file debug/debug.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <execinfo.h>

#include "debug.h"
#include "_defs.h"

#include "output/print.h"

#ifdef DEBUG_MODE

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define X(name, ...) [LOG_##name] = { #name, __VA_ARGS__ },
static const LogLevel LOG_LEVELS[] = { LOG_LEVEL_TABLE };
#undef X

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define TAB "  "
#define MATCHED(len) ((len) != -1L)

#define GET_CHAR(chr_)		\
	(chr_) == '\0'	? "∅" :	\
	(chr_) == ' '	? "·" :	\
	(char[2]){ (chr_) }

#define STRING ((char*)(MATCHES->string))

static ssize_t CHR_IDX = -1;
static const matches_t *MATCHES	= NULL;

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

void dmatch(const matches_t *matches, const token_t *token, const size_t idx, const ssize_t len, const int depth) {
	const bool same = ((size_t)CHR_IDX == idx);

	MATCHES = matches;
	CHR_IDX = idx;

	/* ———————————————————————————————————————————————————————————————————— */

	eprintf( TAB // prefix, i.e. `[3] <5>`
		ANSI8(%d) D("[") "%2ld"	D("]") RESET " "
		ANSI8(%d) D("<") "%d"	D(">") RESET,

		GET_COL(idx	 ), idx,
		GET_COL(depth), depth
	);

	/* ———————————————————————————————————————————————————————————————————— */

	eprintf("%s  ", MATCHED(len) ? "\33[32m" : "\33[31m"); // purely colour

	// print the char that we're trying to match
	if (same) eputs("   ");
	else {
		eputs(D("'")); eputs(GET_CHAR(STRING[idx])); eputs(D("'"));
	}

	for (int i = 0; i < 6	 ; i++) eputs(TAB); // basic indentation
	for (int i = 0; i < depth; i++) eputs(TAB); // tiered indentation

	eputs("\33[2m————\33[m "); // line
	eprint_token(*token); // token

	/* ———————————————————————————————————————————————————————————————————— */

	eputc('\n');
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

void dmatch_len(const size_t idx, const ssize_t match_len) {
	CHR_IDX = -1;
	eputs(TAB TAB);

	if (!MATCHED(match_len)) {
		eprintf(
			"\33[91mno match "
			ANSI8(%d) D("[") "%ld"D("]") RESET " "
			"\33[92m" D("'") "%s" D("'")
			"\33[m\n",

			GET_COL(idx), idx,
			GET_CHAR(STRING[idx])
		);
		return;
	}

	eprintf(
		"\33[92mmatched  "
		ANSI8(%d) D("[") "%ld"D("]") RESET " "
		"\33[92m" D("'") "%.*s"	D("'") "\33[m"
		SP D("(") "len %zd"	D(")")
		"\n",

		GET_COL(idx), idx,
		(int)match_len, &STRING[idx],
		match_len
	);
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

void d__debug(
	const LogLevelIdx level_idx, const int lineno,
	const char *const time, const char *const file,
	const char *const func, const char *const fmt, ...
) {
	const LogLevel level = LOG_LEVELS[level_idx < LOG_COUNT ? level_idx : LOG_DEBUG];

	/* ———————————————————————————————————————————————————————————————————— */

	PERR(ANSI("%hu")						,			level.colour	); // colour
	PERR(D("[")	SP	"%-*s"	D("]") RESET " ", NAME_LEN, level.name		); //	[ WARNING ]
	PERR(D("[")		"%*s"	D("]") RESET " ", TIME_LEN, time			); //		[02:41:15]
	PERR(ANSI8(217)	"%*s"SP	D("@")		 " ", FUNC_LEN, func			); //			getTargetInfo @
	PERR(ANSI8(111)	"%-*s"				 " ", FILE_LEN, REL_PATH(file)	); //				info/get-file-info.c
	PERR(D("(")		"%*d"	D(")") RESET " ", LNNO_LEN, lineno			); //					(110)
	PERR(ANSI("%hu")						,			level.colour	); // colour

	/* ———————————————————————————————————————————————————————————————————— */

	// fill the buffer with the output of `printf`
	char *pbuffer;
	va_list va_args;

	va_start(va_args, fmt); // `fmt` is the last known fixed argument
	vasprintf(&pbuffer, fmt, va_args);
	va_end(va_args);

	/* —————————————————————————————————— */

	for (const char *chr = pbuffer; *chr != '\0'; chr++) {
		fputc(*chr, stderr);
		if (*chr == '\n') PERR("%*s", TOTAL_LEN, "");
	}

	free(pbuffer);
	PSERR(RESET "\n");
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

void d__line(const uint8_t len) {
	PSERR(DIM);
	for (uint8_t i = 0; i < len; i++) PSERR("─");
	PSERR(RESET "\n");
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

void stacktrace(void) {
	void *stack_buffer[STACK_MAX];

	// get the current stack return addresses
	int trace_size = backtrace(stack_buffer, STACK_MAX);
	// translate addresses into strings
	char **symbols = backtrace_symbols(stack_buffer, trace_size);

	if (symbols == NULL) { error("`stacktrace` failed"); return; }

	dline();

	PERR("%s function call stack (depth: %d) %s\n", "────────", trace_size, "────────");
	for (int i = 1; i < trace_size; i++) PERR("[%d] %s\n", i - 1, symbols[i]);
	dline();

	free(symbols);
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#endif /* !DEBUG_MODE */

// spell:ignoreRegexp /(\\(?:[␛e]|0?33|[xUu]1[Bb])|␛)\[[0-9;]*?m\B|LNNO/g
