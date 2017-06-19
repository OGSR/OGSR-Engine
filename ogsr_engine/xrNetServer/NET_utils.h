#ifndef _INCDEF_NETUTILS_H_
#define _INCDEF_NETUTILS_H_
#pragma once

#include "client_id.h"

#pragma pack(push,1)



//for presentation
const	u32			NET_PacketSizeLimit	= 8192; //16384;//8192;
// const	u32			NET_PacketSizeLimit	= 16384; 

struct	NET_Buffer
{
	BYTE	data	[NET_PacketSizeLimit];
	u32		count;
};

class	NET_Packet
{
public:

    void            construct( const void* data, unsigned size )
                    {
                        memcpy( B.data, data, size );
                        B.count = size;
                    }
                    
	NET_Buffer		B;
	u32				r_pos;
	u32				timeReceive;
public:
	// writing - main
	IC void write_start(){
		B.count=0;
	}
	IC void	w_begin	( u16 type		)				// begin of packet 'type'
	{
		B.count=0;
		w_u16(type);
	}
	IC void	w		( const void* p, u32 count )
	{
		VERIFY		(p && count);
		VERIFY		(B.count + count < NET_PacketSizeLimit);
		CopyMemory(&B.data[B.count],p,count);
		B.count		+= count;
		VERIFY		(B.count<NET_PacketSizeLimit);
	}
	IC void w_seek	(u32 pos, const void* p, u32 count)	// random write (only inside allocated region)
	{
		VERIFY		(p && count && (pos+count<=B.count));
		CopyMemory(&B.data[pos],p,count);
	}
	IC u32	w_tell	()	{ return B.count; }

	// writing - utilities
	IC void	w_float		( float a       )	{ w(&a,4);					}			// float
	IC void w_vec3		( const Fvector& a) { w(&a,3*sizeof(float));	}			// vec3
	IC void w_vec4		( const Fvector4& a){ w(&a,4*sizeof(float));	}			// vec4
	IC void w_u64		( u64 a			)	{ w(&a,8);					}			// qword (8b)
	IC void w_s64		( s64 a			)	{ w(&a,8);					}			// qword (8b)
	IC void w_u32		( u32 a			)	{ w(&a,4);					}			// dword (4b)
	IC void w_s32		( s32 a			)	{ w(&a,4);					}			// dword (4b)
	IC void w_u24		( u32 a			)	{ w(&a,3);					}			// dword (3b)
	IC void w_u16		( u16 a			)	{ w(&a,2);					}			// word (2b)
	IC void w_s16		( s16 a			)	{ w(&a,2);					}			// word (2b)
	IC void	w_u8		( u8 a			)	{ w(&a,1);					}			// byte (1b)
	IC void	w_s8		( s8 a			)	{ w(&a,1);					}			// byte (1b)
	IC void w_float_q16	( float a, float min, float max)
	{
		VERIFY		(a>=min && a<=max);
		float q		= (a-min)/(max-min);
		w_u16( u16(iFloor(q*65535.f+0.5f)));
	}
	IC void w_float_q8	( float a, float min, float max)
	{
		VERIFY		(a>=min && a<=max);
		float q		= (a-min)/(max-min);
		w_u8( u8(iFloor(q*255.f+0.5f)));
	}
	IC void w_angle16	( float a		)	{
		w_float_q16	(angle_normalize(a),0,PI_MUL_2);
	}
	IC void w_angle8	( float a		)	{
		w_float_q8	(angle_normalize(a),0,PI_MUL_2);
	}
	IC void w_dir		( const Fvector& D) {
		w_u16(pvCompress(D));
	}
	IC void w_sdir		( const Fvector& D) {
		Fvector C;
		float mag		= D.magnitude();
		if (mag>EPS_S)	{
			C.div		(D,mag);
		} else {
			C.set		(0,0,1);
			mag			= 0;
		}
		w_dir	(C);
		w_float (mag);
	}
	IC void w_stringZ			( LPCSTR S )
	{
		w	(S,(u32)xr_strlen(S)+1);
	}
	IC void w_stringZ			( shared_str& p )
	{
    	if (*p)	w(*p,(u32)xr_strlen(p)+1);
        else	w_u8(0);
	}
	IC void w_matrix			(Fmatrix& M)
	{
		w_vec3	(M.i);
		w_vec3	(M.j);
		w_vec3	(M.k);
		w_vec3	(M.c);
	}
	
	IC void w_clientID			(ClientID& C)
	{
		w_u32(C.value());
	}
	
	IC void	w_chunk_open8		(u32& position)
	{
		position			= w_tell	();
		w_u8				(0);
	}
	
	IC void w_chunk_close8		(u32 position)
	{
		u32 size			= u32(w_tell() - position) - sizeof(u8);
		VERIFY				(size<256	);
		u8					_size = (u8)size;
		w_seek				(position,&_size,sizeof(_size));
	}

	IC void	w_chunk_open16		(u32& position)
	{
		position			= w_tell	();
		w_u16				(0);
	}

	IC void w_chunk_close16		(u32 position)
	{
		u32 size			= u32(w_tell() - position) - sizeof(u16);
		VERIFY				(size<65536);
		u16					_size = (u16)size;
		w_seek				(position,&_size,sizeof(_size));
	}

	// reading
	IC void		read_start(){
		r_pos		= 0;
	}

	IC u32		r_begin			( u16& type	)	// returns time of receiving
	{
		r_pos		= 0;
		r_u16		(type);
		return		timeReceive;
	}

