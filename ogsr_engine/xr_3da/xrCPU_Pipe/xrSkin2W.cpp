#include "stdafx.h"
#pragma hdrstop

#ifdef _EDITOR
#include "skeletonX.h"
#include "skeletoncustom.h"
#else
#include "..\skeletonX.h"
#include "..\skeletoncustom.h"
#endif

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
*/

//#pragma optimize("a",on)
// it means no aliasing inside the function, /Oa compiler option
void __stdcall xrSkin2W_x86(vertRender*		D,
							vertBoned2W*	S,
							u32				vCount,
							CBoneInstance*	Bones) 
{
	// Prepare
	int U_Count			= vCount;
	vertBoned2W*	V	= S;
	vertBoned2W*	E	= V+U_Count;
	Fvector			P0,N0,P1,N1;

	// NON-Unrolled loop
	for (; S!=E; ){
    	if (S->matrix1!=S->matrix0){
            Fmatrix& M0		= Bones[S->matrix0].mRenderTransform;
            Fmatrix& M1		= Bones[S->matrix1].mRenderTransform;
            M0.transform_tiny(P0,S->P);
            M0.transform_dir (N0,S->N);
            M1.transform_tiny(P1,S->P);
            M1.transform_dir (N1,S->N);
            D->P.lerp		(P0,P1,S->w);
            D->N.lerp		(N0,N1,S->w);
            D->u			= S->u;
            D->v			= S->v;
        }else{
            Fmatrix& M0		= Bones[S->matrix0].mRenderTransform;
            M0.transform_tiny(D->P,S->P);
            M0.transform_dir (D->N,S->N);
            D->u			= S->u;
            D->v			= S->v;
        }
		S++; D++;
	}
}
