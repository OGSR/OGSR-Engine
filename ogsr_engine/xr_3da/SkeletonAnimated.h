//---------------------------------------------------------------------------
#ifndef SkeletonAnimatedH
#define SkeletonAnimatedH

#include		"skeletoncustom.h"
#include		"skeletonmotions.h"

// consts
const	u32		MAX_BLENDED			=	16;
const	u32		MAX_BLENDED_POOL	=	(MAX_BLENDED*MAX_PARTS*MAX_CHANNELS);
const	u32		MAX_ANIM_SLOT		=	4;

// refs
class   ENGINE_API CBlend;
class 	ENGINE_API CKinematicsAnimated;
class   ENGINE_API CBoneInstanceAnimated;
struct	ENGINE_API CKey;
class	ENGINE_API CInifile;

struct MotionID {
private:
	typedef const MotionID *(MotionID::*unspecified_bool_type) () const;
public:
	union{
    	struct{
    	    u16		idx:14;
	        u16		slot:2;
        };
        u16			val;
    };
public:
    				MotionID	(){invalidate();}
    				MotionID	(u16 motion_slot, u16 motion_idx){set(motion_slot,motion_idx);}
	ICF bool		operator==	(const MotionID& tgt) const {return tgt.val==val;}
	ICF bool		operator!=	(const MotionID& tgt) const {return tgt.val!=val;}
	ICF bool		operator<	(const MotionID& tgt) const {return val<tgt.val;}
	ICF	bool		operator!	() const					{return !valid();}
    ICF void		set			(u16 motion_slot, u16 motion_idx){slot=motion_slot; idx=motion_idx;}
	ICF void		invalidate	() {val=u16(-1);}
    ICF bool		valid		() const {return val!=u16(-1);}
	const MotionID*	get			() const {return this;};
	ICF	operator	unspecified_bool_type () const 
	{
		if (valid())	return &MotionID::get;
		else			return 0;
//		return(!valid()?0:&MotionID::get);
	}
};

//. typedef u16 	BoneID;
//. typedef u16 	PartitionID;

//*** Run-time Blend definition *******************************************************************
class ENGINE_API CBlend {
public:
	enum ECurvature
	{
		eFREE_SLOT=0,
//		eFixed,
		eAccrue,
		eFalloff,
		eFORCEDWORD = u32(-1)
	};
public:
	float			blendAmount;
	float			timeCurrent;
	float			timeTotal;
    MotionID		motionID;
	u16				bone_or_part;	// startup parameters
	u8				channel;
	ECurvature		blend;
	float			blendAccrue;	// increasing
	float			blendFalloff;	// decreasing
	float			blendPower;			
	float			speed;

	BOOL			playing;
	BOOL			stop_at_end;
	BOOL			fall_at_end;
	PlayCallback	Callback;
	void*			CallbackParam;
	
	u32				dwFrame;

	u32				mem_usage			(){ return sizeof(*this); }
};
typedef svector<CBlend*,MAX_BLENDED*MAX_CHANNELS>	BlendSVec;//*MAX_CHANNELS
typedef BlendSVec::iterator							BlendSVecIt;
typedef BlendSVec::const_iterator					BlendSVecCIt;
//*** Bone Instance *******************************************************************************
#pragma pack(push,8)
class ENGINE_API		CBlendInstance	// Bone Instance Blend List (per-bone data)
{
public:
	typedef svector<CBlend*,MAX_BLENDED>	BlendSVec;
	typedef BlendSVec::iterator				BlendSVecIt;
	typedef BlendSVec::const_iterator		BlendSVecCIt;
private:
	BlendSVec			Blend;
public:

	// methods
IC	BlendSVec			&blend_vector	()	{ return Blend;}
	void				construct		();
	void				blend_add		(CBlend* H);
	void				blend_remove	(CBlend* H);

