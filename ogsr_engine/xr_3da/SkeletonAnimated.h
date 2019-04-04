//---------------------------------------------------------------------------
#ifndef SkeletonAnimatedH
#define SkeletonAnimatedH

#include		"skeletoncustom.h"
#include		"animation.h"
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

struct IterateBlendsCallback
{
	virtual	void	operator () ( CBlend &B ) = 0;
};
struct IUpdateTracksCallback
{
	virtual	bool	operator () ( float dt, CKinematicsAnimated& k ) = 0;
};

struct SKeyTable
{
	CKey				keys						[MAX_CHANNELS][MAX_BLENDED];	//all keys 
	CBlend				*blends						[MAX_CHANNELS][MAX_BLENDED];	//blend pointers
	int					chanel_blend_conts			[MAX_CHANNELS]	;				//channel counts
	SKeyTable			()							{ std::fill_n( chanel_blend_conts, MAX_CHANNELS, 0 ); }
};

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
	BOOL			stop_at_end_callback;
	BOOL			stop_at_end;
	BOOL			fall_at_end;
	PlayCallback	Callback;
	void*			CallbackParam;
	
	u32				dwFrame;
	
	u32				mem_usage			(){ return sizeof(*this); }
IC	ECurvature		blend_state			(  )const { return blend ;}
IC	void			set_free_state		( ){ blend = eFREE_SLOT; }
IC	void			set_accrue_state	( ){ blend = eAccrue; }
IC	void			set_falloff_state	( ){ blend = eFalloff; }
IC	void			set					( const CBlend &r ){ *this = r; }

IC void CBlend::update_play( float dt, PlayCallback _Callback )
{

	float pow_dt = dt;
	if( pow_dt < 0.f )
	{
		pow_dt = 0;
		if( stop_at_end )
		{
			VERIFY( blendAccrue>0.f );
			pow_dt = timeCurrent + dt - 1.f/blendAccrue;
			clamp( pow_dt, dt, 0.f );
		}
	}
	
	blendAmount 		+= pow_dt*blendAccrue*blendPower;

	clamp				( blendAmount, 0.f, blendPower); 


	if( !update_time( dt ) )//reached end 
		return;

	if ( _Callback &&  stop_at_end_callback )	
		_Callback( this );		// callback only once

	stop_at_end_callback		= FALSE;

	if( fall_at_end )
	{
		blend = eFalloff;
		blendFalloff = 2.f;
		//blendAccrue = timeCurrent;
	}
	return ;
}

IC	bool CBlend::update_time			( float dt )
{
	if (!playing) 
			return false;
	float quant = dt*speed;
	timeCurrent += quant; // stop@end - time is not going

	bool	running_fwrd	=  ( quant > 0 );
	float	const pEND_EPS	=	SAMPLE_SPF+EPS;
	bool	at_end			=	running_fwrd && ( timeCurrent > ( timeTotal- pEND_EPS) );
	bool	at_begin		=	!running_fwrd && ( timeCurrent < 0.f );
	
	if( !stop_at_end )
	{
		if( at_begin )
			timeCurrent+= timeTotal;
		if( at_end )
			timeCurrent -= ( timeTotal- pEND_EPS);
		VERIFY( timeCurrent>=0.f );
		return false;
	}
	if( !at_end && !at_begin )
					return false;

	if( at_end )
	{
		timeCurrent	= timeTotal- pEND_EPS;		// stop@end - time frozen at the end
		if( timeCurrent<0.f ) timeCurrent =0.f; 
	}
	else
		timeCurrent	= 0.f;

	VERIFY( timeCurrent>=0.f );
	return true;
}

IC bool CBlend::update_falloff( float dt )
{
	update_time( dt );
	
	blendAmount 		-= dt*blendFalloff*blendPower;

	bool ret			= blendAmount<=0;
	clamp				( blendAmount, 0.f, blendPower);
	return ret;
}

IC bool CBlend::update( float dt, PlayCallback _Callback )
{
	switch (blend) 
	{
		case eFREE_SLOT: 
			NODEFAULT;
		case eAccrue:
			update_play( dt, _Callback );
			break;
		case eFalloff:
			if( update_falloff( dt ) )
				return true;
			break;
		default: 
			NODEFAULT;
	}
	return false;
}
};

class IBlendDestroyCallback
{
	public:
		virtual void BlendDestroy( CBlend& blend )	= 0;
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
	void						LL_BuldBoneMatrixDequatize	( const CBoneData* bd, u8 channel_mask,  SKeyTable& keys );
	void						LL_BoneMatrixBuild			( CBoneInstance &bi, const Fmatrix *parent, const SKeyTable& keys );
private:
	
virtual	void					BuildBoneMatrix				( const CBoneData* bd, CBoneInstance &bi, const Fmatrix *parent, u8 mask_channel = (1<<0) );
public:

	virtual void				OnCalculateBones		();
private:
	u32											Update_LastTime;

	CBlendInstance*								blend_instances;

	struct SMotionsSlot{
		shared_motions							motions;
		BoneMotionsVec							bone_motions;
	};
	DEFINE_VECTOR(SMotionsSlot,MotionsSlotVec,MotionsSlotVecIt);
	MotionsSlotVec								m_Motions;

    CPartition*									m_Partition;

	IBlendDestroyCallback						*m_blend_destroy_callback;
	IUpdateTracksCallback						*m_update_tracks_callback;
	// Blending
	svector<CBlend, MAX_BLENDED_POOL>			blend_pool;
	BlendSVec									blend_cycles[MAX_PARTS];
	BlendSVec									blend_fx;
	animation::channels							channels;
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
	u32							LL_PartBlendsCount			( u32 bone_part_id );
	CBlend						*LL_PartBlend				( u32 bone_part_id, u32 n );
	void						LL_IterateBlends			( IterateBlendsCallback &callback );

	void						SetUpdateTracksCalback		( IUpdateTracksCallback	*callback );
	IUpdateTracksCallback		*GetUpdateTracksCalback		( ){ return m_update_tracks_callback; }

	u16							LL_MotionsSlotCount(){return (u16)m_Motions.size();}
	const shared_motions&		LL_MotionsSlot	(u16 idx){return m_Motions[idx].motions;}

	IC CMotionDef*				LL_GetMotionDef	(MotionID id){return m_Motions[id.slot].motions.motion_def(id.idx);}
	IC CMotion*					LL_GetRootMotion(MotionID id){return &m_Motions[id.slot].bone_motions[iRoot]->at(id.idx);}
	IC CMotion*					LL_GetMotion	(MotionID id, u16 bone_id){return &m_Motions[id.slot].bone_motions[bone_id]->at(id.idx);}

	virtual IBlendDestroyCallback	*GetBlendDestroyCallback	( );
	virtual void					SetBlendDestroyCallback		( IBlendDestroyCallback	*cb );
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
	void						LL_UpdateTracks	( float dt, bool b_force, bool leave_blends );						// Update motions
	void						LL_UpdateFxTracks( float dt );
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
								CKinematicsAnimated		();

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

	virtual float				get_animation_length (MotionID motion_ID);
};
IC CKinematicsAnimated* PKinematicsAnimated(IRender_Visual* V) { return V?V->dcast_PKinematicsAnimated():0; }
//---------------------------------------------------------------------------
#endif
