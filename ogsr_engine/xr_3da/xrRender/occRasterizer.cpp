// occRasterizer.cpp: implementation of the occRasterizer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "occRasterizer.h"

occRasterizer	Raster;

void __stdcall fillDW_8x	(void* _p, u32 size, u32 value)
{
	LPDWORD ptr = LPDWORD	(_p);
	LPDWORD end = ptr+size;
	for (; ptr!=end; ptr+=2)
	{
		ptr[0]	= value;
		ptr[1]	= value;
	}
}

IC void propagade_depth			(LPVOID p_dest, LPVOID p_src, int dim)
{
	occD*	dest = (occD*)p_dest;
	occD*	src	 = (occD*)p_src;

	for (int y=0; y<dim; y++)
	{
		for (int x=0; x<dim; x++)
		{
			occD*	base0		= src + (y*2+0)*(dim*2) + (x*2);
			occD*	base1		= src + (y*2+1)*(dim*2) + (x*2);
			occD	f1			= base0[0];
			occD	f2			= base0[1];
			occD	f3			= base1[0];
			occD	f4			= base1[1];
			occD	f			= f1;
			if (f2>f)	f		= f2;
			if (f3>f)	f		= f3;
			if (f4>f)	f		= f4;
			dest[y*dim+x]		= f;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

occRasterizer::occRasterizer	()
{
	
}

occRasterizer::~occRasterizer	()
{
	
}

void occRasterizer::clear		()
{
	u32 size			= occ_dim*occ_dim;
	float f				= 1.f;
	Memory.mem_fill32	(bufFrame,0,size);
	Memory.mem_fill32	(bufDepth,*LPDWORD(&f),size);
}

IC BOOL shared(occTri* T1, occTri* T2)
{
	if (T1==T2)					return TRUE;
	if (T1->adjacent[0]==T2)	return TRUE;
	if (T1->adjacent[1]==T2)	return TRUE;
	if (T1->adjacent[2]==T2)	return TRUE;
	return FALSE;
}

void occRasterizer::propagade	()
{
	// Clip-and-propagade zero level
	occTri**	pFrame	= get_frame	();
	float*		pDepth	= get_depth	();
	for (int y=0; y<occ_dim_0; y++)
	{
		for (int x=0; x<occ_dim_0; x++)
		{
			int				ox=x+2, oy=y+2;
			
			// Y2-connect
			int	pos			= oy*occ_dim+ox;
			int	pos_up		= pos-occ_dim;
			int	pos_down	= pos+occ_dim;
			int	pos_down2	= pos_down+occ_dim;
			
			occTri* Tu1		= pFrame	[pos_up];
			if (Tu1) {
				// We has pixel 1scan up
				if (shared(Tu1,pFrame[pos_down]))
				{
					// We has pixel 1scan down
					float ZR			= (pDepth[pos_up]+pDepth[pos_down])/2;
					if (ZR<pDepth[pos])	{ pFrame[pos] = Tu1; pDepth[pos] = ZR; }
				} else if (shared(Tu1,pFrame[pos_down2])) 
				{
					// We has pixel 2scan down
					float ZR			= (pDepth[pos_up]+pDepth[pos_down2])/2;
					if (ZR<pDepth[pos])	{ pFrame[pos] = Tu1; pDepth[pos] = ZR; }
				}
			}
			
			//
			float d				= pDepth[pos];
			clamp				(d,-1.99f,1.99f);
			bufDepth_0[y][x]	= df_2_s32	(d);
		}
	}
	
	// Propagade other levels
	propagade_depth	(bufDepth_1,bufDepth_0,occ_dim_1);
	propagade_depth	(bufDepth_2,bufDepth_1,occ_dim_2);
	propagade_depth	(bufDepth_3,bufDepth_2,occ_dim_3);
}

IC	BOOL			test_Level	(occD* depth, int dim, float _x0, float _y0, float _x1, float _y1, occD z)
{
	int x0		= iFloor	(_x0*dim+.5f);	clamp(x0,0,		dim-1);
	int x1		= iFloor	(_x1*dim+.5f);	clamp(x1,x0,	dim-1);
	int y0		= iFloor	(_y0*dim+.5f);	clamp(y0,0,		dim-1);
	int y1		= iFloor	(_y1*dim+.5f);	clamp(y1,y0,	dim-1);
	
	// MT-Sync (delayed as possible)
	RImplementation.HOM.MT_SYNC	();

	for (int y=y0; y<=y1; y++)
	{
		occD* base	= depth+y*dim;
		occD* it	= base + x0;
		occD* end	= base + x1;
		for (; it<=end; it++)
			if (z<*it)	return TRUE;
	}
	return FALSE;
}

BOOL occRasterizer::test		(float _x0, float _y0, float _x1, float _y1, float _z)
{ 
	occD	z	= df_2_s32up	(_z)+1;
	return		test_Level		(get_depth_level(0),occ_dim_0,_x0,_y0,_x1,_y1,z);
	/*
	if	(test_Level(get_depth_level(2),occ_dim_2,_x0,_y0,_x1,_y1,z))
	{
		// Visbible on level 2 - test level 0
		return test_Level(get_depth_level(0),occ_dim_0,_x0,_y0,_x1,_y1,z);
	}
	return FALSE;
	*/
}
