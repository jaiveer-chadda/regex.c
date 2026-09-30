/// @file debug/debug.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <execinfo.h>

#include "debug.h"
#include "_defs.h"

#include "output/print.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define X(name, ...) [LOG_##name] = { #name, __VA_ARGS__ }, 
static const LogLevel LOG_LEVELS[] = { LOG_LEVEL_TABLE };
#undef X

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define TAB "  "
#define MATCHED(len) ((len) != -1L)

void d__match(const char *const str, const token_t *token, const char *const chr, const ssize_t len, const int depth) {
	static const char *STRING_START = NULL;
	static ssize_t idx = -1;

	if (str != NULL) { STRING_START = str; return; }

	const bool same = (idx == chr - STRING_START);
	idx = chr - STRING_START;

	/* ———————————————————————————————————————————————————————————————————— */

	eprintf(TAB D("[") "%ld" D("] <") "%d" D(">"), chr - STRING_START, depth);

	/* ———————————————————————————————————————————————————————————————————— */

	eprintf("%s  ", MATCHED(len) ? "\33[32m" : "\33[31m");
	if (same) eputs("   "); else eprintf(D("'") "%c" D("'"), *chr);

	for (int i = 0; i < 8 - depth; i++) eputs(TAB);

	eputs("\33[2m————\33[m ");
	eprint_token(*token);

	/* ———————————————————————————————————————————————————————————————————— */

	eputc('\n');
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

void d__match_len(const char *const chr, const ssize_t match_len) {
	if (!MATCHED(match_len)) return;

	eprintf(
		TAB TAB
		"\33[32mmatched\33[92m"
		SP D("'") "%.*s"	D("'") "\33[m"
		SP D("(") "len %zd"	D(")")
		"\n",

		(int)match_len, chr, match_len
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

void d__stacktrace(void) {
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

// spell:ignoreRegexp /(\\(?:[␛e]|0?33|[xUu]1[Bb])|␛)\[[0-9;]*?m\B/g
