/// @file debug/debug.h

#ifndef DEBUG_H_
#define DEBUG_H_

#include <inttypes.h>

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

	void d__debug(
		const LogLevelIdx level_idx, const int lineno,
		const char *const time, const char *const file,
		const char *const func, const char *const fmt, ...
	);

	void d__stacktrace(void);
	void d__line(const uint8_t len);

	/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#	define	  dlog(log_level, ...) d__debug(LOG_##log_level, __LINE__, __TIME__, __FILE__, __func__, __VA_ARGS__)
#	define	 trace(...) dlog(TRACE	, __VA_ARGS__)
#	define	 debug(...) dlog(DEBUG	, __VA_ARGS__)
#	define	  info(...) dlog(INFO	, __VA_ARGS__)
#	define success(...) dlog(SUCCESS, __VA_ARGS__)
#	define warning(...) dlog(WARNING, __VA_ARGS__)
#	define	 error(...) dlog(ERROR	, __VA_ARGS__)
#	define	 fatal(...) dlog(FATAL	, __VA_ARGS__)
#	define stacktrace()			 d__stacktrace()

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

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#else
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
#endif

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#endif /* !DEBUG_H_ */