	IC void r_seek	(u32 pos)
	{
		VERIFY		(pos < B.count);
		r_pos		= pos;
	}
	IC u32		r_tell			()	{ return r_pos; }

	IC void		r				( void* p, u32 count)
	{
		VERIFY		(p && count);
		CopyMemory(p,&B.data[r_pos],count);
		r_pos		+= count;
		VERIFY		(r_pos<=B.count);
	}
	IC BOOL		r_eof			()
	{
		return r_pos>=B.count;
	}
	IC u32		r_elapsed			()
	{
		return B.count-r_pos;
	}
	IC void		r_advance		(u32 size)
	{
		r_pos		+= size;
		VERIFY		(r_pos<=B.count);
	}

	// reading - utilities
	IC void		r_vec3			(Fvector& A)	{ r(&A,3*sizeof(float));		} // vec3
	IC void		r_vec4			(Fvector4& A)	{ r(&A,4*sizeof(float));		} // vec4
	IC void		r_float			(float& A )		{ r(&A,4);						} // float
	IC void 	r_u64			(u64& A)		{ r(&A,8);						} // qword (8b)
	IC void 	r_s64			(s64& A)		{ r(&A,8);						} // qword (8b)
	IC void 	r_u32			(u32& A)		{ r(&A,4);						} // dword (4b)
	IC void		r_s32			(s32& A)		{ r(&A,4);						} // dword (4b)
	IC void		r_u24			(u32& A)		{ A=0; r(&A,3);					} // dword (3b)
	IC void		r_u16			(u16& A)		{ r(&A,2);						} // word (2b)
	IC void		r_s16			(s16& A)		{ r(&A,2);						} // word (2b)
	IC void		r_u8			(u8&  A)		{ r(&A,1);						} // byte (1b)
	IC void		r_s8			(s8&  A)		{ r(&A,1);						} // byte (1b)
	// IReader compatibility
	IC Fvector	r_vec3			()		{Fvector A;r(&A,3*sizeof(float));	return(A);		} // vec3
	IC Fvector4	r_vec4			()		{Fvector4 A;r(&A,4*sizeof(float));	return(A);		} // vec4
	IC float	r_float_q8		(float min,float max){float A;r_float_q8(A,min,max);return A;}
	IC float	r_float_q16		(float min, float max){float A;r_float_q16(A,min,max);return A;}
	IC float	r_float			()		{ float A; r(&A,4);					return(A);		} // float
	IC u64 		r_u64			()		{ u64 	A; r(&A,8);					return(A);		} // qword (8b)
	IC s64 		r_s64			()		{ s64 	A; r(&A,8);					return(A);		} // qword (8b)
	IC u32 		r_u32			()		{ u32 	A; r(&A,4);					return(A);		} // dword (4b)
	IC s32		r_s32			()		{ s32	A; r(&A,4);					return(A);		} // dword (4b)
	IC u32		r_u24			()		{ u32	A=0; r(&A,3);				return(A);		} // dword (3b)
	IC u16		r_u16			()		{ u16	A; r(&A,2);					return(A);		} // word (2b)
	IC s16		r_s16			()		{ s16	A; r(&A,2);					return(A);		} // word (2b)
	IC u8		r_u8			()		{ u8	A; r(&A,1);					return(A);		} // byte (1b)
	IC s8		r_s8			()		{ s8	A; r(&A,1);					return(A);		} // byte (1b)

	IC void		r_float_q16		(float& A, float min, float max)
	{
		u16				val;
		r_u16			(val);
		A				= (float(val)*(max-min))/65535.f + min;		// floating-point-error possible
		VERIFY			((A >= min-EPS_S) && (A <= max+EPS_S));
	}
	IC void		r_float_q8		(float& A, float min, float max)
	{
		u8				val;
		r_u8			(val);
		A				= (float(val)/255.0001f) *(max-min) + min;	// floating-point-error possible
		VERIFY			((A >= min) && (A <= max));
	}
	IC void		r_angle16		(float& A)		{ r_float_q16	(A,0,PI_MUL_2);	}
	IC void		r_angle8		(float& A)		{ r_float_q8	(A,0,PI_MUL_2);	}
	IC void		r_dir			(Fvector& A)	{ u16 t; r_u16(t); pvDecompress  (A,t); }

	IC void		r_sdir			(Fvector& A)
	{
		u16	t;	r_u16	(t);
		float s;r_float	(s);
		pvDecompress	(A,t);
		A.mul			(s);
	}

	IC void		r_stringZ		( LPSTR S )
	{
		LPCSTR	data	= LPCSTR(&B.data[r_pos]);
		size_t	len		= xr_strlen(data);
		r		(S,(u32)len+1);
	}
    
	IC void		r_stringZ		( xr_string& dest )
	{
		dest		= LPCSTR(&B.data[r_pos]);
		r_advance	(u32(dest.size()+1));
	}

	void 		r_stringZ		(shared_str& dest)
    {
        dest		= LPCSTR(&B.data[r_pos]);
        r_advance	(dest.size()+1);
    }

	IC void		r_matrix		(Fmatrix& M)
	{
		r_vec3	(M.i);	M._14_	= 0;
		r_vec3	(M.j);	M._24_	= 0;
		r_vec3	(M.k);	M._34_	= 0;
		r_vec3	(M.c);	M._44_	= 1;
	}
	IC void		r_clientID		(ClientID& C)
	{
		u32 tmp;
		r_u32(tmp);
		C.set(tmp);
	}
};

#pragma pack(pop)

#endif /*_INCDEF_NETUTILS_H_*/

