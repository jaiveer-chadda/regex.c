/// @file debug/debug.h

#ifndef DEBUG_H_
#define DEBUG_H_

#define LOG_LEVEL_TABLE \
	X(TRACE		, 90) \
	X(DEBUG		, 34) \
	X(INFO		, 37) \
	X(SUCCESS	, 32) \
	X(WARNING	, 33) \
	X(ERROR		, 31) \
	X(FATAL		, 41) \

#define X(name, ...) LOG_##name,
typedef enum { LOG_LEVEL_TABLE LOG_COUNT } LogLevelIdx;
#undef X

typedef struct {
	char name[10];
	unsigned short colour;
} LogLevel;

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#ifdef DEBUG_MODE

#	include <inttypes.h>

#	include "match/match.h"
#	include "parsing/parse.h"

	void d__debug(
		const LogLevelIdx level_idx, const int lineno,
		const char *const time, const char *const file,
		const char *const func, const char *const fmt, ...
	);

	void stacktrace(void);
	void d__line(const uint8_t len);

	void dmatch(const matches_t *matches, const token_t *token, const size_t idx, const ssize_t len, const int depth);
	void dmatch_len(const size_t idx, const ssize_t match_len);

	/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#	define	  dlog(log_level, ...) d__debug(LOG_##log_level, __LINE__, __TIME__, __FILE__, __func__, __VA_ARGS__)
#	define	 trace(...) dlog(TRACE	, __VA_ARGS__)
#	define	 debug(...) dlog(DEBUG	, __VA_ARGS__)
#	define	  info(...) dlog(INFO	, __VA_ARGS__)
#	define success(...) dlog(SUCCESS, __VA_ARGS__)
#	define warning(...) dlog(WARNING, __VA_ARGS__)
#	define	 error(...) dlog(ERROR	, __VA_ARGS__)
#	define	 fatal(...) dlog(FATAL	, __VA_ARGS__)

	/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#	define DEFAULT_DLINE_LEN 120

#	ifdef TTYCOLUMNS
#		define arg1__dline(len)	d__line((uint8_t)(((len) * TTYCOLUMNS) / DEFAULT_DLINE_LEN))
#		define arg0__dline()	d__line((uint8_t)(TTYCOLUMNS))
#	else
#		define arg1__dline(len)	d__line((uint8_t)(len))
#		define arg0__dline()	d__line((uint8_t)(DEFAULT_DLINE_LEN))
#	endif

#	define dline__DISPATCH(_1, NAME, ...) NAME
#	define dline(...) dline__DISPATCH(__VA_ARGS__ __VA_OPT__(,) arg1__dline, arg0__dline)(__VA_ARGS__)

	/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#	define DEBUG_INPUT(i) do {										\
		dline(60);													\
		printf("\33[40m[input %zu] = '\33[4m", i);					\
		for (const char *chr = INPUTS[i]; *chr != '\0'; chr++) {	\
			printf("%s", (*chr == ' ' ? "·" : (char[2]){ *chr }));	\
		}															\
		fputs("\33[24m'\33[m   [regex] = ", stdout);				\
		print_regex(regex); putchar('\n');							\
	} while (0)

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#else /* !DEBUG_MODE */
#	define stacktrace()
#	define	  dlog(...)
#	define	 trace(...)
#	define	 debug(...)
#	define	  info(...)
#	define success(...)
#	define warning(...)
#	define	 error(...)
#	define	 fatal(...)
#	define	 dline(...)
#	define dmatch(token, chr, mlen, depth)	(void)(token), (void)(chr), (void)(mlen), (void)(depth)
#	define dmatch_len(pchar, len)			(void)(pchar), (void)(len)
#	define DEBUG_INPUT(i)					(void)(i)
#endif /* DEBUG_MODE */

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#endif /* !DEBUG_H_ */
