// SkeletonX.cpp: implementation of the CSkeletonX class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)

#ifndef _EDITOR
    #include	"Render.h"
#endif
#include "SkeletonX.h"
#include "SkeletonCustom.h"
#include "fmesh.h"
#include "xrCPU_Pipe.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
shared_str	s_bones_array_const;

#pragma pack(push,1)
/*
float u_P	(s16 v)
{
	return	float(v)/(32767.f/12.f);
}
s16	q_P		(float v)
{
	int		_v	= clampr(iFloor(v*(32767.f/12.f)), -32768, 32767);
	return	s16	(_v);
}
u8	q_N		(float v)
{
	int		_v	= clampr(iFloor((v+1.f)*127.5f), 0, 255);
	return	u8	(_v);
}
s16	q_tc	(float v)
{
	int		_v	= clampr(iFloor(v*(32767.f/16.f)), -32768, 32767);
	return	s16	(_v);
}
#ifdef _DEBUG
float errN	(Fvector3 v, u8* qv)
{
	Fvector3	uv;	
	uv.set		(float(qv[0]),float(qv[1]),float(qv[2])).div(255.f).mul(2.f).sub(1.f);
	uv.normalize();
	return		v.dotproduct(uv);
}
#else
float errN	(Fvector3 v, u8* qv)	{ return 0; }
#endif

static	D3DVERTEXELEMENT9 dwDecl_01W	[] =	// 24bytes
{
	{ 0, 0,		D3DDECLTYPE_SHORT4,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,		0 },	// : P						: 2	: -12..+12
	{ 0, 8,		D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_NORMAL,		0 },	// : N, w=index(RC, 0..1)	: 1	:  -1..+1
	{ 0, 12,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TANGENT,		0 },	// : T						: 1	:  -1..+1
	{ 0, 16,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_BINORMAL,		0 },	// : B						: 1	:  -1..+1
	{ 0, 20,	D3DDECLTYPE_SHORT2,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,		0 },	// : tc						: 1	: -16..+16
	D3DDECL_END()
};
struct	vertHW_1W
{
	s16			_P		[4];
	u32			_N_I	;
	u32			_T		;
	u32			_B		;
	s16			_tc		[2];
	void set	(Fvector3& P, Fvector3 N, Fvector3 T, Fvector3 B, Fvector2& tc, int index)
	{
		N.normalize_safe();
		T.normalize_safe();
		B.normalize_safe();
		_P[0]		= q_P(P.x);
		_P[1]		= q_P(P.y);
		_P[2]		= q_P(P.z);
		_P[3]		= q_P(1);
		_N_I		= color_rgba(q_N(N.x), q_N(N.y), q_N(N.z), u8(index));
		_T			= color_rgba(q_N(T.x), q_N(T.y), q_N(T.z), 0);
		_B			= color_rgba(q_N(B.x), q_N(B.y), q_N(B.z), 0);
		_tc[0]		= q_tc(tc.x);
		_tc[1]		= q_tc(tc.y);
	}
	u16 get_bone()
	{
		return	u16((u16)color_get_A(_N_I)/3);
	}
	void get_pos(Fvector& p)
	{
		p.x			= u_P(_P[0]);
		p.y			= u_P(_P[1]);
		p.z			= u_P(_P[2]);
	}
};

static	D3DVERTEXELEMENT9 dwDecl_2W	[] =	// 28bytes
{
	{ 0, 0,		D3DDECLTYPE_SHORT4,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,		0 },	// : p					: 2	: -12..+12
	{ 0, 8,		D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_NORMAL,		0 },	// : n.xyz, w = weight	: 1	:  -1..+1, w=0..1
	{ 0, 12,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TANGENT,		0 },	// : T						: 1	:  -1..+1
	{ 0, 16,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_BINORMAL,		0 },	// : B						: 1	:  -1..+1
	{ 0, 20,	D3DDECLTYPE_SHORT4,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,		0 },	// : xy(tc), zw(indices): 2	: -16..+16, zw[0..32767]
	D3DDECL_END()
};
struct	vertHW_2W
{
	s16			_P		[4];
	u32			_N_w	;
	u32			_T		;
	u32			_B		;
	s16			_tc_i	[4];
	void set	(Fvector3& P, Fvector3 N, Fvector3 T, Fvector3 B, Fvector2& tc, int index0, int index1, float w)
	{
		N.normalize_safe	();
		T.normalize_safe	();
		B.normalize_safe	();
		_P[0]		= q_P	(P.x);
		_P[1]		= q_P	(P.y);
		_P[2]		= q_P	(P.z);
		_P[3]		= 1;
		_N_w		= color_rgba(q_N(N.x), q_N(N.y), q_N(N.z), u8(clampr(iFloor(w*255.f+.5f),0,255)));
		_T			= color_rgba(q_N(T.x), q_N(T.y), q_N(T.z), 0);
		_B			= color_rgba(q_N(B.x), q_N(B.y), q_N(B.z), 0);
		_tc_i[0]	= q_tc	(tc.x);
		_tc_i[1]	= q_tc	(tc.y);
		_tc_i[2]	= s16	(index0);
		_tc_i[3]	= s16	(index1);
	}
	float get_weight()
	{
		return	float(color_get_A(_N_w))/255.f;
	}
	u16 get_bone(u16 w)
	{
		return	u16((u16)_tc_i[w+2]/3);
	}
	void get_pos(Fvector& p)
	{
		p.x			= u_P(_P[0]);
		p.y			= u_P(_P[1]);
		p.z			= u_P(_P[2]);
	}
};
#pragma pack(pop)

/*
struct	vertHW_NW
{
	s16			_P_cnt	[4];	// position x,y,z,cnt,	4*2		=	8b
	u32			_N		;		// normal	x,y,z,0,	4*1		=	4b,	12b
	u32			_T		;		// tangent	x,y,z,0,	4*1		=	4b,	16b
	u32			_B		;		// binormal	x,y,z,0,	4*1		=	4b,	20b
	u32			_bones	;		// bone ids,			4*1		=	4b,	24b
	u32			_weights;		// weights,				4*1		=	4b, 28b
	s16			_tc		[2];	// qtc,					2*2		=	4b,	32b
	//								*total*						=	32b
	void set	(Fvector3& P, Fvector3 N, Fvector3 T, Fvector3 B, Fvector2& tc, int index0, int index1, float w)
	{
		N.normalize_safe	();
		T.normalize_safe	();
		B.normalize_safe	();
		_P[0]		= q_P	(P.x);
		_P[1]		= q_P	(P.y);
		_P[2]		= q_P	(P.z);
		_P[3]		= 1;
		_N_w		= color_rgba(q_N(N.x), q_N(N.y), q_N(N.z), u8(clampr(iFloor(w*255.f+.5f),0,255)));
		_T			= color_rgba(q_N(T.x), q_N(T.y), q_N(T.z), 0);
		_B			= color_rgba(q_N(B.x), q_N(B.y), q_N(B.z), 0);
		_tc_i[0]	= q_tc	(tc.x);
		_tc_i[1]	= q_tc	(tc.y);
		_tc_i[2]	= s16	(index0);
		_tc_i[3]	= s16	(index1);
	}
	float get_weight()
	{
		return	float(color_get_A(_N_w))/255.f;
	}
	u16 get_bone(u16 w)
	{
		return	u16((u16)_tc_i[w+2]/3);
	}
	void get_pos(Fvector& p)
	{
		p.x			= u_P(_P[0]);
		p.y			= u_P(_P[1]);
		p.z			= u_P(_P[2]);
	}
};
*/

