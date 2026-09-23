/// @file types/types.h

#ifndef TYPES_H_
#define TYPES_H_

#include "_flags.h"
#include "_chars.h"
#include "_tokens.h"
#include "_escapes.h"

/** An enum holding all the flags that could be applied to a regex search. */
typedef enum RxFlag	  RxFlag  ;
/** An enum holding every semantically-significant regex character. */
typedef enum RxChar	  RxChar  ;
/** An enum holding every regex token to match against. */
typedef enum RxToken  RxToken ;
/** An enum holding every character that can appear after a backslash. */
typedef enum RxEscape RxEscape;

#endif /* !TYPES_H_ */
