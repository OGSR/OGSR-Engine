#pragma once

#ifndef BORDER
#define BORDER 1
#endif

#include "hash2D.h"

struct UVtri : public _TCF		{ Face*	owner; };
typedef hash2D<UVtri*,128,128>	HASH;

struct lm_layer
{
	enum LMODE
	{
		LMODE_RGBS			= 0,
		LMODE_HS			= 1,
	};

	u32						width;
	u32						height;
	xr_vector<base_color>	surface;
	xr_vector<u8>			marker;
	LMODE					mode;	

	void					create			(u32 w, u32 h)
	{
		width				= w;
		height				= h;
		u32		size		= w*h;
		surface.clear();	surface.resize	(size);
		marker.clear();		marker.assign	(size,0);
	}
	void					destroy			()
	{
		width=height		= 0;
		surface.clear_and_free				();
		marker.clear_and_free				();
	}
	u32						Area			()						{ return (width+2*BORDER)*(height+2*BORDER); }
	void					Pixel			(u32 ID, u8& r, u8& g, u8& b, u8& s, u8& h);
	void					Pack			(xr_vector<u32>& dest);
	void					Pack_hemi		(xr_vector<u32>& dest);

	lm_layer()				{ width=height=0; }
};

class CDeflector
{
public:
	xr_vector<UVtri>			UVpolys;
	Fvector						normal;
	lm_layer					layer;
	Fsphere						Sphere;
	
	BOOL						bMerged;
public:
	CDeflector					();
	~CDeflector					();

	void	OA_SetNormal		(Fvector &_N )	{ normal.set(_N); normal.normalize(); VERIFY(_valid(normal)); }
	BOOL	OA_Place			(Face *owner);
	void	OA_Place			(vecFace& lst);
	void	OA_Export			();
		
	void	GetRect				(Fvector2 &min, Fvector2 &max);
	u32		GetFaceCount()		{ return (u32)UVpolys.size();	};
		
	void	Light				(CDB::COLLIDER* DB, base_lighting* LightsSelected, HASH& H	);
	void	L_Direct			(CDB::COLLIDER* DB, base_lighting* LightsSelected, HASH& H  );
	void	L_Direct_Edge		(CDB::COLLIDER* DB, base_lighting* LightsSelected, Fvector2& p1, Fvector2& p2, Fvector& v1, Fvector& v2, Fvector& N, float texel_size, Face* skip);
	void	L_Calculate			(CDB::COLLIDER* DB, base_lighting* LightsSelected, HASH& H  );

	u16	GetBaseMaterial		() { return UVpolys.front().owner->dwMaterial;	}

	void	Bounds				(u32 ID, Fbox2& dest)
	{
		UVtri& TC		= UVpolys[ID];
		dest.min.set	(TC.uv[0]);
		dest.max.set	(TC.uv[0]);
		dest.modify		(TC.uv[1]);
		dest.modify		(TC.uv[2]);
	}
	void	Bounds_Summary		(Fbox2& bounds)
	{
		bounds.invalidate();
		for (u32 I=0; I<UVpolys.size(); I++)
		{
			Fbox2	B;
			Bounds	(I,B);
			bounds.merge(B);
		}
	}
	void	RemapUV				(xr_vector<UVtri>& dest, u32 base_u, u32 base_v, u32 size_u, u32 size_v, u32 lm_u, u32 lm_v, BOOL bRotate);
	void	RemapUV				(u32 base_u, u32 base_v, u32 size_u, u32 size_v, u32 lm_u, u32 lm_v, BOOL bRotate);
};

typedef xr_vector<UVtri>::iterator UVIt;

extern void		Jitter_Select	(Fvector2* &Jitter, u32& Jcount);
extern void		blit			(u32* dest,		u32 ds_x, u32 ds_y, u32* src,		u32 ss_x, u32 ss_y, u32 px, u32 py, u32 aREF);
extern void		blit			(lm_layer& dst, u32 ds_x, u32 ds_y, lm_layer& src,	u32 ss_x, u32 ss_y, u32 px, u32 py, u32 aREF);
extern void		blit_r			(u32* dest,		u32 ds_x, u32 ds_y, u32* src,		u32 ss_x, u32 ss_y, u32 px, u32 py, u32 aREF);
extern void		blit_r			(lm_layer& dst, u32 ds_x, u32 ds_y, lm_layer& src,	u32 ss_x, u32 ss_y, u32 px, u32 py, u32 aREF);
extern void		lblit			(lm_layer& dst, lm_layer& src, u32 px, u32 py, u32 aREF);
extern void		LightPoint		(CDB::COLLIDER* DB, CDB::MODEL* MDL, base_color_c &C, Fvector &P, Fvector &N, base_lighting& lights, u32 flags, Face* skip);
extern BOOL		ApplyBorders	(lm_layer &lm, u32 ref);

enum
{
	LP_DEFAULT			= 0,
	LP_UseFaceDisable	= (1<<0),
	LP_dont_rgb			= (1<<1),
	LP_dont_hemi		= (1<<2),
	LP_dont_sun			= (1<<3),
};
