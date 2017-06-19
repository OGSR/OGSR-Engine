#include "stdafx.h"
#pragma hdrstop

#ifdef _EDITOR
#include "skeletonX.h"
#include "skeletoncustom.h"
#else
#include "..\skeletonX.h"
#include "..\skeletoncustom.h"
#endif

// -- offsets -------------------------------------------------------
#define M11 0
#define M12 4
#define M13 8
#define M14 12
#define M21 16
#define M22 20
#define M23 24
#define M24 28
#define M31 32
#define M32 36
#define M33 40
#define M34 44
#define M41 48
#define M42 52
#define M43 56
#define M44 60
// ------------------------------------------------------------------

/*
struct vertBoned2W	// (1+3+3 + 1+3+3 + 2)*4 = 16*4 = 64 bytes
{
	u16	matrix0;
	u16	matrix1;
	Fvector	P0;
	Fvector	N0;
	Fvector	P1;
	Fvector	N1;
	float	w;
	float	u,v;
};

struct vertRender
{
	Fvector	P;
	Fvector	N;
	float	u,v;
};

	IC	void	transform_tiny		(Tvector &dest, const Tvector &v)	const // preferred to use
	{
		dest.x = v.x*_11 + v.y*_21 + v.z*_31 + _41;
		dest.y = v.x*_12 + v.y*_22 + v.z*_32 + _42;
		dest.z = v.x*_13 + v.y*_23 + v.z*_33 + _43;
	}
	IC	void	transform_dir		(Tvector &dest, const Tvector &v)	const 	// preferred to use
	{
		dest.x = v.x*_11 + v.y*_21 + v.z*_31;
		dest.y = v.x*_12 + v.y*_22 + v.z*_32;
		dest.z = v.x*_13 + v.y*_23 + v.z*_33;
	}
	IC	SelfRef	lerp(const Self &p1, const Self &p2, T t )
	{
		T invt = 1.f-t;
		x = p1.x*invt + p2.x*t;
		y = p1.y*invt + p2.y*t;
		z = p1.z*invt + p2.z*t;
		return *this;	
	}

*/

#define LINE1(base) XMMWORD PTR [base+M11]
#define LINE2(base) XMMWORD PTR [base+M21]
#define LINE3(base) XMMWORD PTR [base+M31]
#define LINE4(base) XMMWORD PTR [base+M41]

