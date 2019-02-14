#pragma once

// integer math on floats
#ifdef _M_X64
IC bool negative(const float f) { return f < 0; }
IC bool positive(const float f) { return f >= 0; }
IC void set_negative(float& f) { f = -fabsf(f); }
IC void set_positive(float& f) { f = fabsf(f); }
#else
static constexpr auto fdSGN = 0x080000000; // mask for sign bit
IC bool negative(const float& f) { return (*((unsigned*)(&f))&fdSGN) != 0; }
IC bool positive(const float& f) { return (*((unsigned*)(&f))&fdSGN) == 0; }
IC void set_negative(float& f) { (*(unsigned*)(&f)) |= fdSGN; }
IC void set_positive(float& f) { (*(unsigned*)(&f)) &= ~fdSGN; }
#endif

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
#define iCeil(x)  int(std::ceil(x))
