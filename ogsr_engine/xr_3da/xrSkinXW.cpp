#include "stdafx.h"

#include "../Layers/xrRender/SkeletonXVertRender.h"
#include "bone.h"
#include "xrSkinXW.hpp"

#ifndef _M_X64
#include <xmmintrin.h>

#define transform_dir(idx,res,SX,SY,SZ,T1)									\
__asm	movzx		eax, WORD PTR [esi][idx*(TYPE u16)]S.m					\
__asm	movaps		res, SX													\
__asm	sal			eax, 5													\
__asm	lea			eax, [eax+eax*4]										\
__asm	movaps		T1, SY													\
__asm	mulps		res, XMMWORD PTR [edx][eax][64]							\
__asm	mulps		T1, XMMWORD PTR [edx][eax][80]							\
__asm	addps		res, T1													\
__asm	movaps		T1, SZ													\
__asm	mulps		T1, XMMWORD PTR [edx][eax][96]							\
__asm	addps		res, T1													

#define transform_tiny(idx,res,SX,SY,SZ,T1)									\
transform_dir(idx,res,SX,SY,SZ,T1)											\
__asm	addps		res, XMMWORD PTR [edx][eax][112]	

#define shuffle_vec(VEC,SX,SY,SZ)											\
__asm	movss		SX, DWORD PTR [esi]VEC.x								\
__asm	movss		SY, DWORD PTR [esi]VEC.y								\
__asm	shufps		SX, SX, _MM_SHUFFLE(1,0,0,0)							\
__asm	movss		SZ, DWORD PTR [esi]VEC.z								\
__asm	shufps		SY, SY, _MM_SHUFFLE(1,0,0,0)							\
__asm	shufps		SZ, SZ, _MM_SHUFFLE(1,0,0,0)							\

#define shuffle_sw4(SW0,SW1,SW2,SW3)										\
__asm	movss		SW3, DWORD PTR [One]									\
__asm	movss		SW0, DWORD PTR [esi][0*(TYPE float)]S.w					\
__asm	movss		SW1, DWORD PTR [esi][1*(TYPE float)]S.w					\
__asm	subss		SW3, SW0												\
__asm	shufps		SW0, SW0, _MM_SHUFFLE(1,0,0,0)							\
__asm	subss		SW3, SW1												\
__asm	movss		SW2, DWORD PTR [esi][2*(TYPE float)]S.w					\
__asm	shufps		SW1, SW1, _MM_SHUFFLE(1,0,0,0)							\
__asm	subss		SW3, SW2												\
__asm	shufps		SW2, SW2, _MM_SHUFFLE(1,0,0,0)							\
__asm	shufps		SW3, SW3, _MM_SHUFFLE(1,0,0,0)							

