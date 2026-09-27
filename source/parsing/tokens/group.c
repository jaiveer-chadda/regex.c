/// @file parsing/tokens/group.c

#include <string.h>

#include "parsing/parse.h"
#include "errors/errors.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define GRP_IDEN_COUNT (sizeof(RX_GROUP_IDENS) / sizeof(RX_GROUP_IDENS[0]))

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

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline RxGroup rx_get_group_type(const char **const chr);

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

token_t rx_tokenise_group(const char **const chr) {
	const RxGroup type = rx_get_group_type(chr);
	(void)type;

	// error_invalid_flag();
	// const rxobj_t group_obj = rx_init(NULL, 0);

	RETURN_TOKEN(RXT_GROUP, NA);
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline RxGroup rx_get_group_type(const char **const chr) {
	switch (*(++(*chr))) {
		case '?':
			switch (*(++(*chr))) {
				case ':': return RXG_NON_CAPT; // (?:...)
				case '#': return RXG_COMMENT ; // (?#...)
				case '|': return RXG_SAME_NUM; // (?|...|...)
				case '>': return RXG_ATOMIC	 ; // (?>...)
				case '*': return RXG_NAPLA	 ; // (?*...)
				case '=': return RXG_PLA	 ; // (?=...)
				case '!': return RXG_PLB	 ; // (?!...)
				case'\'': return RXG_NAMED	 ; // (?'name'...)
				case 'P': return RXG_NAMED	 ; // (?P<name>...)
				case '<':
					switch (*(++(*chr))) {
						case '*': return RXG_NAPLB; // (?<*...)
						case '=': return RXG_PLB  ; // (?<=...)
						case '!': return RXG_NLB  ; // (?<!...)
						default	: return RXG_NAMED; // (?<name>...)
					}

				default: return RXG_FLAGS; // (?flags:...)
			}

		case '*': // (*iden:...)
			// note down where the identifier starts
			const char *const iden_start = (*chr) + 1;

			// iterate through the identifier to find its length
			while (*(++(*chr)) != '\0'
				&& **chr != ':'
				&& **chr != ')' // maybe remove later when implementing the `(?X)` groups?
			);

			if (**chr != ':') error_invalid_group_type();

			const size_t iden_len = (size_t)(*chr - iden_start);
			for (size_t i = 0; i < GRP_IDEN_COUNT; i++) {
				const rx__grpiden iden_i = RX_GROUP_IDENS[i];

				if (iden_len == iden_i.len && strncmp(iden_start, iden_i.name, iden_len) == 0) {
					return iden_i.type;
				}
			}

			error_invalid_group_type();

		default: return RXG_REGULAR; // (...)
	}
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

// spell:ignoreRegExp /(?:\b|_)\w?apl\w\b/gi
