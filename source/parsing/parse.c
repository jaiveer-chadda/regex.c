/// @file parsing/parse.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "defs.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

/// Allocate `size` bytes of memory, and copy that many bytes from `src`.
#define memdup(src, size) memcpy(malloc((size)), (src), (size))

/// Approximately multiply a number by 1.5, in place.
#define MULT_BY_1_5(var) ((var) += (var) <= 1 ? 1 : (var) >> 1)

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

rxobj_t rx_compile(const char *const string, const uint64_t flags) {
	const rxobj_t rx_obj = rx_init(string, flags);
	rx_tokenise(rx_obj);

	return rx_obj;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline rxobj_t rx_init(const char *const string, const uint64_t flags) {
	const rxobj_t rx_obj = calloc(1, sizeof(struct rx__regex));
	const size_t str_len = strlen(string) + 1;

	*rx_obj = (struct rx__regex){
		.token_count = 0,
		.tokens = NULL,
		.string = memdup(string, str_len),
		.flags  = flags,
	};

	return rx_obj;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define CHR_TO_INT(char_) ((char_) - '0')

#define PACK_INTS(i1, i2)  ((((uint64_t)(uint32_t)(i1)) << (1 << (sizeof(int32_t) + 1))) | (uint32_t)(i2))
#define UNPACK_INTS(l)			(int32_t)((uint64_t)(l) >> (1 << (sizeof(int32_t) + 1))),	(int32_t)(l)

static inline void rx_tokenise(const rxobj_t rx_obj) {
	token_t token;
	size_t alloc_count = 0;

	for (const char *chr = rx_obj->string; *chr != '\0'; chr++) {
		switch (*chr) {
			case '?': case '*': case '+': [[fallthrough]];
			case '{': token = rx_tokenise_quant(&chr); break;

			case '|': token = (token_t){ RXT_OR		, NA		}; break;
			case '.': token = (token_t){ RXT_DOT	, NA		}; break;
			case '^': token = (token_t){ RXT_ASSERT	, *chr		}; break;
			case '$': token = (token_t){ RXT_ASSERT	, *chr		}; break;
			case'\\': token = (token_t){ RXT_ESCAPE	, *(++chr)	}; break;

			case '(': token = (token_t){ RXT_GROUP	, NA		}; break;
			case '[': token = (token_t){ RXT_SET	, NA		}; break;

			default	: token = (token_t){ RXT_LITERAL, *chr		}; break;
		}

		// reallocate new memory as its needed
		//	on the first iteration, `rx_obj->tokens` will be `NULL`, but `reallocf` will allocate new memory for it
		if (rx_obj->token_count + 1 > alloc_count) {
			rx_obj->tokens = reallocf(rx_obj->tokens, MULT_BY_1_5(alloc_count) * sizeof(token_t));
		}

		rx_obj->tokens[rx_obj->token_count++] = token;
	}
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline token_t rx_tokenise_quant(const char **const chr) {
	int size[2] = { 0, 0 };

	switch (**chr) {
		case '?': size[0] = 0, size[1] =  1; break;
		case '*': size[0] = 0, size[1] = -1; break;
		case '+': size[0] = 1, size[1] = -1; break;

		case '{': 
			// ( in `{a,b}`, assume for now that `a` and `b` are both in the range `[0,9]` )
			// parse each of the integers, ignoring the comma and closing brace
			size[0] = CHR_TO_INT(*(++(*chr)));
			assert(*(++(*chr)) == ',');

			size[1] = CHR_TO_INT(*(++(*chr)));
			assert(*(++(*chr)) == '}');

			break;

		default:
			assert(false);
			exit(EXIT_FAILURE);
	}

	return (token_t){ RXT_QUANT, PACK_INTS(size[0], size[1]) };
}


/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
