/// @file parsing/parse.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "defs.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define memdup(src, size) memcpy(malloc((size)), (src), (size))

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
		.tokens = malloc(str_len),
		.string = memdup(string, str_len),
		.flags  = flags,
	};

	return rx_obj;
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define SET_SIZE(min, max) (size[0] = (min), size[1] = (max))
#define CHR_TO_INT(char_) ((char_) - '0')

#define PACK_INTS(i1, i2)  ((((uint64_t)(uint32_t)(i1)) << (1 << (sizeof(int32_t) + 1))) | (uint32_t)(i2))
#define UNPACK_INTS(l)			(int32_t)((uint64_t)(l) >> (1 << (sizeof(int32_t) + 1))),	(int32_t)(l)

#define GET_QUANT_TOKEN() ((token_t){ RXT_QUANT, PACK_INTS(size[0], size[1]) })

static inline void rx_tokenise(const rxobj_t rx_obj) {
	token_t token;

	for (const char *chr = rx_obj->string; *chr != '\0'; chr++) {
		int size[2] = {0};

		switch (*chr) {
			case '|': token = (token_t){ RXT_OR		, NA	 } ; break;
			case '.': token = (token_t){ RXT_DOT	, NA	 } ; break;
			case'\\': token = (token_t){ RXT_ESCAPE	, *++chr } ; break;
			case '^': token = (token_t){ RXT_ASSERT	, *chr	 } ; break;
			case '$': token = (token_t){ RXT_ASSERT	, *chr	 } ; break;

			case '(': token = (token_t){ RXT_GROUP	, NA	 } ; break;
			case '[': token = (token_t){ RXT_SET	, NA	 } ; break;

			case '?': SET_SIZE(0, 1); token = GET_QUANT_TOKEN(); break;
			case '*': SET_SIZE(0,-1); token = GET_QUANT_TOKEN(); break;
			case '+': SET_SIZE(1,-1); token = GET_QUANT_TOKEN(); break;
			case '{':
				// ( in `{a,b}`, assume for now that `a` and `b` are both in the range `[0,9]` )
				// parse each of the integers, ignoring the comma and closing brace
				size[0] = CHR_TO_INT(*(++chr)); assert(*(++chr) == ',');
				size[1] = CHR_TO_INT(*(++chr)); assert(*(++chr) == '}');

				token = GET_QUANT_TOKEN();
				break;

			default: token = (token_t){ RXT_LITERAL, *chr };
		}

		(void)token;
	}
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