//////////////////////////////////////////////////////////////////////
// Body Part
//////////////////////////////////////////////////////////////////////
void CSkeletonX::AfterLoad	(CKinematics* parent, u16 child_idx)
{
	SetParent				(parent);
    ChildIDX				= child_idx;
}
void CSkeletonX::_Copy		(CSkeletonX *B)
{
	Parent					= NULL;
	ChildIDX				= B->ChildIDX;
	Vertices1W				= B->Vertices1W;
	Vertices2W				= B->Vertices2W;
	BonesUsed				= B->BonesUsed;

	// caution - overlapped (union)
	cache_DiscardID			= B->cache_DiscardID;
	cache_vCount			= B->cache_vCount;
	cache_vOffset			= B->cache_vOffset;
	RenderMode				= B->RenderMode;
	RMS_boneid				= B->RMS_boneid;
	RMS_bonecount			= B->RMS_bonecount;
}
//////////////////////////////////////////////////////////////////////
void CSkeletonX::_Render	(ref_geom& hGeom, u32 vCount, u32 iOffset, u32 pCount)
{
	RCache.stat.r.s_dynamic.add		(vCount);
	switch (RenderMode)
	{
	case RM_SKINNING_SOFT:
		_Render_soft		(hGeom,vCount,iOffset,pCount);
		RCache.stat.r.s_dynamic_sw.add	(vCount);
		break;
	case RM_SINGLE:	
		{
			Fmatrix	W;	W.mul_43	(RCache.xforms.m_w,Parent->LL_GetTransform_R	(u16(RMS_boneid)));
			RCache.set_xform_world	(W);
			RCache.set_Geometry		(hGeom);
			RCache.Render			(D3DPT_TRIANGLELIST,0,0,vCount,iOffset,pCount);
			RCache.stat.r.s_dynamic_inst.add	(vCount);
		}
		break;
	case RM_SKINNING_1B:
	case RM_SKINNING_2B:
		{
			// transfer matrices
			ref_constant			array	= RCache.get_c				(s_bones_array_const);
			u32						count	= RMS_bonecount;
			for (u32 mid = 0; mid<count; mid++)	{
				Fmatrix&	M				= Parent->LL_GetTransform_R				(u16(mid));
#ifdef QUATERNION_SKINNING
				u32			id = mid * 2;

				D3DXVECTOR3 scale, translate;
				D3DXQUATERNION rotate;
				D3DXMatrixDecompose(&scale, &rotate, &translate, (D3DXMATRIX*)&M);

				RCache.set_ca(&*array, id + 0, rotate.x, rotate.y, rotate.z, rotate.w);
				RCache.set_ca(&*array, id + 1, translate.x, translate.y, translate.z, 1.0);
#else
				u32			id				= mid*3;
				RCache.set_ca	(&*array,id+0,M._11,M._21,M._31,M._41);
				RCache.set_ca	(&*array,id+1,M._12,M._22,M._32,M._42);
				RCache.set_ca	(&*array,id+2,M._13,M._23,M._33,M._43);
#endif
			}

			// render
			RCache.set_Geometry		(hGeom);
			RCache.Render			(D3DPT_TRIANGLELIST,0,0,vCount,iOffset,pCount);
			if (RM_SKINNING_1B==RenderMode)	RCache.stat.r.s_dynamic_1B.add	(vCount);
			else							RCache.stat.r.s_dynamic_2B.add	(vCount);
		}
		break;
	}
}
void CSkeletonX::_Render_soft	(ref_geom& hGeom, u32 vCount, u32 iOffset, u32 pCount)
{
	u32 vOffset				= cache_vOffset;

	_VertexStream&	_VS		= RCache.Vertex;
	if (cache_DiscardID!=_VS.DiscardID() || vCount>=cache_vCount )
	{
		vertRender*	Dest	= (vertRender*)_VS.Lock(vCount,hGeom->vb_stride,vOffset);
		cache_DiscardID		= _VS.DiscardID();
		cache_vCount		= vCount;
		cache_vOffset		= vOffset;
		
		Device.Statistic->RenderDUMP_SKIN.Begin	();
		if (*Vertices1W)
		{
			PSGP.skin1W(
				Dest,										// dest
				*Vertices1W,								// source
				vCount,										// count
				Parent->bone_instances						// bones
				);
		} else {
			PSGP.skin2W(
				Dest,										// dest
				*Vertices2W,								// source
				vCount,										// count
				Parent->bone_instances						// bones
				);
		}
		Device.Statistic->RenderDUMP_SKIN.End	();
		_VS.Unlock			(vCount,hGeom->vb_stride);
	}

	RCache.set_Geometry		(hGeom);
	RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,vCount,iOffset,pCount);
}

