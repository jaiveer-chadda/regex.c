/// @file parsing/tokens/tokens.h

#ifndef TOKENS_H_
#define TOKENS_H_

#include "parsing/parse.h"

token_t	rx_tokenise_set(const char **const chr);
token_t	rx_tokenise_group(const char **const chr, const rxobj_t rx_obj);
token_t	rx_tokenise_quant(const char **const chr);
token_t	rx_tokenise_escape(const char **const chr);
token_t	rx_tokenise_literal(const char **const chr);
RxTokens rx_tokenise_sections(const char **const chr, const rxobj_t rx_obj, const char end_chr, const bool is_root);

#endif /* !TOKENS_H_ */