void __stdcall xrSkin2W_SSE(vertRender*		D,
							vertBoned2W*	S,
							u32				vCount,
							CBoneInstance*	Bones) 
{__asm{
// ------------------------------------------------------------------
	mov			ecx,vCount		; ecx = vCount
// ------------------------------------------------------------------
//	esi		= source _vector_ [S]
//	edi		= result _vector_ [D]
//	eax		= transform matrix 1
//	ebx		= transform matrix 0
// ------------------------------------------------------------------
	mov			edi,D			; edi = D
	mov			esi,S			; esi = S
// ------------------------------------------------------------------
	ALIGN		16				;
	new_dot:					; _new cycle iteration
// ------------------------------------------------------------------
// checking for private case, when matrixes are equal
// ------------------------------------------------------------------
	mov			ax,WORD PTR [esi]S.matrix0		;	ax = matrix0 index
	mov			bx,WORD PTR [esi]S.matrix1		;	bx = matrix1 index
	cmp			ax,bx							;
	jz			save_private_case				; 
// ------------------------------------------------------------------
// calculating transformation matrix 0 addresses
// ------------------------------------------------------------------
	mov			eax,TYPE CBoneInstance			;
	mul			WORD PTR [esi]S.matrix0			;
	add			eax,Bones						;
	mov			ebx,eax							;
// ------------------------------------------------------------------
// calculating transformation matrix 1 addresses
// ------------------------------------------------------------------
	mov			eax,TYPE CBoneInstance			;
	mul			WORD PTR [esi]S.matrix1			;
	add			eax,Bones						;
// ------------------------------------------------------------------
// transform tiny m1 & m0 interleaved
// ------------------------------------------------------------------
	movups		xmm3,XMMWORD PTR [esi]S.P1		; xmm3 = ?.? | v.z | v.y | v.x
	movups		xmm0,XMMWORD PTR [esi]S.P0		; xmm0 = ?.? | v.z | v.y | v.x

// ------------------------------------------------------------------
	movss		xmm7,DWORD PTR [esi]S.w			; xmm7 = ?.? | ?.? | ?.? | w
// ------------------------------------------------------------------

	movaps		xmm4,xmm3						; xmm4 = ?.? | v.z | v.y | v.x
	movaps		xmm1,xmm0						; xmm1 = ?.? | v.z | v.y | v.x

	movaps		xmm5,xmm3						; xmm5 = ?.? | v.z | v.y | v.x
	movaps		xmm2,xmm0						; xmm2 = ?.? | v.z | v.y | v.x

	shufps		xmm4,xmm4,00000000b				; xmm4 = v.x | v.x | v.x | v.x
	shufps		xmm1,xmm1,00000000b				; xmm1 = v.x | v.x | v.x | v.x

	shufps		xmm5,xmm5,01010101b				; xmm5 = v.y | v.y | v.y | v.y
	shufps		xmm2,xmm2,01010101b				; xmm2 = v.y | v.y | v.y | v.y
	
	mulps		xmm4,LINE1(eax)					; xmm4 = v.x*_14 | v.x*_13 | v.x*_12 | v.x*_11
	mulps		xmm1,LINE1(ebx)					; xmm1 = v.x*_14 | v.x*_13 | v.x*_12 | v.x*_11

	mulps		xmm5,LINE2(eax)					; xmm5 = v.y*_24 | v.y*_23 | v.y*_22 | v.y*_21
	mulps		xmm2,LINE2(ebx)					; xmm2 = v.y*_24 | v.y*_23 | v.y*_22 | v.y*_21
	
	addps		xmm4,xmm5						; xmm4 = v.x*_14+v.y*_24 | v.x*_13+v.y*_23 |
												;		 v.x*_12+v.y*_22 | v.x*_11+v.y*_21
	addps		xmm1,xmm2						; xmm1 = v.x*_14+v.y*_24 | v.x*_13+v.y*_23 |
												;		 v.x*_12+v.y*_22 | v.x*_11+v.y*_21

	movaps		xmm5,xmm3						; xmm5 = ?.? | v.z | v.y | v.x
	movaps		xmm2,xmm0						; xmm2 = ?.? | v.z | v.y | v.x

	shufps		xmm5,xmm5,10101010b				; xmm5 = v.z | v.z | v.z | v.z
	shufps		xmm2,xmm2,10101010b				; xmm2 = v.z | v.z | v.z | v.z

	mulps		xmm5,LINE3(eax)					; xmm5 = v.z*_34 | v.z*_33 | v.z*_32 | v.z*_31
	mulps		xmm2,LINE3(ebx)					; xmm2 = v.z*_34 | v.z*_33 | v.z*_32 | v.z*_31

	addps		xmm5,LINE4(eax)					; xmm5 = v.z*_34+_44 | v.z*_33+_43 | 
												;		 v.z*_32+_42 | v.z*_31+_41
	addps		xmm2,LINE4(ebx)					; xmm2 = v.z*_34+_44 | v.z*_33+_43 | 
												;		 v.z*_32+_42 | v.z*_31+_41

	addps		xmm5,xmm4						; xmm5 = v.x*_14+v.y*_24+v.z*_34+_44 | v.x*_13+v.y*_23+v.z*_33+_43 |
												;		 v.x*_12+v.y*_22+v.z*_32+_42 | v.x*_11+v.y*_21+v.z*_31+_41
	addps		xmm2,xmm1						; xmm2 = v.x*_14+v.y*_24+v.z*_34+_44 | v.x*_13+v.y*_23+v.z*_33+_43 |
												;		 v.x*_12+v.y*_22+v.z*_32+_42 | v.x*_11+v.y*_21+v.z*_31+_41
// ------------------------------------------------------------------
// lerp P0 & P1
// ------------------------------------------------------------------
	shufps		xmm7,xmm7,00000000b				; xmm7 = w | w | w | w
// ------------------------------------------------------------------
	subps		xmm5,xmm2			; xmm5 = ?.? | p1.z-p0.z | p1.y-p1.y | p1.x-p0.x
	mulps		xmm5,xmm7			; xmm5 = ?.? | (p1.z-p0.z)*w | (p1.y-p0.y)*w | (p1.x-p0.x)*w
	addps		xmm5,xmm2			; xmm5 = ?.? | (p1.z-p0.z)*w+p0.z | (p1.y-p0.y)*w+p0.y | (p1.x-p0.x)*w+p0.x
// ------------------------------------------------------------------
// => xmm5			: lerp(p0,p1) result	: ?.? | lerp(z) | lerp(y) | lerp(x)
// ------------------------------------------------------------------
// transform dir m1 & m0 interleaved
// ------------------------------------------------------------------	
	movups		xmm3,XMMWORD PTR [esi]S.N1		; xmm3 = ?.? | v.z | v.y | v.x
	movaps		xmm0,XMMWORD PTR [esi]S.N0		; xmm0 = ?.? | v.z | v.y | v.x
	
	movaps		xmm4,xmm3						; xmm4 = ?.? | v.z | v.y | v.x
	movaps		xmm1,xmm0						; xmm1 = ?.? | v.z | v.y | v.x

	movaps		xmm6,xmm3						; xmm6 = ?.? | v.z | v.y | v.x
	movaps		xmm2,xmm0						; xmm2 = ?.? | v.z | v.y | v.x

	shufps		xmm4,xmm4,00000000b				; xmm4 = v.x | v.x | v.x | v.x
	shufps		xmm1,xmm1,00000000b				; xmm1 = v.x | v.x | v.x | v.x

	shufps		xmm6,xmm6,01010101b				; xmm6 = v.y | v.y | v.y | v.y
	shufps		xmm2,xmm2,01010101b				; xmm2 = v.y | v.y | v.y | v.y

	mulps		xmm4,LINE1(eax)					; xmm4 = v.x*_14 | v.x*_13 | v.x*_12 | v.x*_11
	mulps		xmm1,LINE1(ebx)					; xmm1 = v.x*_14 | v.x*_13 | v.x*_12 | v.x*_11

	mulps		xmm6,LINE2(eax)					; xmm6 = v.y*_24 | v.y*_23 | v.y*_22 | v.y*_21
	mulps		xmm2,LINE2(ebx)					; xmm2 = v.y*_24 | v.y*_23 | v.y*_22 | v.y*_21
	
	addps		xmm4,xmm2						; xmm4 = v.x*_14+v.y*_24 | v.x*_13+v.y*_23 |
												;		 v.x*_12+v.y*_22 | v.x*_11+v.y*_21
	addps		xmm1,xmm2						; xmm1 = v.x*_14+v.y*_24 | v.x*_13+v.y*_23 |
												;		 v.x*_12+v.y*_22 | v.x*_11+v.y*_21

	movaps		xmm6,xmm3						; xmm6 = ?.? | v.z | v.y | v.x
	movaps		xmm2,xmm0						; xmm2 = ?.? | v.z | v.y | v.x

	shufps		xmm6,xmm6,10101010b				; xmm6 = v.z | v.z | v.z | v.z
	shufps		xmm2,xmm2,10101010b				; xmm2 = v.z | v.z | v.z | v.z

	mulps		xmm6,LINE3(eax)					; xmm6 = v.z*_34 | v.z*_33 | v.z*_32 | v.z*_31
	mulps		xmm2,LINE3(ebx)					; xmm2 = v.z*_34 | v.z*_33 | v.z*_32 | v.z*_31

	addps		xmm6,xmm4						; xmm6 = v.x*_14+v.y*_24+v.z*_34 | v.x*_13+v.y*_23+v.z*_33 |
												;		 v.x*_12+v.y*_22+v.z*_32 | v.x*_11+v.y*_21+v.z*_31
	addps		xmm2,xmm1						; xmm2 = v.x*_14+v.y*_24+v.z*_34 | v.x*_13+v.y*_23+v.z*_33 |
												;		 v.x*_12+v.y*_22+v.z*_32 | v.x*_11+v.y*_21+v.z*_31
// ------------------------------------------------------------------
// lerp N0 & N1
// ------------------------------------------------------------------
	subps		xmm6,xmm2			; xmm6 = ?.? | p1.z-p0.z | p1.y-p1.y | p1.x-p0.x
	mulps		xmm6,xmm7			; xmm6 = ?.? | (p1.z-p0.z)*w | (p1.y-p0.y)*w | (p1.x-p0.x)*w
	addps		xmm6,xmm2			; xmm6 = ?.? | (p1.z-p0.z)*w+p0.z | (p1.y-p0.y)*w+p0.y | (p1.x-p0.x)*w+p0.x
// ------------------------------------------------------------------
// => xmm6			: lerp(p0,p1) result	: ?.? | lerp(z) | lerp(y) | lerp(x)
// ------------------------------------------------------------------
//	preparing for aligned store
// ------------------------------------------------------------------
	movaps		xmm3,xmm5						; xmm3 = ?.? | z1 | y1 | x1
	movss		xmm3,xmm6						; xmm3 = ?.? | z1 | y1 | x2
	shufps		xmm3,xmm3,00100111b				; xmm3 = x2 | z1 | y1 | ?.?
	movss		xmm3,xmm5						; xmm3 = x2 | z1 | y1 | x1

	shufps		xmm6,xmm6,11001001b				; xmm6 = ?.? | x2 | z2 | y2
	movhps		xmm6,MMWORD PTR [esi]S.u		; xmm6 = v | u | z2 | y2
// ------------------------------------------------------------------
//	writing data
// ------------------------------------------------------------------
	movntps		XMMWORD PTR [edi],xmm3			; 
	movntps		XMMWORD PTR [edi+16],xmm6		; 
// ------------------------------------------------------------------
	add			esi,TYPE vertBoned2W			;	// advance source
	add			edi,TYPE vertRender				;	// advance dest
// ------------------------------------------------------------------
	dec			ecx								;	// ecx = ecx - 1
	jnz			new_dot							;	// ecx==0 ? exit : goto new_dot
	jmp short	we_are_done						;	skip private_case and go exit
// ------------------------------------------------------------------
	ALIGN		16				;
	save_private_case:			; more cheaper out
// ------------------------------------------------------------------
// calculating transformation matrix 0 addresses
// ------------------------------------------------------------------
	mov			eax,TYPE CBoneInstance			;
	mul			WORD PTR [esi]S.matrix0			;
	add			eax,Bones						;
// ------------------------------------------------------------------
// transform tiny & dir m0 interleaved
// ------------------------------------------------------------------
	movups		xmm0,XMMWORD PTR [esi]S.P0		; xmm0 = ?.? | v.z | v.y | v.x
	movaps		xmm3,XMMWORD PTR [esi]S.N0		; xmm3 = ?.? | v.z | v.y | v.x

	movaps		xmm1,xmm0						; xmm1 = ?.? | v.z | v.y | v.x
	movaps		xmm4,xmm3						; xmm4 = ?.? | v.z | v.y | v.x

	movaps		xmm2,xmm0						; xmm2 = ?.? | v.z | v.y | v.x
	movaps		xmm5,xmm3						; xmm5 = ?.? | v.z | v.y | v.x

	shufps		xmm1,xmm1,00000000b				; xmm1 = v.x | v.x | v.x | v.x
	shufps		xmm4,xmm4,00000000b				; xmm4 = v.x | v.x | v.x | v.x

	shufps		xmm2,xmm2,01010101b				; xmm2 = v.y | v.y | v.y | v.y
	shufps		xmm5,xmm5,01010101b				; xmm5 = v.y | v.y | v.y | v.y
	
	mulps		xmm1,LINE1(eax)					; xmm1 = v.x*_14 | v.x*_13 | v.x*_12 | v.x*_11
	mulps		xmm4,LINE1(eax)					; xmm4 = v.x*_14 | v.x*_13 | v.x*_12 | v.x*_11

	mulps		xmm2,LINE2(eax)					; xmm2 = v.y*_24 | v.y*_23 | v.y*_22 | v.y*_21
	mulps		xmm5,LINE2(eax)					; xmm5 = v.y*_24 | v.y*_23 | v.y*_22 | v.y*_21
	
	addps		xmm1,xmm2						; xmm1 = v.x*_14+v.y*_24 | v.x*_13+v.y*_23 |
												;		 v.x*_12+v.y*_22 | v.x*_11+v.y*_21
	addps		xmm4,xmm5						; xmm4 = v.x*_14+v.y*_24 | v.x*_13+v.y*_23 |
												;		 v.x*_12+v.y*_22 | v.x*_11+v.y*_21

	movaps		xmm2,xmm0						; xmm2 = ?.? | v.z | v.y | v.x
	movaps		xmm5,xmm3						; xmm5 = ?.? | v.z | v.y | v.x

	shufps		xmm2,xmm2,10101010b				; xmm2 = v.z | v.z | v.z | v.z
	shufps		xmm5,xmm5,10101010b				; xmm5 = v.z | v.z | v.z | v.z

	mulps		xmm2,LINE3(eax)					; xmm2 = v.z*_34 | v.z*_33 | v.z*_32 | v.z*_31
	mulps		xmm5,LINE3(eax)					; xmm5 = v.z*_34 | v.z*_33 | v.z*_32 | v.z*_31

	addps		xmm2,LINE4(eax)					; xmm2 = v.z*_34+_44 | v.z*_33+_43 | 
												;		 v.z*_32+_42 | v.z*_31+_41
	addps		xmm5,xmm4						; xmm5 = v.x*_14+v.y*_24+v.z*_34 | v.x*_13+v.y*_23+v.z*_33 |
												;		 v.x*_12+v.y*_22+v.z*_32 | v.x*_11+v.y*_21+v.z*_31

	addps		xmm2,xmm1						; xmm2 = v.x*_14+v.y*_24+v.z*_34+_44 | v.x*_13+v.y*_23+v.z*_33+_43 |
												;		 v.x*_12+v.y*_22+v.z*_32+_42 | v.x*_11+v.y*_21+v.z*_31+_41
// ------------------------------------------------------------------
//	preparing for aligned store
// ------------------------------------------------------------------
	movaps		xmm3,xmm2						; xmm3 = ?.? | z1 | y1 | x1
	movss		xmm3,xmm5						; xmm3 = ?.? | z1 | y1 | x2
	shufps		xmm3,xmm3,00100111b				; xmm3 = x2 | z1 | y1 | ?.?
	movss		xmm3,xmm2						; xmm3 = x2 | z1 | y1 | x1

	shufps		xmm5,xmm5,11001001b				; xmm5 = ?.? | x2 | z2 | y2
	movhps		xmm5,MMWORD PTR [esi]S.u		; xmm5 = v | u | z2 | y2
// ------------------------------------------------------------------
//	writing data
// ------------------------------------------------------------------
	movntps		XMMWORD PTR [edi],xmm3			; 
	movntps		XMMWORD PTR [edi+16],xmm5		; 
// ------------------------------------------------------------------
	add			esi,TYPE vertBoned2W			;	// advance source
	add			edi,TYPE vertRender				;	// advance dest
// ------------------------------------------------------------------
	dec			ecx								;	// ecx = ecx - 1
	jnz			new_dot							;	// ecx==0 ? exit : goto new_dot
// ------------------------------------------------------------------
	we_are_done:								;
// ------------------------------------------------------------------
	sfence										;	write back cache
// ------------------------------------------------------------------
}}
