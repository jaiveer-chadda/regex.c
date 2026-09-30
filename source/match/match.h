/// @file match/match.h

#ifndef MATCH_H_
#define MATCH_H_

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#include "parsing/parse.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

typedef struct match_s { size_t idx, len; } match_t;

typedef struct matches_s {
	const char *const string;
	match_t *arr; size_t len;
	char *const captures;
} matches_t;

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

matches_t rx_match(const rxobj_t rx_obj, const char *const string);
bool rx_match_class(const char class, const char chr);

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#endif /* !MATCH_H_ */
