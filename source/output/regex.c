/// @file output/regex.c

#include <stdio.h>
#include "print.h"

#define putstr(string) fputs((string), stdout)
#define CASE(type) case(type): do { if (do_repr) printf(("\33[96m%-*s\33[;2m, \33[m"), 11, #type); } while (0)

static inline void rx_print_tokens(const RxTokens tokens);

void print_regex(const rxobj_t rx_obj) {
	rx_print_tokens(rx_obj->tokens);
	putchar('\n');
}

static inline void rx_print_tokens(const RxTokens tokens) {
	for (size_t i = 0; i < tokens.len; i++) print_token(tokens.arr[i], false);
}

void print_token(const token_t token, const bool do_repr) {
	if (do_repr) putstr("\33[91m(\33[93mtoken_t\33[91m){ ");

	switch (token.type) {
		CASE(RXT_LITERAL); printf("\33[1;38;5;147m%c\33[m", (char)token.value); break;

		CASE(RXT_CLASS);
			printf("\33[34m%c%c\33[m",
				((char)token.value == '.') ? '\0' : '\\',
				((char)token.value)
			);
			break;

		CASE(RXT_OR);
			const RxOrToken *const or = (RxOrToken*)token.value;
			for (size_t j = 0; j < or->count; j++) {
				if (j != 0) putstr("\33[32m|\33[m");
				rx_print_tokens(or->sections[j]);
			}
			break;

		CASE(RXT_ANCHOR);
			printf("\33[96m%c%c\33[m",
				((char)token.value == '^' || (char)token.value == '$') ? '\0' : '\\',
				((char)token.value)
			);
			break;

		CASE(RXT_BACKREF); printf("\33[32m\\%d\33[m", (int)token.value); break;

		CASE(RXT_GROUP);
			putstr("\33[31m(");
			const RxGroupToken *const group = (RxGroupToken*)token.value;

			switch (group->type) {
				case RXG_NON_CAPT	: putstr("?:"	); break;
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

				case RXG_NAMED		:
					groupid_t id = group->id;
					if (id.type == GIDT_STR) printf("?<%s>",(char*)group->id.id);
					break;
			}

			putstr("\33[m");
			rx_print_tokens(group->tokens);
			putstr("\33[31m)\33[m");
			break;

		CASE(RXT_SET);
			putstr("\33[33m[\33[m");
			const RxSetToken *const set = (RxSetToken*)token.value;
			rx_print_tokens(set->tokens);
			putstr("\33[33m]\33[m");
			break;

		CASE(RXT_RANGE);
			const RxRangeToken *const range = (RxRangeToken*)token.value;
			printf("\33[92m%c\33[;2m-\33[;92m%c\33[m", range->lhs, range->rhs);
			break;

		CASE(RXT_QUANT);
			const RxQuantToken *const quant = (RxQuantToken*)token.value;
			print_token(quant->repeat, false);

			putstr("\33[95m");

			if		(quant->lhs == 0 && quant->rhs == 1	 ) putchar('?');
			else if	(quant->lhs == 0 && quant->rhs == INF) putchar('*');
			else if	(quant->lhs == 1 && quant->rhs == INF) putchar('+');
			else {
				printf("{%d,", quant->lhs);
				if (quant->rhs != INF) printf("%d", quant->rhs);
				putchar('}');
			}

			if (quant->has_qm) putchar('?');
			putstr("\33[m");
			break;

		CASE(RXT_INVALID); break;
		default: break;
	}

	if (do_repr) puts(" \33[91m}\33[;2m;\33[m");
}