// ==================================================================
void Skin4W(vertRender* D,
	vertBoned4W*	S,
	u32				vCount,
	CBoneInstance*	Bones)
{
	__m128 P0, P1, P2, P3; DWORD One;
	__asm {
		// ------------------------------------------------------------------
		mov			edi, DWORD PTR[D]; edi = D
		mov			esi, DWORD PTR[S]; esi = S
		mov			ecx, DWORD PTR[vCount]; ecx = vCount
		mov			edx, DWORD PTR[Bones]; edx = Bones
		mov			DWORD PTR[One], 0x3f800000; One = 1.0f
		// ------------------------------------------------------------------
		ALIGN		16;
	new_vert:; _new cycle iteration
		// ------------------------------------------------------------------
		shuffle_vec(S.P, xmm4, xmm5, xmm6);

			 transform_tiny(0, xmm0, xmm4, xmm5, xmm6, xmm7);		xmm0 = P0
				 transform_tiny(1, xmm1, xmm4, xmm5, xmm6, xmm7);		xmm1 = P1
				 transform_tiny(2, xmm2, xmm4, xmm5, xmm6, xmm7);		xmm2 = P2
				 transform_tiny(3, xmm3, xmm4, xmm5, xmm6, xmm7);		xmm3 = P3

				 movaps		XMMWORD PTR[P0], xmm0
				 movaps		XMMWORD PTR[P1], xmm1
				 movaps		XMMWORD PTR[P2], xmm2
				 movaps		XMMWORD PTR[P3], xmm3

				 shuffle_vec(S.N, xmm4, xmm5, xmm6);

			 transform_dir(0, xmm0, xmm4, xmm5, xmm6, xmm7);		xmm0 = N0
				 transform_dir(1, xmm1, xmm4, xmm5, xmm6, xmm7);		xmm1 = N1
				 transform_dir(2, xmm2, xmm4, xmm5, xmm6, xmm7);		xmm2 = N2
				 transform_dir(3, xmm3, xmm4, xmm5, xmm6, xmm7);		xmm3 = N3

				 shuffle_sw4(xmm4, xmm5, xmm6, xmm7);

			 mulps		xmm0, xmm4; xmm0 = N0
				 mulps		xmm1, xmm5; xmm1 = N1
				 mulps		xmm2, xmm6; xmm2 = N2
				 addps		xmm0, xmm1; xmm0 = N0 + N1
				 mulps		xmm3, xmm7; xmm3 = N3

				 mulps		xmm4, XMMWORD PTR[P0]; xmm4 = P0
				 mulps		xmm5, XMMWORD PTR[P1]; xmm5 = P1
				 mulps		xmm6, XMMWORD PTR[P2]; xmm6 = P2
				 addps		xmm4, xmm5; xmm4 = P0 + P1
				 mulps		xmm7, XMMWORD PTR[P3]; xmm7 = P3

				 addps		xmm0, xmm2; xmm0 = N0 + N1 + N2
				 addps		xmm4, xmm6; xmm4 = P0 + P1 + P2
				 addps		xmm0, xmm3; xmm0 = N0 + N1 + N2 + N3 = 00 | Nz | Ny | Nx
				 addps		xmm4, xmm7; xmm4 = P0 + P1 + P2 + P3 = 00 | Pz | Py | Px
				 // ------------------------------------------------------------------
				 movlps		xmm1, MMWORD PTR[esi]S.u; xmm1 = ? ? | ? ? | v | u
				 movaps		xmm5, xmm4; xmm5 = 00 | Pz | Py | Px
				 add			edi, TYPE vertRender;	// advance dest
			 movss		xmm5, xmm0; xmm5 = 00 | Pz | Py | Nx
				 prefetchnta BYTE PTR[esi + 4 * (TYPE vertBoned4W)];		one cache line ahead
				 add			esi, TYPE vertBoned4W;	// advance source

			 shufps		xmm4, xmm5, _MM_SHUFFLE(0, 2, 1, 0); xmm4 = Nx | Pz | Py | Px
				 shufps		xmm0, xmm1, _MM_SHUFFLE(1, 0, 2, 1); xmm0 = v | u | Nz | Ny
				 // ------------------------------------------------------------------
				 dec			ecx;	// vCount--
										// ------------------------------------------------------------------
										//	writing data
										// ------------------------------------------------------------------
			 movntps		XMMWORD PTR[edi - (TYPE vertRender)], xmm4;
			 movntps		XMMWORD PTR[edi + 16 - (TYPE vertRender)], xmm0;
			 // ------------------------------------------------------------------
			 jnz			new_vert;	// vCount == 0 ? exit : goto new_vert
										// ------------------------------------------------------------------
			 sfence;	write back cache
				 // ------------------------------------------------------------------
	}
}


