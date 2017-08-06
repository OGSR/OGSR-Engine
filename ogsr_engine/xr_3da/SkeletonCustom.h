//---------------------------------------------------------------------------
#ifndef SkeletonCustomH
#define SkeletonCustomH

#include		"fhierrarhyvisual.h"
#include		"bone.h"

// consts
const	u32					MAX_BONE_PARAMS		=	4;
const	u32					UCalc_Interval		=	100;	// 10 fps
extern	xrCriticalSection	UCalc_Mutex			;

// refs
class	ENGINE_API CKinematics;
class	ENGINE_API CInifile;
class	ENGINE_API CBoneData;
class   ENGINE_API CBoneInstance;
struct	SEnumVerticesCallback;
// t-defs
typedef xr_vector<CBoneData*>		vecBones;
typedef vecBones::iterator			vecBonesIt;

// callback
typedef void (* BoneCallback)		(CBoneInstance* P);
typedef void (* UpdateCallback)		(CKinematics*	P);

// MT-locker
struct	UCalc_mtlock	{
	UCalc_mtlock()		{ UCalc_Mutex.Enter(); }
	~UCalc_mtlock()		{ UCalc_Mutex.Leave(); }
};

//*** Bone Instance *******************************************************************************
#pragma pack(push,8)
class ENGINE_API		CBoneInstance
{
public:
	// data
	Fmatrix				mTransform;							// final x-form matrix (local to model)
	Fmatrix				mRenderTransform;					// final x-form matrix (model_base -> bone -> model)
	BoneCallback		Callback;
	void*				Callback_Param;
	BOOL				Callback_overwrite;					// performance hint - don't calc anims
	float				param			[MAX_BONE_PARAMS];	// 
	u32					Callback_type;						//
	// methods
	void				construct		();
	void				set_callback	(u32 Type, BoneCallback C, void* Param, BOOL overwrite=FALSE);
	void				reset_callback	();
	void				set_param		(u32 idx, float data);
	float				get_param		(u32 idx);

	u32					mem_usage		(){return sizeof(*this);}
};
#pragma pack(pop)

//*** Shared Bone Data ****************************************************************************
class ENGINE_API		CBoneData
{
protected:
	u16					SelfID;
	u16					ParentID;
public:
	shared_str			name;

	vecBones			children;		// bones which are slaves to this
	Fobb				obb;			

	Fmatrix				bind_transform;
    Fmatrix				m2b_transform;	// model to bone conversion transform
    SBoneShape			shape;
    shared_str			game_mtl_name;
	u16					game_mtl_idx;
    SJointIKData		IK_data;
    float				mass;
    Fvector				center_of_mass;

	DEFINE_VECTOR		(u16,FacesVec,FacesVecIt);
	DEFINE_VECTOR		(FacesVec,ChildFacesVec,ChildFacesVecIt);
	ChildFacesVec		child_faces;	// shared
public:    
						CBoneData		(u16 ID):SelfID(ID)	{VERIFY(SelfID!=BI_NONE);}
	virtual				~CBoneData		()					{}
#ifdef DEBUG
	typedef svector<int,128>	BoneDebug;
	void						DebugQuery		(BoneDebug& L);
#endif
	IC void				SetParentID		(u16 id){ParentID=id;}
	
	IC u16				GetSelfID		() const {return SelfID;}
	IC u16				GetParentID		() const {return ParentID;}

	// assign face
	void				AppendFace		(u16 child_idx, u16 idx)
	{
		child_faces[child_idx].push_back(idx);
	}
	// Calculation
	void				CalculateM2B	(const Fmatrix& Parent);

	virtual u32			mem_usage		()
	{
		u32 sz			= sizeof(*this)+sizeof(vecBones::value_type)*children.size();
		for (ChildFacesVecIt c_it=child_faces.begin(); c_it!=child_faces.end(); c_it++)
			sz			+= c_it->size()*sizeof(FacesVec::value_type)+sizeof(*c_it);
		return			sz;
	}
};

