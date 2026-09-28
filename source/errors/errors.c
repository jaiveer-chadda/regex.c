/// @file errors/errors.c

#include <stdio.h>
#include <stdlib.h>

#define BASIC_ERR(code) puts(__func__); exit(code);

void error_general				(void) { BASIC_ERR(1); }
void error_invalid_flag			(void) { BASIC_ERR(1); }
void error_invalid_quant		(void) { BASIC_ERR(1); }
void error_invalid_range		(void) { BASIC_ERR(1); }
void error_invalid_escape		(void) { BASIC_ERR(1); }
void error_impossible_case		(void) { BASIC_ERR(2); }
void error_not_implemented		(void) { BASIC_ERR(3); }
void error_unterminated_set		(void) { BASIC_ERR(1); }
void error_unterminated_group	(void) { BASIC_ERR(1); }
void error_invalid_group_type	(void) { BASIC_ERR(1); }
void error_invalid_group_name	(void) { BASIC_ERR(1); }
