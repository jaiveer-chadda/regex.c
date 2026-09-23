/// @file parsing/defs.h

#ifndef DEFS_H_
#define DEFS_H_

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#include <inttypes.h>

#include "parse.h"
#include "types/types.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline regex_t rx_init(const char *const string, const uint64_t flags);
static inline void rx_tokenise(const regex_t rx_obj);

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

typedef enum RxTokenType {
	RXS_LITERAL
} RxTokenType;

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

typedef struct rx__token {
	RxTokenType type;
	void *value;
} token_t;

struct rx__regex {
	const char *string;
	uint64_t flags;
	token_t *tokens;
};

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#endif /* !DEFS_H_ */
