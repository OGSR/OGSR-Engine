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

// Couple more tricks 
// Counting number of nonzero bits for 8bit number: 
IC	u8		btwCount1(u8 v)
{
	v = (v & 0x55) + ((v >> 1) & 0x55);
	v = (v & 0x33) + ((v >> 2) & 0x33);
	return (v & 0x0f) + ((v >> 4) & 0x0f);
}

//same for 32bit 
IC	u32	btwCount1(u32 v)
{
	const u32 g31 = 0x49249249ul;	// = 0100_1001_0010_0100_1001_0010_0100_1001
	const u32 g32 = 0x381c0e07ul;	// = 0011_1000_0001_1100_0000_1110_0000_0111
	v = (v & g31) + ((v >> 1) & g31) + ((v >> 2) & g31);
	v = ((v + (v >> 3)) & g32) + ((v >> 6) & g32);
	return (v + (v >> 9) + (v >> 18) + (v >> 27)) & 0x3f;
}

IC	u64	btwCount1(u64 v)
{
	return btwCount1(u32(v&u32(-1))) + btwCount1(u32(v >> u64(32)));
}