//////////////////////////////////////////////////////////////////////
void CSkeletonX::_Load	(const char* N, IReader *data, u32& dwVertCount) 
{	
	s_bones_array_const	= "sbones_array";
#pragma todo("container is created in stack!")
	xr_vector<u16>	bids;

	// Load vertices
	R_ASSERT	(data->find_chunk(OGF_VERTICES));
			
	u16			hw_bones	= u16((HW.Caps.geometry.dwRegisters-22)/3);
	u16			sw_bones	= 0;
#ifdef _EDITOR
	hw_bones	= 0;
#endif

	u32			dwVertType,size,it,crc;
	dwVertType	= data->r_u32(); 
	dwVertCount	= data->r_u32();

	RenderMode						= RM_SKINNING_SOFT;
	Render->shader_option_skinning	(-1);
	switch		(dwVertType)
	{
	case OGF_VERTEXFORMAT_FVF_1L: // 1-Link
		{
			size				= dwVertCount*sizeof(vertBoned1W);
			vertBoned1W* VO = (vertBoned1W*)data->pointer();
			for (it=0; it<dwVertCount; it++)	{
				u16		mid = (u16)VO[it].matrix;
				if		(bids.end() == std::find(bids.begin(),bids.end(),mid))	bids.push_back(mid);
				if		(mid>sw_bones)	sw_bones = mid;
			}
#ifdef _EDITOR
			// software
			crc					= crc32	(data->pointer(),size);
			Vertices1W.create	(crc,dwVertCount,(vertBoned1W*)data->pointer());
#else
			if	(1==bids.size())	{
				// HW- single bone
				RenderMode						= RM_SINGLE;
				RMS_boneid						= *bids.begin();
				Render->shader_option_skinning	(0);
			} else if (sw_bones<=hw_bones) {
				// HW- one weight
				RenderMode						= RM_SKINNING_1B;
				RMS_bonecount					= sw_bones+1;
				Render->shader_option_skinning	(1);
			} else {
				// software
				crc					= crc32	(data->pointer(),size);
				Vertices1W.create	(crc,dwVertCount,(vertBoned1W*)data->pointer());
				Render->shader_option_skinning	(-1);
			}
#endif        
		}
		break;
	case OGF_VERTEXFORMAT_FVF_2L: // 2-Link
		{
			size			= dwVertCount*sizeof(vertBoned2W);
			vertBoned2W* VO = (vertBoned2W*)data->pointer();
			for (it=0; it<dwVertCount; it++)	{
				if	(VO[it].matrix0>sw_bones)	sw_bones = VO[it].matrix0;
				if	(VO[it].matrix1>sw_bones)	sw_bones = VO[it].matrix1;
				if	(bids.end() == std::find(bids.begin(),bids.end(),VO[it].matrix0))	bids.push_back(VO[it].matrix0);
				if	(bids.end() == std::find(bids.begin(),bids.end(),VO[it].matrix1))	bids.push_back(VO[it].matrix1);
			}
			if (sw_bones<=hw_bones) {
				// HW- two weights
				RenderMode						= RM_SKINNING_2B;
				RMS_bonecount					= sw_bones+1;
				Render->shader_option_skinning	(2);
			} else {
				// software
				crc			= crc32	(data->pointer(),size);
				Vertices2W.create	(crc,dwVertCount,(vertBoned2W*)data->pointer());
				Render->shader_option_skinning	(-1);
			}
		}
		break;
	default:
		Debug.fatal	(DEBUG_INFO,"Invalid vertex type in skinned model '%s'",N);
		break;
	}
#ifdef _EDITOR
	if (bids.size()>0)	
#else
	if (bids.size()>1)	
#endif
    {
		crc					= crc32(&*bids.begin(),bids.size()*sizeof(u16)); 
		BonesUsed.create	(crc,bids.size(),&*bids.begin());
	}
}

