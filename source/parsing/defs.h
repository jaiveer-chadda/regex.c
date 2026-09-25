/// @file parsing/defs.h

#ifndef DEFS_H_
#define DEFS_H_

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#include <inttypes.h>

#include "parse.h"
#include "types/types.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

typedef enum RxTokenType {
	RXT_LITERAL	, // 'a' 'b' 'c'

	RXT_DOT		, // '.'
	RXT_ESCAPE	, // '\X'

	RXT_OR		, // '|'
	RXT_ASSERT	, // '^' '$'

	RXT_GROUP	, // '('
	RXT_SET		, // '['

	RXT_QUANT	, // '+' '*' '?' '{1,2}'
} RxTokenType;

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define NA ((any_t)0)
typedef int64_t any_t;

typedef struct rx__token {
	RxTokenType type;
	any_t value;
} token_t;

struct rx__regex {
	const char *string;
	uint64_t flags;
	token_t *tokens;
	size_t token_count;
};

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline rxobj_t	rx_init(const char *const string, const uint64_t flags);
static inline void		rx_tokenise(const rxobj_t rx_obj);
static inline token_t	rx_tokenise_char(const char **const chr);
static inline token_t	rx_tokenise_quant(const char **const chr);

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#endif /* !DEFS_H_ */
