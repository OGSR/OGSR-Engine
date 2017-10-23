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
struct ENGINE_API vertBoned1W			// (3+3+3+3+2+1)*4 = 15*4 = 60 bytes
{
	Fvector	P;
	Fvector	N;
	Fvector	T;
	Fvector	B;
	float	u,v;
	u32		matrix;
	void	get_pos( Fvector& p ) const { p.set(P); }
#ifdef	DEBUG	
	static const u8 bones_count = 1;
	u16		get_bone_id(u8 bone)const { VERIFY(bone<bones_count); return u16(matrix); }
#endif
};
struct ENGINE_API vertBoned2W			// (1+3+3 + 1+3+3 + 2)*4 = 16*4 = 64 bytes
{
	u16		matrix0;
	u16		matrix1;
	Fvector	P;
	Fvector	N;
	Fvector	T;
	Fvector	B;
	float	w;
	float	u,v;
	void	get_pos( Fvector& p ) const { p.set(P); }
#ifdef	DEBUG
	static const u8 bones_count = 2;
	u16		get_bone_id(u8 bone)const { VERIFY(bone<bones_count); return bone == 0 ? matrix0 : matrix1; }
#endif
};
struct ENGINE_API vertBoned3W          // 70 bytes
{
	u16		m[3];
	Fvector	P;
	Fvector	N;
	Fvector	T;
	Fvector	B;
	float	w[2];
	float	u, v;
	void	get_pos(Fvector& p) const { p.set(P); }
#ifdef	DEBUG
	static const u8 bones_count = 3;
	u16		get_bone_id(u8 bone)const { VERIFY(bone<bones_count); return m[bone]; }
#endif
};
struct ENGINE_API vertBoned4W       //76 bytes
{
	u16		m[4];
	Fvector	P;
	Fvector	N;
	Fvector	T;
	Fvector	B;
	float	w[3];
	float	u, v;
	void	get_pos(Fvector& p) const { p.set(P); }
#ifdef	DEBUG
	static const u8 bones_count = 4;
	u16		get_bone_id(u8 bone)const { VERIFY(bone<bones_count); return m[bone]; }
#endif
};

template<typename T>
IC void get_pos_bones(const T& v, Fvector& p, CKinematics* Parent)
{
	v.get_pos_bones(p, Parent);
}

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
	enum					{ RM_SKINNING_SOFT, RM_SINGLE, RM_SKINNING_1B, RM_SKINNING_2B, RM_SKINNING_3B, RM_SKINNING_4B	};

	CKinematics*			Parent		;		// setted up by parent
	ref_smem<vertBoned1W>	Vertices1W	;		// shared
	ref_smem<vertBoned2W>	Vertices2W	;		// shared
	ref_smem<vertBoned3W>	Vertices3W	;		// shared
	ref_smem<vertBoned4W>	Vertices4W	;		// shared
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
	void					_FillVerticesSoft3W	(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, u16* indices, CBoneData::FacesVec& faces);
	void					_FillVerticesSoft4W	(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, u16* indices, CBoneData::FacesVec& faces);
	virtual void			_FillVerticesHW1W	(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)	=0;
	virtual void			_FillVerticesHW2W	(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)	=0;
	virtual void			_FillVerticesHW3W	(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)	=0;
	virtual void			_FillVerticesHW4W	(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)	=0;
	virtual void			_FillVertices		(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, Fvisual* V, u16 bone_id, u32 iBase, u32 iCount)			=0;

	BOOL					_PickBoneSoft1W		(CKinematics::pick_result &r, float range, const Fvector& S, const Fvector& D, u16* indices, CBoneData::FacesVec& faces);
	BOOL					_PickBoneSoft2W		(CKinematics::pick_result &r, float range, const Fvector& S, const Fvector& D,	u16* indices, CBoneData::FacesVec& faces);
	BOOL					_PickBoneSoft3W		(CKinematics::pick_result &r, float range, const Fvector& S, const Fvector& D, u16* indices, CBoneData::FacesVec& faces);
	BOOL					_PickBoneSoft4W		(CKinematics::pick_result &r, float range, const Fvector& S, const Fvector& D, u16* indices, CBoneData::FacesVec& faces);
	virtual BOOL			_PickBoneHW1W		(CKinematics::pick_result &r, float range, const Fvector& S, const Fvector& D, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)				=0;
	virtual BOOL			_PickBoneHW2W		(CKinematics::pick_result &r, float range, const Fvector& S, const Fvector& D, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)				=0;
	virtual BOOL			_PickBoneHW3W		(CKinematics::pick_result &r, float range, const Fvector& S, const Fvector& D, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)				=0;
	virtual BOOL			_PickBoneHW4W		(CKinematics::pick_result &r, float range, const Fvector& S, const Fvector& D, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)				=0;
	virtual BOOL			_PickBone			(CKinematics::pick_result &r, float range, const Fvector& S, const Fvector& D, Fvisual* V, u16 bone_id, u32 iBase, u32 iCount)						=0;
public:
	BOOL					has_visible_bones	();
							CSkeletonX		()	{ Parent = 0; ChildIDX = u16(-1); }

	virtual void			SetParent		(CKinematics* K)					{ Parent = K; }
	virtual void			AfterLoad		(CKinematics* parent, u16 child_idx)=0;
	virtual void			EnumBoneVertices(SEnumVerticesCallback &C, u16 bone_id)=0;
	virtual BOOL			PickBone		(CKinematics::pick_result &r, float dist, const Fvector& start, const Fvector& dir, u16 bone_id)=0;
	virtual void			FillVertices	(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, u16 bone_id)=0;
};

template<typename T_vertex, typename T_buffer >
BOOL pick_bone(T_buffer vertices, CKinematics* Parent, CKinematics::pick_result &r, float dist, const Fvector& S, const Fvector& D, u16* indices, CBoneData::FacesVec& faces)
{
	for (CBoneData::FacesVecIt it = faces.begin(); it != faces.end(); it++) {

		u32 idx = (*it) * 3;
		for (u32 k = 0; k<3; k++) {
			const T_vertex& vert = vertices[indices[idx + k]];
			get_pos_bones(vert, r.tri[k], Parent);
		}
		float u, v;
		r.dist = flt_max;
		if (CDB::TestRayTri(S, D, r.tri, u, v, r.dist, true) && (r.dist<dist)) { //KRodin: fixed! Именно из-за простейшей ошибки в этой строке не работали воллмарки на динамических объектах.
			r.normal.mknormal(r.tri[0], r.tri[1], r.tri[2]);
			return TRUE;
		};
	}
	return FALSE;
}

template<typename T>
BOOL pick_bone(CKinematics* Parent, CKinematics::pick_result &r, float dist, const Fvector& S, const Fvector& D, Fvisual* V, u16* indices, CBoneData::FacesVec& faces)
{
	T* vertices;
	CHK_DX(V->p_rm_Vertices->Lock(V->vBase, V->vCount, (void**)&vertices, D3DLOCK_READONLY));
	bool intersect = !!pick_bone<T, T*>(vertices, Parent, r, dist, S, D, indices, faces);
	CHK_DX(V->p_rm_Vertices->Unlock());
	return intersect;
}

#endif // SkeletonXH
