#pragma once

// integer math on floats
#ifdef _M_AMD64
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


#include <immintrin.h>
// Может когда-нибудь я сделаю сборку с включенными AVX инструкциями. А юзать их вместе с включенным SSE - накладно из-за переключения между регистрами.
// Можно конечно обнулять регистры в начале и конце каждой функции, но это на мой взгляд не лучшее решение.
// см. http://qaru.site/questions/4585/using-avx-cpu-instructions-poor-performance-without-archavx/39253#39253
#ifdef __AVX__
//--------------------------------------------------------------------
ICF int iFloor(float x) {
	__m256 float_cast = _mm256_broadcast_ss(&x);
	__m256 floor = _mm256_floor_ps(float_cast);
	__m256i int_cast = _mm256_cvttps_epi32(floor);
	return _mm256_cvtsi256_si32(int_cast);
}

ICF int iCeil(float x) {
	__m256 float_cast = _mm256_broadcast_ss(&x);
	__m256 ceil = _mm256_ceil_ps(float_cast);
	__m256i int_cast = _mm256_cvttps_epi32(ceil);
	return _mm256_cvtsi256_si32(int_cast);
}
//--------------------------------------------------------------------
#else
//--------------------------------------------------------------------
ICF int iFloorFPU(float x) {
	int a = *(const int*)(&x);
	int exponent = (127 + 31) - ((a >> 23) & 0xFF);
	int r = (((u32)(a) << 8) | (1U << 31)) >> exponent;
	exponent += 31 - 127;
	{
		int imask = (!(((((1 << (exponent))) - 1) >> 8)&a));
		exponent -= (31 - 127) + 32;
		exponent >>= 31;
		a >>= 31;
		r -= (imask&a);
		r &= exponent;
		r ^= a;
	}
	return r;
}

ICF int iCeilFPU(float x) {
	int a = (*(const int*)(&x));
	int exponent = (127 + 31) - ((a >> 23) & 0xFF);
	int r = (((u32)(a) << 8) | (1U << 31)) >> exponent;
	exponent += 31 - 127;
	{
		int imask = (!(((((1 << (exponent))) - 1) >> 8)&a));
		exponent -= (31 - 127) + 32;
		exponent >>= 31;
		a = ~((a - 1) >> 31); /* change sign */
		r -= (imask&a);
		r &= exponent;
		r ^= a;
		r = -r; /* change sign */
	}
	return r;
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
ICF int iFloorSSE41(float x) {
	__m128 float_cast = _mm_broadcast_ss(&x);
	__m128 floor = _mm_floor_ps(float_cast);
	__m128i int_cast = _mm_cvttps_epi32(floor);
	return _mm_cvtsi128_si32(int_cast);
}

ICF int iCeilSSE41(float x) {
	__m128 float_cast = _mm_broadcast_ss(&x);
	__m128 ceil = _mm_ceil_ps(float_cast);
	__m128i int_cast = _mm_cvttps_epi32(ceil);
	return _mm_cvtsi128_si32(int_cast);
}
//--------------------------------------------------------------------

// По умолчанию пусть используются старые функции для поддержки старых процессоров.
// На новые переключимся в _initialize_cpu() при обнаружении SSE4.1
inline int(__cdecl* iFloor)(float) = iFloorFPU;
inline int(__cdecl* iCeil)(float)  = iCeilFPU;

#endif
