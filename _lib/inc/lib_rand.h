
// ****************************************************************************
//
//                          Random number generator
//
// ****************************************************************************
// PicoLibSDK - Alternative SDK library for Raspberry Pico and RP2040
// Copyright (c) 2023 Miroslav Nemecek, Panda38@seznam.cz, hardyplotter2@gmail.com
// 	https://github.com/Panda381/PicoLibSDK
//	https://www.breatharian.eu/hw/picolibsdk/index_en.html
//	https://github.com/pajenicko/picopad
//	https://picopad.eu/en/
// License:
//	This source code is freely available for any purpose, including commercial.
//	It is possible to take and modify the code or parts of it, without restriction.

#if USE_RAND		// 1=use random number generator

#ifndef _LIB_RAND_H
#define _LIB_RAND_H

#ifdef __cplusplus
extern "C" {
#endif

// Seed of random number generator
extern u32 RandSeed;

// get seed of random number generator
INLINE u32 RandGet() { return RandSeed; }

// set seed of random number generator
INLINE void RandSet(u32 seed) { RandSeed = seed; }

// shift random number generator and return 16-bit random number
u16 RandShift();

// generate 8-bit unsigned integer random number
INLINE u8 RandU8() { return RandShift() >> 8; }

// generate 16-bit unsigned integer random number
INLINE u16 RandU16() { return RandShift(); }
INLINE int rand() { return (int)(RandShift() >> 1); }

#ifndef RAND_MAX
#define RAND_MAX 0x7fff
#endif

// generate 32-bit unsigned integer random number
u32 RandU32();

// generate 64-bit unsigned integer random number
u64 RandU64();

// generate 8-bit signed integer random number
INLINE s8 RandS8() { return (s8)RandU8(); }

// generate 16-bit signed integer random number
INLINE s16 RandS16() { return (s16)RandU16(); }

// generate 32-bit signed integer random number
INLINE s32 RandS32() { return (s32)RandU32(); }

// generate 64-bit signed integer random number
INLINE s64 RandS64() { return (s64)RandU64(); }

// generate 8-bit unsigned integer random number in range 0 to MAX (including)
u8 RandU8Max(u8 max);

// generate 16-bit unsigned integer random number in range 0 to MAX (including)
u16 RandU16Max(u16 max);

// generate 32-bit unsigned integer random number in range 0 to MAX (including)
u32 RandU32Max(u32 max);

// generate 64-bit unsigned integer random number in range 0 to MAX (including)
u64 RandU64Max(u64 max);

// generate 8-bit signed integer random number in range 0 to MAX (including, 'max' can be negative)
s8 RandS8Max(s8 max);

// generate 16-bit signed integer random number in range 0 to MAX (including, 'max' can be negative)
s16 RandS16Max(s16 max);

// generate 32-bit signed integer random number in range 0 to MAX (including, 'max' can be negative)
s32 RandS32Max(s32 max);

// generate 64-bit signed integer random number in range 0 to MAX (including, 'max' can be negative)
s64 RandS64Max(s64 max);

// generate 8-bit unsigned integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval.
u8 RandU8MinMax(u8 min, u8 max);

// generate 16-bit unsigned integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval.
u16 RandU16MinMax(u16 min, u16 max);

// generate 32-bit unsigned integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval.
u32 RandU32MinMax(u32 min, u32 max);

// generate 64-bit unsigned integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval.
u64 RandU64MinMax(u64 min, u64 max);

// generate 8-bit signed integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval.
s8 RandS8MinMax(s8 min, s8 max);

// generate 16-bit signed integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval.
s16 RandS16MinMax(s16 min, s16 max);

// generate 32-bit signed integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval.
s32 RandS32MinMax(s32 min, s32 max);

// generate 64-bit signed integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval.
s64 RandS64MinMax(s64 min, s64 max);

#ifdef __cplusplus
}
#endif

#endif // _LIB_RAND_H

#endif // USE_RAND
