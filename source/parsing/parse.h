/// @file parsing/parse.h

#ifndef PARSE_H_
#define PARSE_H_

#include <inttypes.h>

typedef struct rx__regex *regex_t;

regex_t rx_compile(const char *const string, const uint64_t flags);

#endif /* !PARSE_H_ */
