/// @file output/regex.c

#include <stdio.h>
#include "print.h"

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#define rx_print_tokens(tokens) rx__print_tokens(tokens, stdout)
#define	  fprint_tokens(tokens) rx__print_tokens(tokens, file)

#define MAX_TOKEN_NAMELEN (int)(sizeof("RXT_LITERAL") - 1)

#define putstr(str) fputs(str, file)
#define fprint(...) fprintf(file, __VA_ARGS__)
#define	  putf(chr) putc(chr, file)

#define CASE(type)									\
	case (type):									\
		do {										\
			if (do_repr) {							\
				fprint("\33[96m%-*s\33[;2m, \33[m",	\
					MAX_TOKEN_NAMELEN, #type		\
				);									\
			}										\
		} while (0)

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline void rx__print_tokens(const RxTokens tokens, FILE *const file);

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

void print_regex(const rxobj_t rx_obj) {
	rx__print_tokens(rx_obj->tokens, stdout);
	putchar('\n');
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

static inline void rx__print_tokens(const RxTokens tokens, FILE *const file) {
	for (size_t i = 0; i < tokens.len; i++) rx__print_token(tokens.arr[i], false, file);
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

void rx__print_token(const token_t token, const bool do_repr, FILE *const file) {
	if (do_repr) putstr("\33[91m(\33[93mtoken_t\33[91m){ ");

	/* ———————————————————————————————————————————————————————————————————— */

	switch (token.type) {
		CASE(RXT_LITERAL); {
			fprint("\33[1;38;5;147m%c\33[m", (char)token.value);
			break;
		}

		/* ———————————————————————————————————————————————————————————————————— */

		CASE(RXT_CLASS); {
			fprint("\33[34m%c%c\33[m",
				((char)token.value == '.') ? '\0' : '\\',
				((char)token.value)
			);
			break;
		}

		/* ———————————————————————————————————————————————————————————————————— */

		CASE(RXT_OR); {
			const RxOrToken *const or = (RxOrToken*)token.value;
			for (size_t j = 0; j < or->count; j++) {
				if (j != 0) putstr("\33[32m|\33[m");
				fprint_tokens(or->sections[j]);
			}
			break;
		}

		/* ———————————————————————————————————————————————————————————————————— */

		CASE(RXT_ANCHOR); {
			fprint("\33[96m%c%c\33[m",
				((char)token.value == '^' || (char)token.value == '$') ? '\0' : '\\',
				((char)token.value)
			);
			break;
		}

		/* ———————————————————————————————————————————————————————————————————— */

		CASE(RXT_BACKREF); {
			fprint("\33[32m\\%d\33[m", (int)token.value);
			break;
		}

		/* ———————————————————————————————————————————————————————————————————— */

		CASE(RXT_GROUP); {
			putstr("\33[31m(");
			const RxGroupToken *const group = (RxGroupToken*)token.value;

			switch (group->type) {
				case RXG_NON_CAPT: putstr("?:"	); break;
				case RXG_COMMENT	: putstr("?#"	); break;
				case RXG_SAME_NUM	: putstr("?|"	); break;
				case RXG_ATOMIC		: putstr("?>"	); break;
				case RXG_NAPLA		: putstr("?*"	); break;
				case RXG_NAPLB		: putstr("?<*"	); break;
				case RXG_PLA		: putstr("?="	); break;
				case RXG_NLA		: putstr("?!"	); break;
				case RXG_PLB		: putstr("?<="	); break;
				case RXG_NLB		: putstr("?<!"	); break;
				case RXG_FLAGS		: putstr("?...:"); break;
				case RXG_REGULAR	: break;

				case RXG_NAMED: {
					groupid_t id = group->id;

					if (id.type == GIDT_STR) fprint("?<%s>", (char*)id.id);
					break;
				}
			}

			putstr("\33[m");
			fprint_tokens(group->tokens);

			putstr("\33[31m)\33[m");

			break;
		}

		/* ———————————————————————————————————————————————————————————————————— */

		CASE(RXT_SET); {
			const RxSetToken *const set = (RxSetToken*)token.value;
			fprint("\33[33m[%s\33[m", set->is_inverse ? "^" : "");
			fprint_tokens(set->tokens);
			putstr("\33[33m]\33[m");
			break;
		}

		/* ———————————————————————————————————————————————————————————————————— */

		CASE(RXT_RANGE); {
			const RxRangeToken *const range = (RxRangeToken*)token.value;
			fprint("\33[92m%c\33[;2m-\33[;92m%c\33[m", range->lhs, range->rhs);
			break;
		}

		/* ———————————————————————————————————————————————————————————————————— */

		CASE(RXT_QUANT); {
			const RxQuantToken *const quant = (RxQuantToken*)token.value;
			rx__print_token(quant->repeat, false, file);

			putstr("\33[95m");

			if		(quant->lhs == 0 && quant->rhs == 1	 ) putf('?');
			else if	(quant->lhs == 0 && quant->rhs == INF) putf('*');
			else if	(quant->lhs == 1 && quant->rhs == INF) putf('+');
			else if	(quant->lhs ==		quant->rhs		 ) fprint("{%zu}", quant->lhs);
			else {
				fprint("{%zu,", quant->lhs);
				if (quant->rhs != INF) fprint("%zu", quant->rhs);
				putf('}');
			}

			if (quant->has_qm) putf('?');
			putstr("\33[m");

			break;
		}

		/* ———————————————————————————————————————————————————————————————————— */

		CASE(RXT_EMPTY); {
			putstr("\33[2m∅\33[m");
			break;
		}

		/* ———————————————————————————————————————————————————————————————————— */

		CASE(RXT_INVALID); { fprint("\33[31minvalid case\33[m, .val = %lld", token.value); break; }
		default			 : { fprint("\33[31munknown case\33[m, .val = %lld", token.value); break; }
	}

	/* ———————————————————————————————————————————————————————————————————— */

	if (do_repr) putstr(" \33[91m}\33[;2m;\33[m\n");
}

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */
