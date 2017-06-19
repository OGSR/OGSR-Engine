// SkeletonX.h: interface for the CSkeletonX class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SkeletonXH
#define SkeletonXH
#pragma once

#include "SkeletonCustom.h"

// refs
class ENGINE_API	CKinematics;
class Fvisual		;

#pragma pack(push,4)
struct vertBoned1W			// (3+3+3+3+2+1)*4 = 15*4 = 60 bytes
{
	Fvector	P;
	Fvector	N;
	Fvector	T;
	Fvector	B;
	float	u,v;
	u32		matrix;
	void	get_pos( Fvector& p ) { p.set(P); }
};
struct vertBoned2W			// (1+3+3 + 1+3+3 + 2)*4 = 16*4 = 64 bytes
{
	u16		matrix0;
	u16		matrix1;
	Fvector	P;
	Fvector	N;
	Fvector	T;
	Fvector	B;
	float	w;
	float	u,v;
	void	get_pos( Fvector& p ) { p.set(P); }
};
struct vertRender			// T&B are not skinned, because in R2 skinning occurs always in hardware
{
	Fvector	P;
	Fvector	N;
	float	u,v;
};
#pragma pack(pop)

struct SEnumVerticesCallback;
class ENGINE_API	CSkeletonX
{
protected:
	enum					{ vertRenderFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1		};
	enum					{ RM_SKINNING_SOFT, RM_SINGLE, RM_SKINNING_1B, RM_SKINNING_2B	};

	CKinematics*			Parent		;		// setted up by parent
	ref_smem<vertBoned1W>	Vertices1W	;		// shared
	ref_smem<vertBoned2W>	Vertices2W	;		// shared
	ref_smem<u16>			BonesUsed	;		// actual bones which have influence on vertices

	u16						RenderMode	;	
	u16						ChildIDX	;

	// render-mode specifics
	union {
		struct {			// soft-skinning only
			u32				cache_DiscardID;
			u32				cache_vCount;
			u32				cache_vOffset;
		};
		u32					RMS_boneid;			// single-bone-rendering
		u32					RMS_bonecount;		// skinning, maximal bone ID
	};

	void					_Copy				(CSkeletonX *V);
	void					_Render_soft		(ref_geom& hGeom, 	u32 vCount,	u32 iOffset, u32 pCount);
	void					_Render				(ref_geom& hGeom, 	u32 vCount,	u32 iOffset, u32 pCount);
	void					_Load				(const char* N,		IReader *data,	u32& dwVertCount);

	virtual void			_Load_hw			(Fvisual& V,		void *data)			= 0;
	virtual void			_CollectBoneFaces	(Fvisual* V, u32 iBase, u32 iCount)		= 0;

	void					_FillVerticesSoft1W	(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, u16* indices, CBoneData::FacesVec& faces);
	void					_FillVerticesSoft2W	(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, u16* indices, CBoneData::FacesVec& faces);
	virtual void			_FillVerticesHW1W	(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)	=0;
	virtual void			_FillVerticesHW2W	(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)	=0;
	virtual void			_FillVertices		(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, Fvisual* V, u16 bone_id, u32 iBase, u32 iCount)			=0;

	BOOL					_PickBoneSoft1W		(Fvector& normal, float& range, const Fvector& S, const Fvector& D, u16* indices, CBoneData::FacesVec& faces);
	BOOL					_PickBoneSoft2W		(Fvector& normal, float& range, const Fvector& S, const Fvector& D,	u16* indices, CBoneData::FacesVec& faces);
	virtual BOOL			_PickBoneHW1W		(Fvector& normal, float& range, const Fvector& S, const Fvector& D, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)				=0;
	virtual BOOL			_PickBoneHW2W		(Fvector& normal, float& range, const Fvector& S, const Fvector& D, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)				=0;
	virtual BOOL			_PickBone			(Fvector& normal, float& range, const Fvector& S, const Fvector& D, Fvisual* V, u16 bone_id, u32 iBase, u32 iCount)						=0;
public:
	BOOL					has_visible_bones	();
							CSkeletonX		()	{ Parent = 0; ChildIDX = u16(-1); }

	virtual void			SetParent		(CKinematics* K)					{ Parent = K; }
	virtual void			AfterLoad		(CKinematics* parent, u16 child_idx)=0;
	virtual void			EnumBoneVertices(SEnumVerticesCallback &C, u16 bone_id)=0;
	virtual BOOL			PickBone		(Fvector& normal, float& dist, const Fvector& start, const Fvector& dir, u16 bone_id)=0;
	virtual void			FillVertices	(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, u16 bone_id)=0;
};

#endif // SkeletonXH