BOOL	CSkeletonX::has_visible_bones		()
{
	//IRender_Visual*	me	= dynamic_cast<IRender_Visual*>	(this);
	//Msg	("~ has_visible_bones: mode[%d] - count[%d], name=%s",RenderMode,BonesUsed.size(),me->dbg_name.c_str());	//.
	if	(RM_SINGLE==RenderMode)	{
	//	Msg	("* has_visible_bones: bone-list: |%d:%s",RMS_boneid,Parent->LL_BoneName_dbg(RMS_boneid));
		return Parent->LL_GetBoneVisible((u16)RMS_boneid)	;
	}

	/*
	string4096	all="",single="";
	for (u32 it=0; it<BonesUsed.size(); it++)
	{
		sprintf		(single,"|%d:%s",BonesUsed[it],Parent->LL_BoneName_dbg(BonesUsed[it]));
		strcat		(all,single);
	}
	Msg	("* has_visible_bones: bone-list: %s",all);
	*/

	for (u32 it=0; it<BonesUsed.size(); it++)
		if (Parent->LL_GetBoneVisible(BonesUsed[it]))	{
			// Msg		("* has_visible_bones: visible: %d",	BonesUsed[it]);
			return	TRUE	;
		}
	// Msg		("* has_visible_bones: non-visible");
	return	FALSE;
}



