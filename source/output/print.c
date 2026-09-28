/// @file output/print.c

#include <stdio.h>
#include "print.h"

static inline void rx_print_tokens(const token_t *const tokens, const size_t count);

void print_regex(const rxobj_t rx_obj) {
	rx_print_tokens(rx_obj->tokens, rx_obj->token_count);
	putchar('\n');
}

static inline void rx_print_tokens(const token_t *const tokens, const size_t count) {
	for (size_t i = 0; i < count; i++) {
		const token_t token = tokens[i];

		switch (token.type) {
			case RXT_LITERAL: putchar(token.value); break;

			case RXT_CLASS:
				printf("\33[34m%c%c\33[m",
					((char)token.value == '.') ? '\0' : '\\',
					((char)token.value)
				);
				break;

			case RXT_OR:
				const RxOrToken *const or = (RxOrToken*)token.value;
				for (size_t j = 0; j < or->count; j++) {
					if (j != 0) fputs("\33[32m|\33[m", stdout);
					rx_print_tokens(or->sections[j].tokens, or->sections[j].token_count);
				}
				break;

			case RXT_ANCHOR:
				printf("\33[96m%c%c\33[m",
					((char)token.value == '^' || (char)token.value == '$') ? '\0' : '\\',
					((char)token.value)
				);
				break;

			case RXT_BACKREF: printf("\33[32m\\%d\33[m", (int)token.value); break;

			case RXT_GROUP:
				fputs("\33[31m(", stdout);
				const RxGroupToken *const group = (RxGroupToken*)token.value;

				switch (group->type) {
					case RXG_REGULAR	: break;
					case RXG_NON_CAPT	: fputs("?:"		, stdout); break;
					case RXG_COMMENT	: fputs("?#"		, stdout); break;
					case RXG_SAME_NUM	: fputs("?|"		, stdout); break;
					case RXG_ATOMIC		: fputs("?>"		, stdout); break;
					case RXG_NAPLA		: fputs("?*"		, stdout); break;
					case RXG_NAPLB		: fputs("?<*"		, stdout); break;
					case RXG_PLA		: fputs("?="		, stdout); break;
					case RXG_NLA		: fputs("?!"		, stdout); break;
					case RXG_PLB		: fputs("?<="		, stdout); break;
					case RXG_NLB		: fputs("?<!"		, stdout); break;
					case RXG_FLAGS		: fputs("?flags:"	, stdout); break;

					case RXG_NAMED		:
						groupid_t id = group->id;
						if (id.type == GIDT_STR) printf("?<%s>",(char*)group->id.id);
						break;
				}

				fputs("\33[m", stdout);
				rx_print_tokens(group->tokens, group->token_count);
				fputs("\33[31m)\33[m", stdout);
				break;

			case RXT_SET:
				fputs("\33[33m[\33[m", stdout);
				const RxSetToken *const set = (RxSetToken*)token.value;
				rx_print_tokens(set->tokens, set->token_count);
				fputs("\33[33m]\33[m", stdout);
				break;

			case RXT_RANGE:
				const RxRangeToken *const range = (RxRangeToken*)token.value;
				printf("\33[92m%c\33[;2m-\33[;92m%c\33[m", range->lhs, range->rhs);
				break;

			case RXT_QUANT:
				const RxQuantToken *const quant = (RxQuantToken*)token.value;
				fputs("\33[95m", stdout);

				if		(quant->lhs == 0 && quant->rhs == 1	 ) putchar('?');
				else if	(quant->lhs == 0 && quant->rhs == INF) putchar('*');
				else if	(quant->lhs == 1 && quant->rhs == INF) putchar('+');
				else {
					printf("{%d,", quant->lhs);
					if (quant->rhs != INF) printf("%d", quant->rhs);
					putchar('}');
				}

				if (quant->has_qm) putchar('?');
				fputs("\33[m", stdout);
				break;

			case RXT_INVALID: break;
			default: break;
		}
	}
}
