/// @file parsing/tokens/group.c

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "parsing/parse.h"
#include "errors/errors.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define GRP_IDEN_COUNT (sizeof(RX_GROUP_IDENS) / sizeof(RX_GROUP_IDENS[0]))

// note: this struct is only used in this file
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

token_t rx_tokenise_group(const char **const chr, const rxobj_t rx_obj) {
	const RxGroup type = rx_get_group_type(chr);
	RxGroupToken *group = NULL;

	/* ———————————————————————————————————————————————————— */

	switch (type) {
		case RXG_NAMED:
			// depending on which character was used to begin the named group,
			//	work out which one we're expecting to end it
			char end_char;
			switch (*(--(*chr))) {
				case '\'': end_char = '\''; break;
				case '<' : end_char = '>' ; break;
				default: error_impossible_case();
			}

			const char *const name_start = (*chr) + 1;
			while (*(++(*chr)) != '\0' && **chr != end_char);

			if (**chr != end_char) error_invalid_group_name();

			group = calloc(1, sizeof(RxGroupToken));
			group->id = (groupid_t){
				.type = GIDT_STR,
				.id = (any_t)strndup(name_start, *chr - name_start)
			};

			break;

		/* ———————————————————————————————————————————————————— */

		case RXG_FLAGS:
			uint64_t flags = 0;

			// iterate through the flags
			while (*(++(*chr)) != '\0' && **chr != ':') {
				// keep track of whether a flag's been found on this iteration
				bool found = false;

				// now iterate through each of the possible flags
				for (int i = 0; i < RXF_COUNT; i++) {
					// if the current char matches on of the possible flags
					if ((char)RX_FLAGS[i].val == **chr) {
						flags |= RX_FLAGS[i].bf; // firstly, add it to the `flags` variable
						found = true; // then mark down that its been found
						break; // then move on - there can't be any more matches for this char
					}
				}

				// if we've broken out of the loop without finding anything, then this char isn't a valid flag char
				if (!found) error_invalid_flag();
			}

			// make sure that it was the colon that caused us to break out of the loop
			if (**chr != ':') error_invalid_flag();

			group = calloc(1, sizeof(RxGroupToken));

			group->info = (any_t)flags;
			group->id = (groupid_t){
				.type = GIDT_INT,
				// since the count starts at 0, it needs to be incremented first before being assigned
				.id = ++(rx_obj->group_count)
			};

			break;

		default: break;
	}

	/* ———————————————————————————————————————————————————— */

	if (group == NULL) {
		// this is a small adjustment for the non-named/flag groups,
		//	so that the pointer always starts at the character _before_ the group's contents
		(*chr)--;

		group = calloc(1, sizeof(RxGroupToken));
		group->id = (groupid_t){
			.type = GIDT_INT,
			.id = ++(rx_obj->group_count)
		};
	}

	// tokenise the contents of the group and set the group's type
	group->tokens = rx_tokenise_sections(chr, rx_obj, ')');
	group->type = type;

	RETURN_TOKEN(RXT_GROUP, group);
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

		/* ———————————————————————————————————————————————————— */

		case '*': // (*iden:...)
			// note down where the identifier starts
			const char *const iden_start = (*chr) + 1;

			// iterate through the identifier to find its length
			while (*(++(*chr)) != '\0'
				&& **chr != ':'
				&& **chr != ')' // maybe remove later when implementing the `(?X)` groups?
			);

			if (**chr != ':') error_invalid_group_type();

			/* ———————————————————————————————————————————————————— */

			const size_t iden_len = (size_t)(*chr - iden_start);
			for (size_t i = 0; i < GRP_IDEN_COUNT; i++) {
				const rx__grpiden iden_i = RX_GROUP_IDENS[i];

				if (iden_len == iden_i.len && strncmp(iden_start, iden_i.name, iden_len) == 0) {
					return iden_i.type;
				}
			}

			error_invalid_group_type();

		/* ———————————————————————————————————————————————————— */

		default: return RXG_REGULAR; // (...)
	}
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

// spell:ignoreRegExp /(?:\b|_)\w?apl\w\b/gi
