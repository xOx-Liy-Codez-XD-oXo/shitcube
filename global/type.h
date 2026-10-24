#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>

#ifndef s8
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uintptr_t uip;
#endif

#define SYSTEM_GAMECUBE 0
#define SYSTEM_WII 1

#endif
