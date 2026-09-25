/// @file errors/errors.c

#include <stdio.h>
#include <stdlib.h>

#define BASIC_ERR(code) puts(__func__); exit(code);

void error_invalid_quant	(void) { BASIC_ERR(1); }
void error_impossible_case	(void) { BASIC_ERR(2); }