#define shuffle_sw3(SW0,SW1,SW2)											\
__asm	movss		SW2, DWORD PTR [One]									\
__asm	movss		SW0, DWORD PTR [esi][0*(TYPE float)]S.w					\
__asm	movss		SW1, DWORD PTR [esi][1*(TYPE float)]S.w					\
__asm	subss		SW2, SW0												\
__asm	shufps		SW0, SW0, _MM_SHUFFLE(1,0,0,0)							\
__asm	subss		SW2, SW1												\
__asm	shufps		SW1, SW1, _MM_SHUFFLE(1,0,0,0)							\
__asm	shufps		SW2, SW2, _MM_SHUFFLE(1,0,0,0)							


// ==================================================================
void Skin3W(vertRender* D,
	vertBoned3W*	S,
	u32				vCount,
	CBoneInstance*	Bones)
{
	__m128 P0, P1; DWORD One;
	__asm {
		// ------------------------------------------------------------------
		mov			edi, DWORD PTR[D]; edi = D
		mov			esi, DWORD PTR[S]; esi = S
		mov			ecx, DWORD PTR[vCount]; ecx = vCount
		mov			edx, DWORD PTR[Bones]; edx = Bones
		mov			DWORD PTR[One], 0x3f800000; One = 1.0f
		// ------------------------------------------------------------------
		ALIGN		16;
	new_vert:; _new cycle iteration
		// ------------------------------------------------------------------
		shuffle_vec(S.P, xmm4, xmm5, xmm6);

			 transform_tiny(0, xmm0, xmm4, xmm5, xmm6, xmm7);		xmm0 = P0
				 transform_tiny(1, xmm1, xmm4, xmm5, xmm6, xmm7);		xmm1 = P1
				 transform_tiny(2, xmm2, xmm4, xmm5, xmm6, xmm7);		xmm2 = P2

				 movaps		XMMWORD PTR[P0], xmm0
				 movaps		XMMWORD PTR[P1], xmm1

				 shuffle_vec(S.N, xmm4, xmm5, xmm6);

			 transform_dir(0, xmm0, xmm4, xmm5, xmm6, xmm7);		xmm0 = N0
				 transform_dir(1, xmm1, xmm4, xmm5, xmm6, xmm7);		xmm1 = N1
				 transform_dir(2, xmm3, xmm4, xmm5, xmm6, xmm7);		xmm3 = N2

				 shuffle_sw3(xmm4, xmm5, xmm6);

			 mulps		xmm0, xmm4; xmm0 = N0
				 mulps		xmm1, xmm5; xmm1 = N1
				 mulps		xmm3, xmm6; xmm2 = N2

				 addps		xmm0, xmm1; xmm0 = N0 + N1

				 mulps		xmm4, XMMWORD PTR[P0]; xmm4 = P0
				 mulps		xmm5, XMMWORD PTR[P1]; xmm5 = P1
				 mulps		xmm6, xmm2; xmm6 = P2

				 addps		xmm4, xmm5; xmm4 = P0 + P1

				 addps		xmm0, xmm3; xmm0 = N0 + N1 + N2
				 addps		xmm4, xmm6; xmm4 = P0 + P1 + P2
				 // ------------------------------------------------------------------
				 movlps		xmm1, MMWORD PTR[esi]S.u; xmm1 = ? ? | ? ? | v | u
				 movaps		xmm5, xmm4; xmm5 = 00 | Pz | Py | Px
				 add			edi, TYPE vertRender;	// advance dest
			 movss		xmm5, xmm0; xmm5 = 00 | Pz | Py | Nx
				 prefetchnta BYTE PTR[esi + 8 * (TYPE vertBoned3W)];		one cache line ahead
				 add			esi, TYPE vertBoned3W;	// advance source

			 shufps		xmm4, xmm5, _MM_SHUFFLE(0, 2, 1, 0); xmm4 = Nx | Pz | Py | Px
				 shufps		xmm0, xmm1, _MM_SHUFFLE(1, 0, 2, 1); xmm0 = v | u | Nz | Ny
				 // ------------------------------------------------------------------
				 dec			ecx;	// vCount--
										// ------------------------------------------------------------------
										//	writing data
										// ------------------------------------------------------------------
			 movntps		XMMWORD PTR[edi - (TYPE vertRender)], xmm4;
			 movntps		XMMWORD PTR[edi + 16 - (TYPE vertRender)], xmm0;
			 // ------------------------------------------------------------------
			 jnz			new_vert;	// vCount == 0 ? exit : goto new_vert
										// ------------------------------------------------------------------
			 sfence;	write back cache
				 // ------------------------------------------------------------------
	}
}


