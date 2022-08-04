#pragma once

inline bool negative(const float f) { return std::signbit(f); }
inline bool positive(const float f) { return !std::signbit(f); }
inline void set_negative(float& f) { f = -fabsf(f); }
inline void set_positive(float& f) { f = fabsf(f); }

/*
 * Here are a few nice tricks for 2's complement based machines
 * that I discovered a few months ago.
 */
IC int btwLowestBitMask(int v) { return v & -v; }
IC u32 btwLowestBitMask(u32 x) { return x & ~(x - 1); }

/* Ok, so now we are cooking on gass. Here we use this function for some */
/* rather useful utility functions */
IC bool btwIsPow2(int v) { return btwLowestBitMask(v) == v; }
IC bool btwIsPow2(u32 v) { return btwLowestBitMask(v) == v; }

IC int btwPow2_Ceil(int v)
{
    int i = btwLowestBitMask(v);
    while (i < v)
        i <<= 1;
    return i;
}
IC u32 btwPow2_Ceil(u32 v)
{
    u32 i = btwLowestBitMask(v);
    while (i < v)
        i <<= 1;
    return i;
}

#define iFloor(x) int(std::floor(x))
#define iCeil(x) int(std::ceil(x))
