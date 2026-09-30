/// @file output/print.h

#ifndef PRINT_H_
#define PRINT_H_

#include "match/match.h"
#include "parsing/parse.h"

void print_regex(const rxobj_t rx_obj);
void rx__print_token(const token_t token, const bool do_repr, FILE *const file);
void rx_print_matches(const matches_t matches);

#define repr(token)			rx__print_token(token, true	, stdout)
#define erepr(token)		rx__print_token(token, true	, stderr)
#define print_token(token)	rx__print_token(token, false, stdout)
#define eprint_token(token)	rx__print_token(token, false, stderr)

#endif /* !PRINT_H_ */