#define transform_dir2(idx,res,SX,SY,SZ,T1)									\
__asm	movzx		eax, WORD PTR [esi]S.matrix##idx						\
__asm	movaps		res, SX													\
__asm	sal			eax, 5													\
__asm	lea			eax, [eax+eax*4]										\
__asm	movaps		T1, SY													\
__asm	mulps		res, XMMWORD PTR [edx][eax][64]							\
__asm	mulps		T1, XMMWORD PTR [edx][eax][80]							\
__asm	addps		res, T1													\
__asm	movaps		T1, SZ													\
__asm	mulps		T1, XMMWORD PTR [edx][eax][96]							\
__asm	addps		res, T1

#define transform_tiny2(idx,res,SX,SY,SZ,T1)								\
transform_dir2(idx,res,SX,SY,SZ,T1)											\
__asm	addps		res, XMMWORD PTR [edx][eax][112]


// ==================================================================
void Skin2W(vertRender* D,
	vertBoned2W*	S,
	u32				vCount,
	CBoneInstance*	Bones)
{
	__asm {
		// ------------------------------------------------------------------
		mov			edi, DWORD PTR[D]; edi = D
		mov			esi, DWORD PTR[S]; esi = S
		mov			ecx, DWORD PTR[vCount]; ecx = vCount
		mov			edx, DWORD PTR[Bones]; edx = Bones
		// ------------------------------------------------------------------
		ALIGN		16;
	new_vert:; _new cycle iteration
		// ------------------------------------------------------------------
		shuffle_vec(S.P, xmm4, xmm5, xmm6);

			 transform_tiny2(0, xmm0, xmm4, xmm5, xmm6, xmm7);		xmm0 = P0
				 transform_tiny2(1, xmm1, xmm4, xmm5, xmm6, xmm7);		xmm1 = P1

				 shuffle_vec(S.N, xmm4, xmm5, xmm6);

			 transform_dir2(0, xmm2, xmm4, xmm5, xmm6, xmm7);			xmm2 = N0
				 transform_dir2(1, xmm3, xmm4, xmm5, xmm6, xmm7);			xmm3 = N1

				 movss		xmm7, DWORD PTR[esi]S.w; xmm7 = 0 | 0 | 0 | w

				 subps		xmm1, xmm0; xmm1 = P1 - P0
				 shufps		xmm7, xmm7, _MM_SHUFFLE(1, 0, 0, 0); xmm7 = 0 | w | w | w
				 subps		xmm3, xmm2; xmm3 = N1 - N0

				 mulps		xmm1, xmm7; xmm1 = (P1 - P0)*w
				 mulps		xmm3, xmm7; xmm3 = (N1 - N0)*w

				 addps		xmm0, xmm1; xmm0 = P0 + (P1 - P0)*w
				 addps		xmm2, xmm3; xmm2 = N0 + (N1 - N0)*w

				 movlps		xmm7, MMWORD PTR[esi]S.u; xmm7 = ? ? | ? ? | v | u
				 movaps		xmm5, xmm0; xmm5 = 00 | Pz | Py | Px
				 add			edi, TYPE vertRender;	// advance dest
			 movss		xmm5, xmm2; xmm5 = 00 | Pz | Py | Nx
				 prefetchnta BYTE PTR[esi + 12 * (TYPE vertBoned2W)];		one cache line ahead
				 add			esi, TYPE vertBoned2W;	// advance source

			 shufps		xmm0, xmm5, _MM_SHUFFLE(0, 2, 1, 0); xmm0 = Nx | Pz | Py | Px
				 shufps		xmm2, xmm7, _MM_SHUFFLE(1, 0, 2, 1); xmm2 = v | u | Nz | Ny
				 // ------------------------------------------------------------------
				 dec			ecx;	// vCount--
										// ------------------------------------------------------------------
										//	writing data
										// ------------------------------------------------------------------
			 movntps		XMMWORD PTR[edi - (TYPE vertRender)], xmm0;
			 movntps		XMMWORD PTR[edi + 16 - (TYPE vertRender)], xmm2;
			 // ------------------------------------------------------------------
			 jnz			new_vert;	// vCount == 0 ? exit : goto new_vert
										// ------------------------------------------------------------------
			 sfence;	write back cache
				 // ------------------------------------------------------------------
	}
}


