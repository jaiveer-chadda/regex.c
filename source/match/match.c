/// @file match/match.c

#include "match.h"

match_t rx_match(rxobj_t rx_obj, const char *const string) {
	(void)rx_obj, (void)string;
	return (match_t){0};
}
