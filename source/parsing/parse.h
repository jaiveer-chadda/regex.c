/// @file parsing/parse.h

#ifndef PARSE_H_
#define PARSE_H_

#include <stddef.h>
#include <inttypes.h>

#include "types/types.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

/// Approximately multiply a number by 1.5, in place.
#define MULT_BY_1_5(var) ((var) += (var) <= 1 ? 1 : (var) >> 1)
#define CHR_TO_INT(char_) ((char_) - '0')

/// Represents infinity in ranged quantifiers.
#define INF ((int)-1)
#define NA ((any_t)0)

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

typedef enum rx__tokentype {
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

typedef int64_t any_t;

typedef enum	{ GIDT_INT, GIDT_STR,			} RxGroupIDType;
typedef struct	{ any_t id; RxGroupIDType type;	} groupid_t;

/* ———————————————————————————————————————————— */

/// @brief Generic token type.
typedef struct { RxTokenType type; any_t value; } token_t;

/* ———————————————————————————————————————————— */

#define RETURN_TOKEN(type, value) return (token_t){ (RxTokenType)type, (any_t)value }

/* ———————————————————————————————————————————— */

typedef struct { char lhs, rhs;											} RxRangeToken;
typedef struct { int lhs, rhs; bool has_qm;								} RxQuantToken;
typedef struct { token_t *tokens; size_t token_count; bool is_inverse;	} RxSetToken;

typedef struct {
	groupid_t id;
	token_t *tokens;
	size_t token_count;

	RxGroup type;
	any_t info;
} RxGroupToken;

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

struct rx__regex {
	uint64_t flags;
	const char *string;

	token_t *tokens;
	size_t token_count;

	uint16_t group_count;
};

typedef struct rx__regex *rxobj_t;

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

rxobj_t rx_compile(const char *const string, const uint64_t flags);
token_t rx_tokenise_char(const char **const chr, const rxobj_t rx_obj);

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#endif /* !PARSE_H_ */