// ==================================================================
void Skin1W(vertRender* D,
	vertBoned1W*	S,
	u32				vCount,
	CBoneInstance*	Bones)
{
	__asm {
		// ------------------------------------------------------------------
		mov			edi, DWORD PTR[D]; edi = D
		mov			esi, DWORD PTR[S]; esi = S
		mov			ecx, DWORD PTR[vCount]; ecx = vCount
		mov			edx, DWORD PTR[Bones]; edx = Bones
		// ------------------------------------------------------------------
		ALIGN		16;
	new_vert:; _new cycle iteration
		// ------------------------------------------------------------------
		mov			eax, DWORD PTR[esi]S.matrix;	eax = S.matrix

		movss		xmm0, DWORD PTR[esi]S.P.x
		movss		xmm1, DWORD PTR[esi]S.P.y
		lea			eax, [eax + eax * 4];	eax *= 5 (160)
		shufps		xmm0, xmm0, _MM_SHUFFLE(1, 0, 0, 0)
		movss		xmm2, DWORD PTR[esi]S.P.z
		shufps		xmm1, xmm1, _MM_SHUFFLE(1, 0, 0, 0)
		shufps		xmm2, xmm2, _MM_SHUFFLE(1, 0, 0, 0)

		movss		xmm3, DWORD PTR[esi]S.N.x
		movss		xmm4, DWORD PTR[esi]S.N.y
		sal			eax, 5;	eax *= 32
		shufps		xmm3, xmm3, _MM_SHUFFLE(1, 0, 0, 0)
		movss		xmm5, DWORD PTR[esi]S.N.z
		shufps		xmm4, xmm4, _MM_SHUFFLE(1, 0, 0, 0)
		shufps		xmm5, xmm5, _MM_SHUFFLE(1, 0, 0, 0)

		mulps		xmm0, XMMWORD PTR[edx][eax][64]
		mulps		xmm1, XMMWORD PTR[edx][eax][80]
		mulps		xmm2, XMMWORD PTR[edx][eax][96]

		mulps		xmm3, XMMWORD PTR[edx][eax][64]
		mulps		xmm4, XMMWORD PTR[edx][eax][80]
		mulps		xmm5, XMMWORD PTR[edx][eax][96]

		addps		xmm0, xmm1
		addps		xmm3, xmm4

		addps		xmm0, xmm2
		addps		xmm3, xmm5

		addps		xmm0, XMMWORD PTR[edx][eax][112]

		movlps		xmm1, MMWORD PTR[esi]S.u; xmm1 = ? ? | ? ? | v | u
		movaps		xmm4, xmm0; xmm4 = 00 | Pz | Py | Px
		add			edi, TYPE vertRender;	// advance dest
			 movss		xmm4, xmm3; xmm4 = 00 | Pz | Py | Nx
				 prefetchnta BYTE PTR[esi + 16 * (TYPE vertBoned1W)];		one cache line ahead
				 add			esi, TYPE vertBoned1W;	// advance source

			 shufps		xmm0, xmm4, _MM_SHUFFLE(0, 2, 1, 0); xmm0 = Nx | Pz | Py | Px
				 shufps		xmm3, xmm1, _MM_SHUFFLE(1, 0, 2, 1); xmm3 = v | u | Nz | Ny
				 // ------------------------------------------------------------------
				 dec			ecx;	// vCount--
										// ------------------------------------------------------------------
										//	writing data
										// ------------------------------------------------------------------
			 movntps		XMMWORD PTR[edi - (TYPE vertRender)], xmm0;
			 movntps		XMMWORD PTR[edi + 16 - (TYPE vertRender)], xmm3;
			 // ------------------------------------------------------------------
			 jnz			new_vert;	// vCount == 0 ? exit : goto new_vert
										// ------------------------------------------------------------------
			 sfence;	write back cache
				 // ------------------------------------------------------------------
	}
}