//-----------------------------------------------------------------------------------------------------
// Wallmarks
//-----------------------------------------------------------------------------------------------------
#include "cl_intersect.h"
BOOL	CSkeletonX::_PickBoneSoft1W	(Fvector& normal, float& dist, const Fvector& S, const Fvector& D, u16* indices, CBoneData::FacesVec& faces)
{
	VERIFY				(*Vertices1W);
	bool intersect		= FALSE;
	for (CBoneData::FacesVecIt it=faces.begin(); it!=faces.end(); it++){
		Fvector			p[3];
		u32 idx			= (*it)*3;
		for (u32 k=0; k<3; k++){
			vertBoned1W& vert		= Vertices1W[indices[idx+k]];
			const Fmatrix& xform	= Parent->LL_GetBoneInstance((u16)vert.matrix).mRenderTransform; 
			xform.transform_tiny	(p[k],vert.P);
		}
		float u,v,range	= flt_max;
		if (CDB::TestRayTri(S,D,p,u,v,range,true)&&(range<dist)){
			normal.mknormal(p[0],p[1],p[2]);
			dist		= range;
			intersect	= TRUE;
		}
	}
	return intersect;
}

BOOL CSkeletonX::_PickBoneSoft2W	(Fvector& normal, float& dist, const Fvector& S, const Fvector& D, u16* indices, CBoneData::FacesVec& faces)
{
	VERIFY				(*Vertices2W);
	bool intersect		= FALSE;
	for (CBoneData::FacesVecIt it=faces.begin(); it!=faces.end(); it++){
		Fvector			p[3];
		u32 idx			= (*it)*3;
		for (u32 k=0; k<3; k++){
			Fvector		P0,P1;
			vertBoned2W& vert		= Vertices2W[indices[idx+k]];
			Fmatrix& xform0			= Parent->LL_GetBoneInstance(vert.matrix0).mRenderTransform; 
			Fmatrix& xform1			= Parent->LL_GetBoneInstance(vert.matrix1).mRenderTransform; 
			xform0.transform_tiny	(P0,vert.P);
			xform1.transform_tiny	(P1,vert.P);
			p[k].lerp				(P0,P1,vert.w);
		}
		float u,v,range	= flt_max;
		if (CDB::TestRayTri(S,D,p,u,v,range,true)&&(range<dist)){
			normal.mknormal(p[0],p[1],p[2]);
			dist		= range;
			intersect	= TRUE;
		}
	}
	return intersect;
}

