/// @file types/types.c

#define RX__TYPE_IMPLEMENTATION
#include "types.h"

#define X(name, chr) [RXF_##name] = { .bf = (1ULL << RXF_##name), .val = chr },
const bfmap_t RX_FLAGS[RXF_COUNT] = { RX__FLAG_TABLE };
#undef X
#define X(name, chr) [RXC_##name] = { .bf = (1ULL << RXC_##name), .val = chr },
const bfmap_t RX_CHARS[RXC_COUNT] = { RX__CHAR_TABLE };
#undef X
#define X(name, chr) [RXE_##name] = { .bf = (1ULL << RXE_##name), .val = chr },
const bfmap_t RX_ESCS [RXE_COUNT] = { RX__ESCS_TABLE };
#undef X