#else

void Skin1W(vertRender* D, vertBoned1W* S, u32 vCount, CBoneInstance* Bones) {
    // Prepare
    int U_Count = vCount / 8;
    vertBoned1W* V = S;
    vertBoned1W* E = V + U_Count * 8;

    // Unrolled loop
    for (; S != E;) {
        Fmatrix& M0 = Bones[S->matrix].mRenderTransform;
        M0.transform_tiny(D->P, S->P);
        M0.transform_dir(D->N, S->N);
        D->u = S->u;
        D->v = S->v;
        S++;
        D++;

        Fmatrix& M1 = Bones[S->matrix].mRenderTransform;
        M1.transform_tiny(D->P, S->P);
        M1.transform_dir(D->N, S->N);
        D->u = S->u;
        D->v = S->v;
        S++;
        D++;

        Fmatrix& M2 = Bones[S->matrix].mRenderTransform;
        M2.transform_tiny(D->P, S->P);
        M2.transform_dir(D->N, S->N);
        D->u = S->u;
        D->v = S->v;
        S++;
        D++;

        Fmatrix& M3 = Bones[S->matrix].mRenderTransform;
        M3.transform_tiny(D->P, S->P);
        M3.transform_dir(D->N, S->N);
        D->u = S->u;
        D->v = S->v;
        S++;
        D++;

        Fmatrix& M4 = Bones[S->matrix].mRenderTransform;
        M4.transform_tiny(D->P, S->P);
        M4.transform_dir(D->N, S->N);
        D->u = S->u;
        D->v = S->v;
        S++;
        D++;

        Fmatrix& M5 = Bones[S->matrix].mRenderTransform;
        M5.transform_tiny(D->P, S->P);
        M5.transform_dir(D->N, S->N);
        D->u = S->u;
        D->v = S->v;
        S++;
        D++;

        Fmatrix& M6 = Bones[S->matrix].mRenderTransform;
        M6.transform_tiny(D->P, S->P);
        M6.transform_dir(D->N, S->N);
        D->u = S->u;
        D->v = S->v;
        S++;
        D++;

        Fmatrix& M7 = Bones[S->matrix].mRenderTransform;
        M7.transform_tiny(D->P, S->P);
        M7.transform_dir(D->N, S->N);
        D->u = S->u;
        D->v = S->v;
        S++;
        D++;
    }

    // The end part
    vertBoned1W* E2 = V + vCount;
    for (; S != E2;) {
        Fmatrix& M = Bones[S->matrix].mRenderTransform;
        M.transform_tiny(D->P, S->P);
        M.transform_dir(D->N, S->N);
        D->u = S->u;
        D->v = S->v;
        S++;
        D++;
    }
}

