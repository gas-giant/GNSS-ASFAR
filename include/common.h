#ifndef COMMON_H
#define COMMON_H

#include <cstdint>

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;
typedef __int128_t S128;

#define F_EPSILON 1e-7
#define F_EZ(x) (((x) > 0 ? (x) : (-(x))) < F_EPSILON)
#define F_EQ(x, y) (F_EZ((x) - (y)))

#endif // COMMON_H