	u32					mem_usage		()
	{
		u32 sz			= sizeof(*this);
		for (BlendSVecIt it=Blend.begin(); it!=Blend.end(); it++)
			sz			+= (*it)->mem_usage();
		return			sz;
	}
};
#pragma pack(pop)

//*** The visual itself ***************************************************************************
class ENGINE_API	CKinematicsAnimated	: public CKinematics
{
	typedef CKinematics							inherited;
	friend class								CBoneData;
	friend class								CMotionDef;
	friend class								CSkeletonX;
private:
	// Motion control
	void						Bone_Motion_Start		(CBoneData* bd, CBlend* handle);	// with recursion
	void						Bone_Motion_Stop		(CBoneData* bd, CBlend* handle);	// with recursion

	void						Bone_Motion_Start_IM	(CBoneData* bd, CBlend* handle);
	void						Bone_Motion_Stop_IM		(CBoneData* bd, CBlend* handle);

public: 
	// Calculation
private:
			void				BoneChain_Calculate		(const CBoneData* bd, CBoneInstance &bi,u8 channel_mask, bool ignore_callbacks);
			void				CLBone					(const CBoneData* bd, CBoneInstance &bi, const Fmatrix *parent,const CBlendInstance::BlendSVec &Blend, u8 mask_channel = (1<<0));
public:
	virtual void				Bone_Calculate			(CBoneData* bd, Fmatrix* parent);
			void				Bone_GetAnimPos			(Fmatrix& pos,u16 id, u8 channel_mask, bool ignore_callbacks);
	virtual void				OnCalculateBones		();
public: 
#ifdef _EDITOR
public:
#else
private:
#endif
	u32											Update_LastTime;

	CBlendInstance*								blend_instances;

	struct SMotionsSlot{
		shared_motions							motions;
		BoneMotionsVec							bone_motions;
	};
	DEFINE_VECTOR(SMotionsSlot,MotionsSlotVec,MotionsSlotVecIt);
	MotionsSlotVec								m_Motions;

    CPartition*									m_Partition;

	// Blending
	svector<CBlend, MAX_BLENDED_POOL>			blend_pool;
	BlendSVec									blend_cycles[MAX_PARTS];
	BlendSVec									blend_fx;
	float										channel_factors[MAX_CHANNELS];
protected:
	// internal functions
	virtual void				IBoneInstances_Create	();
	virtual void				IBoneInstances_Destroy	();

	void						IBlend_Startup			();
	void						ChannelFactorsStartup	();
	CBlend*						IBlend_Create			();
private:
	void						IBlendSetup				(CBlend& B,u16 part,u8 channel, MotionID motion_ID, BOOL  bMixing, float blendAccrue, float blendFalloff, float Speed, BOOL noloop, PlayCallback Callback, LPVOID CallbackParam);
	void						IFXBlendSetup			(CBlend &B, MotionID motion_ID, float blendAccrue, float blendFalloff,float Power ,float Speed,u16 bone);
//.	bool						LoadMotions				(LPCSTR N, IReader *data);
public:
#ifdef DEBUG
	std::pair<LPCSTR,LPCSTR>	LL_MotionDefName_dbg	(MotionID	ID);
//	LPCSTR						LL_MotionDefName_dbg	(LPVOID		ptr);
#endif
#ifdef _EDITOR
    u32							LL_CycleCount	(){u32 cnt=0; for (u32 k=0; k<m_Motions.size(); k++) cnt+=m_Motions[k].motions.cycle()->size(); return cnt;}
    u32							LL_FXCount		(){u32 cnt=0; for (u32 k=0; k<m_Motions.size(); k++) cnt+=m_Motions[k].motions.fx()->size(); return cnt;}
	accel_map*					LL_Motions		(u32 slot){return m_Motions[slot].motions.motion_map();}
	MotionID					ID_Motion		(LPCSTR  N, u16 slot);
#endif
	u16							LL_MotionsSlotCount(){return (u16)m_Motions.size();}
	const shared_motions&		LL_MotionsSlot	(u16 idx){return m_Motions[idx].motions;}

