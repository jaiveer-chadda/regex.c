/// @file parsing/defs.h

#ifndef DEFS_H_
#define DEFS_H_

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#include <inttypes.h>

#include "parse.h"
#include "types/types.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

typedef enum RxTokenType {
	RXT_INVALID	,
	RXT_LITERAL	, // 'a' 'b' 'c'

	RXT_DOT		, // '.'
	RXT_CLASS	, // '\w' '\d' '\N', etc.

	RXT_OR		, // '|'
	RXT_ANCHOR	, // '^' '$'
	RXT_BACKREF	, // '\1' '\k<name>', etc.

	RXT_GROUP	, // '('
	RXT_SET		, // '['
	RXT_RANGE	, // ['a-z']

	RXT_QUANT	, // '+' '*' '?' '{1,2}'
} RxTokenType;

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

typedef struct {
	const char *const name;
	const size_t	  len;
	const RxGroup	  type;
} rx__grpiden;

#define RX_GRPNAME(str, type) { str, sizeof(str) - 1, type }

static const rx__grpiden RX_GROUP_IDENS[] = {
	RX_GRPNAME("atomic"							, RXG_ATOMIC),
	RX_GRPNAME("pla"							, RXG_PLA	),
	RX_GRPNAME("plb"							, RXG_PLB	),
	RX_GRPNAME("nla"							, RXG_NLA	),
	RX_GRPNAME("nlb"							, RXG_NLB	),
	RX_GRPNAME("napla"							, RXG_NAPLA	),
	RX_GRPNAME("naplb"							, RXG_NAPLB	),
	RX_GRPNAME("positive_lookahead"				, RXG_PLA	),
	RX_GRPNAME("positive_lookbehind"			, RXG_PLB	),
	RX_GRPNAME("negative_lookahead"				, RXG_NLA	),
	RX_GRPNAME("negative_lookbehind"			, RXG_NLB	),
	RX_GRPNAME("non_atomic_positive_lookahead"	, RXG_NAPLA	),
	RX_GRPNAME("non_atomic_positive_lookbehind"	, RXG_NAPLB	),
};

#undef RX_GRPNAME

#define GRP_IDEN_COUNT (sizeof(RX_GROUP_IDENS) / sizeof(RX_GROUP_IDENS[0]))

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

/// Represents infinity in ranged quantifiers.
#define INF ((int)-1)
#define NA ((any_t)0)

/* ———————————————————————————————————————————— */

typedef int64_t any_t;

/* ———————————————————————————————————————————— */

/// @brief Generic token type.
typedef struct rx__token {
	RxTokenType type;
	any_t value;
} token_t;

/* ———————————————————————————————————————————— */

typedef struct {
	int lhs, rhs;
	bool has_qm;
} RxGroupToken;

typedef struct {
	int lhs, rhs;
	bool has_qm;
} RxQuantToken;

typedef struct {
	char lhs, rhs;
} RxRangeToken;

typedef struct {
	token_t *tokens;
	size_t token_count;
	bool is_inverse;
} RxSetToken;

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

struct rx__regex {
	uint64_t flags;
	token_t *tokens;
	size_t token_count;
	const char *string;
};

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline rxobj_t	rx_init(const char *const string, const uint64_t flags);
static inline void		rx_tokenise(const rxobj_t rx_obj);
static inline token_t	rx_tokenise_set(const char **const chr);
static inline token_t	rx_tokenise_char(const char **const chr);
static inline token_t	rx_tokenise_group(const char **const chr);
static inline token_t	rx_tokenise_quant(const char **const chr);
static inline token_t	rx_tokenise_escape(const char **const chr);
static inline token_t	rx_tokenise_literal(const char **const chr);

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#endif /* !DEFS_H_ */

// spell:ignoreRegExp /(?:\b|_)\w?apl\w\b/gi