#pragma warning(push)
#pragma warning(disable:4275)
class ENGINE_API CSkeletonWallmark : public intrusive_base // 4+4+4+12+4+16+16 = 60 + 4 = 64
{
#pragma warning(pop)
	CKinematics*		m_Parent;		// 4
	const Fmatrix*		m_XForm;		// 4
	ref_shader			m_Shader;		// 4
	Fvector3			m_ContactPoint;	// 12		model space
	float				m_fTimeStart;	// 4
public:
#ifdef DEBUG
	u32					used_in_render;	
#endif
	Fsphere				m_LocalBounds;	// 16		model space
	struct WMFace{
		Fvector3		vert	[3];
		Fvector2		uv		[3];
		u16				bone_id	[3][4];
		float			weight	[3][3];
	};
	DEFINE_VECTOR		(WMFace,WMFacesVec,WMFacesVecIt);
	WMFacesVec			m_Faces;		// 16 
public:
	Fsphere				m_Bounds;		// 16		world space
public:									
						CSkeletonWallmark	(CKinematics* p,const Fmatrix* m, ref_shader s, const Fvector& cp, float ts):
						m_Parent(p),m_XForm(m),m_Shader(s),m_fTimeStart(ts),m_ContactPoint(cp)
						{
#ifdef DEBUG
						used_in_render = u32(-1);
#endif
						
						}
						~CSkeletonWallmark	()
#ifdef DEBUG
							;
#else
							{}
#endif

	IC CKinematics*		Parent				(){return m_Parent;}
	IC u32				VCount				(){return m_Faces.size()*3;}
	IC bool				Similar				(ref_shader& sh, const Fvector& cp, float eps){return (m_Shader==sh)&&m_ContactPoint.similar(cp,eps);}
	IC float			TimeStart			(){return m_fTimeStart;}
	IC const Fmatrix*	XFORM				(){return m_XForm;}
	IC const Fvector3&	ContactPoint		(){return m_ContactPoint;}
	IC ref_shader		Shader				(){return m_Shader;}
};
DEFINE_VECTOR(intrusive_ptr<CSkeletonWallmark>,SkeletonWMVec,SkeletonWMVecIt);

// sanity check
#ifdef DEBUG
	struct dbg_marker{
		BOOL*			lock;
		dbg_marker		(BOOL* b)	{lock=b; VERIFY(*lock==FALSE); *lock=TRUE;}
		~dbg_marker		()			{*lock=FALSE;}
	};
#	define _DBG_SINGLE_USE_MARKER	dbg_marker	_dbg_marker(&dbg_single_use_marker)
#else
#	define _DBG_SINGLE_USE_MARKER
#endif

#include "vismask.h"
class ENGINE_API	CKinematics: public FHierrarhyVisual
{
	typedef FHierrarhyVisual	inherited;
	friend class				CBoneData;
	friend class				CSkeletonX;

public: 

	struct	pick_result
	{
		Fvector	normal;
		float	dist;
		Fvector	tri[3];
	};
#ifdef DEBUG
	BOOL						dbg_single_use_marker;
#endif
	virtual void				Bone_Calculate		(CBoneData* bd, Fmatrix* parent);
	virtual void				OnCalculateBones	(){}
public:
	typedef xr_vector<std::pair<shared_str,u16> >	accel;
public:
	IRender_Visual*				m_lod;
protected:
	SkeletonWMVec				wallmarks;
	u32							wm_frame;

	xr_vector<IRender_Visual*>	children_invisible	;

	// Globals
    CInifile*					pUserData;
	CBoneInstance*				bone_instances;	// bone instances
	vecBones*					bones;			// all bones	(shared)
	u16							iRoot;			// Root bone index

	// Fast search
	accel*						bone_map_N;		// bones  associations	(shared)	- sorted by name
	accel*						bone_map_P;		// bones  associations	(shared)	- sorted by name-pointer

	BOOL						Update_Visibility		;
	u32							UCalc_Time				;
	s32							UCalc_Visibox			;
  
	VisMask						visimask				;
    
	CSkeletonX*					LL_GetChild				(u32 idx);

	// internal functions
	virtual CBoneData*			CreateBoneData			(u16 ID){return xr_new<CBoneData>(ID);}
	virtual void				IBoneInstances_Create	();
	virtual void				IBoneInstances_Destroy	();
	void						Visibility_Invalidate	()	{ Update_Visibility=TRUE; };
	void						Visibility_Update		()	;

    void						LL_Validate				();
public:
	UpdateCallback				Update_Callback;
	void*						Update_Callback_Param;
public:
	// wallmarks
	void						AddWallmark			(const Fmatrix* parent, const Fvector3& start, const Fvector3& dir, ref_shader shader, float size);
	void						CalculateWallmarks	();
	void						RenderWallmark		(intrusive_ptr<CSkeletonWallmark> wm, FVF::LIT* &verts);
	void						ClearWallmarks		();
public:
				
