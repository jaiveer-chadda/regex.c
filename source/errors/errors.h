/// @file errors/errors.h

#ifndef ERRORS_H_
#define ERRORS_H_

void error_general(void);
void error_invalid_flag(void);
void error_invalid_quant(void);
void error_invalid_range(void);
void error_invalid_escape(void);
void error_impossible_case(void);
void error_not_implemented(void);
void error_unterminated_set(void);
void error_nothing_to_repeat(void);
void error_unterminated_group(void);
void error_invalid_group_type(void);
void error_invalid_group_name(void);

#endif /* !ERRORS_H_ */
