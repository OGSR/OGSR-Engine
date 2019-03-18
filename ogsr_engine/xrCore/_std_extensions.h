#pragma once

#ifdef DEBUG
#	define BREAK_AT_STRCMP
#endif

#ifdef abs
#undef abs
#endif

#ifdef _MIN
#undef _MIN
#endif

#ifdef _MAX
#undef _MAX
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

// token type definition
struct XRCORE_API xr_token
{
	LPCSTR	name;
	int 	id;
};

IC LPCSTR get_token_name(xr_token* tokens, int key)
{
    for (int k=0; tokens[k].name; k++)
    	if (key==tokens[k].id) return tokens[k].name;
    return "";
}

IC int get_token_id(xr_token* tokens, LPCSTR key)
{
    for (int k=0; tokens[k].name; k++)
    	if ( stricmp(tokens[k].name,key)==0 ) 
			return tokens[k].id;
    return -1;
}

struct XRCORE_API xr_token2
{
	LPCSTR	name;
	LPCSTR	info;
	int 	id;
};

// generic
template <class T>	IC T		_min	(T a, T b)	{ return a<b?a:b;	}
template <class T>	IC T		_max	(T a, T b)	{ return a>b?a:b;	}
template <class T>	IC T		_sqr	(T a)		{ return a*a;		}

// float
IC float	_abs	(float x)		{ return fabsf(x); }
IC float	_sqrt	(float x)		{ return sqrtf(x); }
IC float	_sin	(float x)		{ return sinf(x); }
IC float	_cos	(float x)		{ return cosf(x); }
IC BOOL		_valid	(const float x)
{
	// check for: Signaling NaN, Quiet NaN, Negative infinity ( –INF), Positive infinity (+INF), Negative denormalized, Positive denormalized
	int			cls			= _fpclass		(double(x));
	if (cls&(_FPCLASS_SNAN+_FPCLASS_QNAN+_FPCLASS_NINF+_FPCLASS_PINF+_FPCLASS_ND+_FPCLASS_PD))	
		return	false;	

	/*	*****other cases are*****
	_FPCLASS_NN Negative normalized non-zero 
	_FPCLASS_NZ Negative zero ( – 0) 
	_FPCLASS_PZ Positive 0 (+0) 
	_FPCLASS_PN Positive normalized non-zero 
	*/
	return		true;
}


// double
IC double	_abs	(double x)		{ return fabs(x); }
IC double	_sqrt	(double x)		{ return sqrt(x); }
IC double	_sin	(double x)		{ return sin(x); }
IC double	_cos	(double x)		{ return cos(x); }
IC BOOL		_valid	(const double x)
{
	// check for: Signaling NaN, Quiet NaN, Negative infinity ( –INF), Positive infinity (+INF), Negative denormalized, Positive denormalized
	int			cls			= _fpclass		(x);
	if (cls&(_FPCLASS_SNAN+_FPCLASS_QNAN+_FPCLASS_NINF+_FPCLASS_PINF+_FPCLASS_ND+_FPCLASS_PD))	
		return false;	

	/*	*****other cases are*****
	_FPCLASS_NN Negative normalized non-zero 
	_FPCLASS_NZ Negative zero ( – 0) 
	_FPCLASS_PZ Positive 0 (+0) 
	_FPCLASS_PN Positive normalized non-zero 
	*/
	return		true;
}

// int8
IC s8		_abs	(s8  x)			{ return (x>=0)? x : s8(-x); }
IC s8 		_min	(s8  x, s8  y)	{ return y + ((x - y) & ((x - y) >> (sizeof(s8 ) * 8 - 1))); };
IC s8 		_max	(s8  x, s8  y)	{ return x - ((x - y) & ((x - y) >> (sizeof(s8 ) * 8 - 1))); };

// unsigned int8
IC u8		_abs	(u8 x)			{ return x; }

// int16
IC s16		_abs	(s16 x)			{ return (x>=0)? x : s16(-x); }
IC s16		_min	(s16 x, s16 y)	{ return y + ((x - y) & ((x - y) >> (sizeof(s16) * 8 - 1))); };
IC s16		_max	(s16 x, s16 y)	{ return x - ((x - y) & ((x - y) >> (sizeof(s16) * 8 - 1))); };

// unsigned int16
IC u16		_abs	(u16 x)			{ return x; }

// int32
IC s32		_abs	(s32 x)			{ return (x>=0)? x : s32(-x); }
IC s32		_min	(s32 x, s32 y)	{ return y + ((x - y) & ((x - y) >> (sizeof(s32) * 8 - 1))); };
IC s32		_max	(s32 x, s32 y)	{ return x - ((x - y) & ((x - y) >> (sizeof(s32) * 8 - 1))); };

// int64
IC s64		_abs	(s64 x)			{ return (x>=0)? x : s64(-x); }
IC s64		_min	(s64 x, s64 y)	{ return y + ((x - y) & ((x - y) >> (sizeof(s64) * 8 - 1))); };
IC s64		_max	(s64 x, s64 y)	{ return x - ((x - y) & ((x - y) >> (sizeof(s64) * 8 - 1))); };

IC u32							xr_strlen				( const char* S );


// dest = S1+S2
IC char* strconcat( size_t dest_sz, char* dest, const char* S1, const char* S2 ) {
  size_t L1 = strlen( S1 ), L2 = strlen( S2 );
  ASSERT_FMT( L1 + L2 + 1 <= dest_sz, "strconcat buffer overflow. Size: [%u], str: [%s], [%s]", dest_sz, S1, S2 );

  memcpy( dest, S1, L1 );
  memcpy( dest + L1, S2, L2 + 1 );

  return dest;
}