void Skin2W(vertRender* D, vertBoned2W* S, u32 vCount, CBoneInstance* Bones) {
    // Prepare
    int U_Count = vCount;
    vertBoned2W* V = S;
    vertBoned2W* E = V + U_Count;
    Fvector P0, N0, P1, N1;

    // NON-Unrolled loop
    for (; S != E;) {
        if (S->matrix1 != S->matrix0) {
            Fmatrix& M0 = Bones[S->matrix0].mRenderTransform;
            Fmatrix& M1 = Bones[S->matrix1].mRenderTransform;
            M0.transform_tiny(P0, S->P);
            M0.transform_dir(N0, S->N);
            M1.transform_tiny(P1, S->P);
            M1.transform_dir(N1, S->N);
            D->P.lerp(P0, P1, S->w);
            D->N.lerp(N0, N1, S->w);
            D->u = S->u;
            D->v = S->v;
        } else {
            Fmatrix& M0 = Bones[S->matrix0].mRenderTransform;
            M0.transform_tiny(D->P, S->P);
            M0.transform_dir(D->N, S->N);
            D->u = S->u;
            D->v = S->v;
        }
        S++;
        D++;
    }
}

void Skin3W(vertRender* D, vertBoned3W* S, u32 vCount, CBoneInstance* Bones) {
    // Prepare
    int U_Count = vCount;
    vertBoned3W* V = S;
    vertBoned3W* E = V + U_Count;
    Fvector P0, N0, P1, N1, P2, N2;

    // NON-Unrolled loop
    for (; S != E;) {
        Fmatrix& M0 = Bones[S->m[0]].mRenderTransform;
        Fmatrix& M1 = Bones[S->m[1]].mRenderTransform;
        Fmatrix& M2 = Bones[S->m[2]].mRenderTransform;

        M0.transform_tiny(P0, S->P);
        P0.mul(S->w[0]);
        M0.transform_dir(N0, S->N);
        N0.mul(S->w[0]);

        M1.transform_tiny(P1, S->P);
        P1.mul(S->w[1]);
        M1.transform_dir(N1, S->N);
        N1.mul(S->w[1]);

        M2.transform_tiny(P2, S->P);
        P2.mul(1.0f - S->w[0] - S->w[1]);
        M2.transform_dir(N2, S->N);
        N2.mul(1.0f - S->w[0] - S->w[1]);

        P0.add(P1);
        P0.add(P2);

        D->P = P0;

        N0.add(N1);
        N0.add(N2);

        D->N = N0;

        D->u = S->u;
        D->v = S->v;

        S++;
        D++;
    }
}

void Skin4W(vertRender* D, vertBoned4W* S, u32 vCount, CBoneInstance* Bones) {
    // Prepare
    int U_Count = vCount;
    vertBoned4W* V = S;
    vertBoned4W* E = V + U_Count;
    Fvector P0, N0, P1, N1, P2, N2, P3, N3;

    // NON-Unrolled loop
    for (; S != E;) {
        Fmatrix& M0 = Bones[S->m[0]].mRenderTransform;
        Fmatrix& M1 = Bones[S->m[1]].mRenderTransform;
        Fmatrix& M2 = Bones[S->m[2]].mRenderTransform;
        Fmatrix& M3 = Bones[S->m[3]].mRenderTransform;

        M0.transform_tiny(P0, S->P);
        P0.mul(S->w[0]);
        M0.transform_dir(N0, S->N);
        N0.mul(S->w[0]);

        M1.transform_tiny(P1, S->P);
        P1.mul(S->w[1]);
        M1.transform_dir(N1, S->N);
        N1.mul(S->w[1]);

        M2.transform_tiny(P2, S->P);
        P2.mul(S->w[2]);
        M2.transform_dir(N2, S->N);
        N2.mul(S->w[2]);

        M3.transform_tiny(P3, S->P);
        P3.mul(1.0f - S->w[0] - S->w[1] - S->w[2]);
        M3.transform_dir(N3, S->N);
        N3.mul(1.0f - S->w[0] - S->w[1] - S->w[2]);

        P0.add(P1);
        P0.add(P2);
        P0.add(P3);

        D->P = P0;

        N0.add(N1);
        N0.add(N2);
        N0.add(N3);

        D->N = N0;

        D->u = S->u;
        D->v = S->v;

        S++;
        D++;
    }
}

#endif