				bool			PickBone			(const Fmatrix &parent_xform, CKinematics::pick_result& r, float dist, const Fvector& start, const Fvector& dir, u16 bone_id);
	virtual		void			EnumBoneVertices	(SEnumVerticesCallback &C, u16 bone_id);
public:
								CKinematics			();
	virtual						~CKinematics		();
	CInifile					*GetIniFile			()	{ return pUserData; }

	// Low level interface
	u16							LL_BoneID		(LPCSTR  B);
	u16							LL_BoneID		(const shared_str& B);
	LPCSTR						LL_BoneName_dbg	(u16 ID);

    CInifile*					LL_UserData			(){return pUserData;}
	accel*						LL_Bones			(){return bone_map_N;}
	ICF CBoneInstance&			LL_GetBoneInstance	(u16 bone_id)		{	VERIFY(bone_id<LL_BoneCount()); VERIFY(bone_instances); return bone_instances[bone_id];	}
	CBoneData&					LL_GetData			(u16 bone_id)		{	VERIFY(bone_id<LL_BoneCount()); VERIFY(bones);			return *((*bones)[bone_id]);	}
	u16							LL_BoneCount		()					{	return u16(bones->size());										}
	u16							LL_VisibleBoneCount()					{ return visimask.count();/*u64 F=visimask.flags&((u64(1)<<u64(LL_BoneCount()))-1); return (u16)btwCount1(F); */ }
	ICF Fmatrix&				LL_GetTransform		(u16 bone_id)		{	return LL_GetBoneInstance(bone_id).mTransform;					}
	ICF Fmatrix&				LL_GetTransform_R	(u16 bone_id)		{	return LL_GetBoneInstance(bone_id).mRenderTransform;			}	// rendering only
	Fobb&						LL_GetBox			(u16 bone_id)		{	VERIFY(bone_id<LL_BoneCount());	return (*bones)[bone_id]->obb;	}
	void						LL_GetBindTransform (xr_vector<Fmatrix>& matrices);
    int 						LL_GetBoneGroups 	(xr_vector<xr_vector<u16> >& groups);

	u16							LL_GetBoneRoot		()					{	return iRoot;													}
	void						LL_SetBoneRoot		(u16 bone_id)		{	VERIFY(bone_id<LL_BoneCount());	iRoot=bone_id;					}

    BOOL						LL_GetBoneVisible	(u16 bone_id)		{	VERIFY(bone_id<LL_BoneCount()); return visimask.is(bone_id);	}
	void						LL_SetBoneVisible	(u16 bone_id, BOOL val, BOOL bRecursive);
	VisMask						LL_GetBonesVisible	()					{	return visimask;	}
	void						LL_SetBonesVisible(/*u64 mask*/VisMask _visimask); //{	visimask = _visimask;};
	void						LL_SetBonesVisibleAll()					{ visimask.set_all(); };

	// Main functionality
	virtual void				CalculateBones				(BOOL bForceExact	=	FALSE);		// Recalculate skeleton
	void						CalculateBones_Invalidate	();
	void						Callback					(UpdateCallback C, void* Param)		{	Update_Callback	= C; Update_Callback_Param	= Param;	}

	// debug
#ifdef DEBUG
	void						DebugRender			(Fmatrix& XFORM);
#endif

	// General "Visual" stuff
    virtual void				Copy				(IRender_Visual *pFrom);
	virtual void				Load				(const char* N, IReader *data, u32 dwFlags);
	virtual void 				Spawn				();
	virtual void				Depart				();
    virtual void 				Release				();

	virtual	CKinematics*		dcast_PKinematics	()				{ return this;	}

	virtual u32					mem_usage			(bool bInstance)
	{
		u32 sz					= sizeof(*this);
		sz						+= bone_instances?bone_instances->mem_usage():0;
		if (!bInstance){
//			sz					+= pUserData?pUserData->mem_usage():0;
			for (vecBonesIt b_it=bones->begin(); b_it!=bones->end(); b_it++)
				sz				+= sizeof(vecBones::value_type)+(*b_it)->mem_usage();
		}
		return sz;
	}
};
IC CKinematics* PKinematics		(IRender_Visual* V)		{ return V?V->dcast_PKinematics():0; }
//---------------------------------------------------------------------------
#endif