	IC CMotionDef*				LL_GetMotionDef	(MotionID id){return m_Motions[id.slot].motions.motion_def(id.idx);}
	IC CMotion*					LL_GetRootMotion(MotionID id){return &m_Motions[id.slot].bone_motions[iRoot]->at(id.idx);}
	IC CMotion*					LL_GetMotion	(MotionID id, u16 bone_id){return &m_Motions[id.slot].bone_motions[bone_id]->at(id.idx);}

	// Low level interface
	MotionID					LL_MotionID		(LPCSTR B);
	u16							LL_PartID		(LPCSTR B);

	CBlend*						LL_PlayFX		(u16 bone,		MotionID motion, float blendAccrue,	float blendFalloff, float Speed, float Power);
	CBlend*						LL_PlayCycle	(u16 partition, MotionID motion, BOOL  bMixing,		float blendAccrue,	float blendFalloff, float Speed, BOOL noloop, PlayCallback Callback, LPVOID CallbackParam, u8 channel = 0);
	CBlend*						LL_PlayCycle	(u16 partition, MotionID motion, BOOL bMixIn, PlayCallback Callback, LPVOID CallbackParam, u8 channel = 0);
	void						LL_FadeCycle	(u16 partition, float	falloff, u8 mask_channel = (1<<0));
	void						LL_CloseCycle	(u16 partition, u8 mask_channel = (1<<0));
	void						LL_SetChannelFactor (u16 channel,float factor);
	CBlendInstance&				LL_GetBlendInstance	(u16 bone_id)	{	VERIFY(bone_id<LL_BoneCount()); return blend_instances[bone_id];	}
	                                                                
	// Main functionality
	void						UpdateTracks	();								// Update motions
	void						DestroyCycle	(CBlend &B);

	// cycles
	MotionID					ID_Cycle		(LPCSTR  N);
	MotionID					ID_Cycle_Safe	(LPCSTR  N);
	MotionID					ID_Cycle		(shared_str  N);
	MotionID					ID_Cycle_Safe	(shared_str  N);
	CBlend*						PlayCycle		(LPCSTR  N,  BOOL bMixIn=TRUE, PlayCallback Callback=0, LPVOID CallbackParam=0, u8 channel = 0);
	CBlend*						PlayCycle		(MotionID M, BOOL bMixIn=TRUE, PlayCallback Callback=0, LPVOID CallbackParam=0, u8 channel = 0);
	// fx'es
	MotionID					ID_FX			(LPCSTR  N);
	MotionID					ID_FX_Safe		(LPCSTR  N);
	CBlend*						PlayFX			(LPCSTR  N, float power_scale);
	CBlend*						PlayFX			(MotionID M, float power_scale);

	// General "Visual" stuff
	virtual void				Copy			(IRender_Visual *pFrom);
	virtual void				Load			(const char* N, IReader *data, u32 dwFlags);
	virtual void				Release			();
	virtual void				Spawn			();
	virtual	CKinematicsAnimated*	dcast_PKinematicsAnimated	()				{ return this;	}
	virtual						~CKinematicsAnimated	();

	virtual u32					mem_usage		(bool bInstance)
	{
		u32 sz					= CKinematics::mem_usage(bInstance)+sizeof(*this)+(bInstance&&blend_instances?blend_instances->mem_usage():0);
		return sz;
	}

	IC	const BlendSVec			&blend_cycle	(const u32 &bone_part_id) const
	{
		VERIFY					(bone_part_id < MAX_PARTS);
		return					(blend_cycles[bone_part_id]);
	}
};
IC CKinematicsAnimated* PKinematicsAnimated(IRender_Visual* V) { return V?V->dcast_PKinematicsAnimated():0; }
//---------------------------------------------------------------------------
#endif
