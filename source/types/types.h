/// @file types/types.h

#ifndef TYPES_H_
#define TYPES_H_

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#include <inttypes.h>

#include "_flags.h"
#include "_chars.h"
#include "_escapes.h"

#ifndef RX__TYPE_IMPLEMENTATION
#	undef RX__FLAG_TABLE
#	undef RX__CHAR_TABLE
#	undef RX__ESCS_TABLE
#endif

/* —————————————————————————————————————————————————————— */

typedef struct bfmap_t__ { uint64_t bf; uint32_t val; } bfmap_t;
const bfmap_t RX_FLAGS[RXF_COUNT], RX_CHARS[RXC_COUNT], RX_ESCS[RXE_COUNT];

/* —————————————————————————————————————————————————————— */

/** An enum holding all the flags that could be applied to a regex search. */
typedef enum RxFlag	  RxFlag  ;
/** An enum holding every semantically-significant regex character. */
typedef enum RxChar	  RxChar  ;
/** An enum holding every character that can appear after a backslash. */
typedef enum RxEscape RxEscape;

/* ————————————————————————————————————————————————————————————————————————————————————————————————————————————————— */

#endif /* !TYPES_H_ */
