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

#define REALLOC_FOR(arr, elem_count, alloc_count, type) do { \
	if ((elem_count) + 1 > (alloc_count)) { \
		(arr) = reallocf((arr), MULT_BY_1_5((alloc_count)) * sizeof(type)); \
	} \
} while (0)

/// Represents infinity in ranged quantifiers.
#define INF ((size_t)UINT64_MAX)
#define NA ((any_t)0)

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

typedef enum rx__tokentype {
	RXT_INVALID	,
	RXT_EMPTY	, // ∅

	RXT_LITERAL	, // char			:  'a' 'b' 'c'
	RXT_CLASS	, // char			:  '.' '\w' '\d' '\N', etc.
	RXT_ANCHOR	, // char			:  '^' '$' '\b', etc.

	RXT_BACKREF	, // groupid_t	  *	:  '\1' '\k<name>', etc.

	RXT_OR		, // RxOrToken	  *	:  '|' /// Can only be the base token, or the exclusive subtoken of `RXT_GROUP`.
	RXT_GROUP	, // RxGroupToken *	:  '('
	RXT_QUANT	, // RxQuantToken *	:  '+' '*' '?' '{1,2}'

	RXT_SET		, // RxSetToken	  *	:  '['
	RXT_RANGE	, // RxRangeToken *	:  ['a-z'] /// Can only be a subtoken of `RXT_SET`.

} RxTokenType;

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

typedef int64_t any_t;

typedef enum	{ GIDT_INT, GIDT_STR,			} RxGroupIDType;
typedef struct	{ any_t id; RxGroupIDType type;	} groupid_t;

/* ———————————————————————————————————————————— */

/// @brief Generic token type.
typedef struct { RxTokenType type; any_t value; } token_t;

/// @brief Struct holding an array of tokens.
typedef struct { token_t *arr; size_t len; } RxTokens;

/* ———————————————————————————————————————————— */

#define RETURN_TOKEN(type, value) return (token_t){ (RxTokenType)type, (any_t)value }

/* ———————————————————————————————————————————— */

typedef struct { char lhs, rhs;						} RxRangeToken;
typedef struct { RxTokens tokens; bool is_inverse;	} RxSetToken;
typedef struct { RxTokens *sections; size_t count;	} RxOrToken;

typedef struct {
	token_t repeat;
	size_t lhs, rhs;
	bool has_qm;
} RxQuantToken;

typedef struct {
	groupid_t id;
	RxTokens tokens;

	RxGroup type;
	any_t info;
} RxGroupToken;

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

struct rx__regex {
	uint64_t flags;
	const char *string;

	RxTokens tokens;
	uint16_t group_count;
};

typedef struct rx__regex *rxobj_t;

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

rxobj_t rx_compile(const char *const string, const uint64_t flags);
token_t rx_tokenise_char(const char **const chr, const rxobj_t rx_obj);

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#endif /* !PARSE_H_ */