// Fill Vertices
void CSkeletonX::_FillVerticesSoft1W(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, u16* indices, CBoneData::FacesVec& faces)
{
	VERIFY				(*Vertices1W);
	for (CBoneData::FacesVecIt it=faces.begin(); it!=faces.end(); it++){
		Fvector			p[3];
		u32 idx			= (*it)*3;
		CSkeletonWallmark::WMFace F;
		for (u32 k=0; k<3; k++){
			vertBoned1W& vert		= Vertices1W[indices[idx+k]];
			F.bone_id[k][0]			= (u16)vert.matrix;
			F.bone_id[k][1]			= F.bone_id[k][0];
			F.weight[k]				= 0.f;
			const Fmatrix& xform	= Parent->LL_GetBoneInstance(F.bone_id[k][0]).mRenderTransform; 
			F.vert[k].set			(vert.P);
			xform.transform_tiny	(p[k],F.vert[k]);
		}
		Fvector test_normal;
		test_normal.mknormal	(p[0],p[1],p[2]);
		float cosa				= test_normal.dotproduct(normal);
		if (cosa<EPS)			continue;
		if (CDB::TestSphereTri(wm.ContactPoint(),size,p))
		{
			Fvector				UV;
			for (u32 k=0; k<3; k++){
				Fvector2& uv	= F.uv[k];
				view.transform_tiny(UV,p[k]);
				uv.x			= (1+UV.x)*.5f;
				uv.y			= (1-UV.y)*.5f;
			}
			wm.m_Faces.push_back(F);
		}
	}
}
void CSkeletonX::_FillVerticesSoft2W(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, u16* indices, CBoneData::FacesVec& faces)
{
	VERIFY				(*Vertices2W);
	for (CBoneData::FacesVecIt it=faces.begin(); it!=faces.end(); it++){
		Fvector			p[3];
		u32 idx			= (*it)*3;
		CSkeletonWallmark::WMFace F;
		for (u32 k=0; k<3; k++){
			Fvector		P0,P1;
			vertBoned2W& vert		= Vertices2W[indices[idx+k]];
			F.bone_id[k][0]			= vert.matrix0;
			F.bone_id[k][1]			= vert.matrix1;
			F.weight[k]				= vert.w;
			Fmatrix& xform0			= Parent->LL_GetBoneInstance(F.bone_id[k][0]).mRenderTransform; 
			Fmatrix& xform1			= Parent->LL_GetBoneInstance(F.bone_id[k][1]).mRenderTransform; 
			F.vert[k].set			(vert.P);		
			xform0.transform_tiny	(P0,F.vert[k]);
			xform1.transform_tiny	(P1,F.vert[k]);
			p[k].lerp				(P0,P1,F.weight[k]);
		}
		Fvector test_normal;
		test_normal.mknormal	(p[0],p[1],p[2]);
		float cosa				= test_normal.dotproduct(normal);
		if (cosa<EPS)			continue;
		if (CDB::TestSphereTri(wm.ContactPoint(),size,p)){
			Fvector				UV;
			for (u32 k=0; k<3; k++){
				Fvector2& uv	= F.uv[k];
				view.transform_tiny(UV,p[k]);
				uv.x			= (1+UV.x)*.5f;
				uv.y			= (1-UV.y)*.5f;
			}
			wm.m_Faces.push_back(F);
		}
	}
}



