/// @file match/match.h

#ifndef MATCH_H_
#define MATCH_H_

#include "parsing/parse.h"

typedef struct match_s { size_t idx, len; } match_t;

typedef struct matches_s {
	const char *const string;
	match_t *arr; size_t len;
} matches_t;

matches_t rx_match(const rxobj_t rx_obj, const char *const string);

#endif /* !MATCH_H_ */
