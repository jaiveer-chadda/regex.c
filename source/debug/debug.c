/// @file debug/debug.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <execinfo.h>

#include "debug.h"
#include "_defs.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define X(name, ...) [LOG_##name] = { #name, __VA_ARGS__ }, 
static const LogLevel LOG_LEVELS[] = { LOG_LEVEL_TABLE };
#undef X

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

// spell:ignore LNNO
