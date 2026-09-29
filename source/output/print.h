/// @file output/print.h

#ifndef PRINT_H_
#define PRINT_H_

#include "match/match.h"
#include "parsing/parse.h"

void print_regex(const rxobj_t rx_obj);
void print_token(const token_t token, const bool do_repr);
void rx_print_matches(const matches_t matches);

#define repr(token) print_token(token, true)

#endif /* !PRINT_H_ */