// dest = S1+S2+S3
IC char* strconcat( size_t dest_sz, char* dest, const char* S1, const char* S2, const char* S3 ) {
  size_t L1 = strlen( S1 ), L2 = strlen( S2 ), L3 = strlen( S3 );
  ASSERT_FMT( L1 + L2 + L3 + 1 <= dest_sz, "strconcat buffer overflow. Size: [%u], str: [%s], [%s], [%s]", dest_sz, S1, S2, S3 );

  memcpy( dest, S1, L1 );
  memcpy( dest + L1, S2, L2 );
  memcpy( dest + L1 + L2, S3, L3 + 1 );

  return dest;
}

// dest = S1+S2+S3+S4
IC char* strconcat( size_t dest_sz, char* dest, const char* S1, const char* S2, const char* S3, const char* S4 ) {
  size_t L1 = strlen( S1 ), L2 = strlen( S2 ), L3 = strlen( S3 ), L4 = strlen( S4 );
  ASSERT_FMT( L1 + L2 + L3 + L4 + 1 <= dest_sz, "strconcat buffer overflow. Size: [%u], str: [%s], [%s], [%s], [%s]", dest_sz, S1, S2, S3, S4 );

  memcpy( dest, S1, L1 );
  memcpy( dest + L1, S2, L2 );
  memcpy( dest + L1 + L2, S3, L3 );
  memcpy( dest + L1 + L2 + L3, S4, L4 + 1 );

  return dest;
}

// dest = S1+S2+S3+S4+S5
IC char* strconcat( size_t dest_sz, char* dest, const char* S1, const char* S2, const char* S3, const char* S4, const char* S5 ) {
  size_t L1 = strlen( S1 ), L2 = strlen( S2 ), L3 = strlen( S3 ), L4 = strlen( S4 ), L5 = strlen( S5 );
  ASSERT_FMT( L1 + L2 + L3 + L4 + L5 + 1 <= dest_sz, "strconcat buffer overflow. Size: [%u], str: [%s], [%s], [%s], [%s], [%s]", dest_sz, S1, S2, S3, S4, S5 );

  memcpy( dest, S1, L1 );
  memcpy( dest + L1, S2, L2 );
  memcpy( dest + L1 + L2, S3, L3 );
  memcpy( dest + L1 + L2 + L3, S4, L4 );
  memcpy( dest + L1 + L2 + L3 + L4, S5, L5 + 1 );

  return dest;
}

// dest = S1+S2+S3+S4+S5+S6
IC char* strconcat( size_t dest_sz, char* dest, const char* S1, const char* S2, const char* S3, const char* S4, const char* S5, const char* S6 ) {
  size_t L1 = strlen( S1 ), L2 = strlen( S2 ), L3 = strlen( S3 ), L4 = strlen( S4 ), L5 = strlen( S5 ), L6 = strlen( S6 );
  ASSERT_FMT( L1 + L2 + L3 + L4 + L5 + L6 + 1 <= dest_sz, "strconcat buffer overflow. Size: [%u], str: [%s], [%s], [%s], [%s], [%s], [%s]", dest_sz, S1, S2, S3, S4, S5, S6 );

  memcpy( dest, S1, L1 );
  memcpy( dest + L1, S2, L2 );
  memcpy( dest + L1 + L2, S3, L3 );
  memcpy( dest + L1 + L2 + L3, S4, L4 );
  memcpy( dest + L1 + L2 + L3 + L4, S5, L5 );
  memcpy( dest + L1 + L2 + L3 + L4 + L5, S6, L6 + 1 );

  return dest;
}

template<typename StrType, typename... Args>
inline char* xr_strconcat(StrType& dest, Args... args) {
    static_assert(std::is_array_v<StrType>);
    static_assert(std::is_same_v<std::remove_extent_t<StrType>, char>);
    dest[0] = 0;
    (strcat_s(dest, args), ...);
    return &dest[0];
}

// return pointer to ".ext"
IC char*						strext					( const char* S )
{	return (char*) strrchr(S,'.');	}

IC u32							xr_strlen				( const char* S )
{	return (u32)strlen(S);			}

IC char*						xr_strlwr				(char* S)
{	return strlwr(S);				}

#ifdef BREAK_AT_STRCMP
XRCORE_API	int					xr_strcmp				( const char* S1, const char* S2 );
#else
IC int							xr_strcmp				( const char* S1, const char* S2 )
{	return strcmp(S1,S2);  }
#endif

XRCORE_API char* xr_strdup( const char* string );

XRCORE_API	char*				timestamp				(string64& dest);

extern XRCORE_API u32			crc32					(const void* P, u32 len);


//KRodin: Всё что ниже - взято из ЗП.
IC int xr_strcpy(LPSTR destination, size_t const destination_size, LPCSTR source)
{
	return strncpy_s(destination, destination_size, source, destination_size);
}

inline int xr_sprintf	( char* destination, size_t const buffer_size, const char* format_string, ... )
{
	va_list args;
	va_start					( args, format_string);
	return						vsprintf_s( destination, buffer_size, format_string, args );
}
template <int count>
IC int xr_strcpy(char(&destination)[count], LPCSTR source)
{
	return xr_strcpy(destination, count, source);
}

template <int count>
inline int __cdecl xr_sprintf(char(&destination)[count], const char* format_string, ...)
{
	va_list args;
	va_start(args, format_string);
	return vsprintf_s(destination, count, format_string, args);
}
