/// @file match/match.h

#ifndef MATCH_H_
#define MATCH_H_

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#include "parsing/parse.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define IN_RANGE(c, min, max) ((min) <= (c) && (c) <= (max))

#define IS_LOWER(c) IN_RANGE(c, 'a', 'z')
#define IS_UPPER(c) IN_RANGE(c, 'A', 'Z')
#define IS_DIGIT(c) IN_RANGE(c, '0', '9')

#define IS_WORDC(c) (IS_LOWER(c) || IS_UPPER(c) || IS_DIGIT(c) || c == '_')
#define IS_SPACE(c) ((c) == ' ' || (c) == '\n' || (c) == '\t' || (c) == '\v' || (c) == '\f' || (c) == '\r')

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

typedef struct match_s { size_t idx, len; } match_t;

typedef struct matches_s {
	const char *const string;
	char **const captures;
	match_t *arr;
	size_t len, num_cap, str_len;
} matches_t;

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

matches_t rx_match(const rxobj_t rx_obj, const char *const str);
bool rx_match_class(const char class, const char chr);
ssize_t rx_match_anchor(const char anchor, const size_t idx, const matches_t *const matches);

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#endif /* !MATCH_H_ */
