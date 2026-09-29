/// @file match/match.h

#ifndef MATCH_H_
#define MATCH_H_

#include "parsing/parse.h"

typedef struct { size_t *indices, *lengths; } match_t;

match_t rx_match(rxobj_t rx_obj, const char *const string);

#endif /* !MATCH_H_ */
