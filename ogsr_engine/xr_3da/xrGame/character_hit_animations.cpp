#include "stdafx.h"

#include "character_hit_animations.h"

#include "entity_alive.h"
#ifdef DEBUG
#include "phdebug.h"
#endif

void character_hit_animation_controller::SetupHitMotions(CKinematicsAnimated &ca)
{
	//CKinematicsAnimated* ca = smart_cast<CKinematicsAnimated*>(m_EntityAlife.Visual());
	/*
	bkhit_motion= ca.LL_MotionID("hitback");	//hitback2.skl
	fvhit_motion= ca.LL_MotionID("hitfront");
	rthit_motion= ca.LL_MotionID("hitright");
	lthit_motion= ca.LL_MotionID("hitleft");
*/

	bkhit_motion= ca.LL_MotionID("hitback17");	//hitback2.skl
	fvhit_motion= ca.LL_MotionID("hitfront17");
	rthit_motion= ca.LL_MotionID("hitf_right17");//hitright
	lthit_motion= ca.LL_MotionID("hitf_left17");//hitleft


	turn_right	= ca.LL_MotionID("hit_right_shoulder17");
	turn_left	= ca.LL_MotionID("hit_left_shoulder17");

	all_shift_down = ca.LL_MotionID("hitf_down17");
	hit_downl	   = ca.LL_MotionID("hit_downl");
	hit_downr	   = ca.LL_MotionID("hit_downr");

	base_bone	= ca.LL_BoneID("bip01_spine1");//bip01_spine1
	for( u16 i = 0; num_anims>i; ++i )
		block_times[i] = 0;

}
ICF int sign(float x)
{
	return x < 0 ? -1 : 1;
}

IC void set_blend_params(CBlend *B)
{
	if(!B)
		return;
	B->blendAmount = 1.0;
}

IC void	play_cycle(CKinematicsAnimated* CA,const MotionID &m,u8 channel,u32 &time_block,float base_power)
{
	const BOOL mixin = TRUE;
	const u32  dellay = 1;
	const u32  dellay1 = 100;
	float power = base_power;
	if(Device.dwTimeGlobal>time_block)
	{
		CBlend*	B = (CA->PlayCycle(m,mixin,0,0,channel)) ;
		
		if(Device.dwTimeGlobal<time_block+dellay1)
			power *= 0.5f;
		B->blendAmount = power;
		B->blendPower = power;	
		time_block = Device.dwTimeGlobal+dellay;
	}
}

void character_hit_animation_controller::PlayHitMotion(const Fvector &dir,const Fvector &bone_pos, u16 bi, CEntityAlive &ea)const
{
	CKinematicsAnimated* CA = smart_cast<CKinematicsAnimated*>(ea.Visual());
	
	//play_cycle(CA,all_shift_down,1,block_times[6],1) ;
	if( !(CA->LL_BoneCount() > bi) )
		return;

	Fvector dr = dir;
	Fmatrix m;
	GetBaseMatrix(m,ea);

#ifdef DEBUG
	if(ph_dbg_draw_mask1.test(phDbgHitAnims))
	{
		DBG_OpenCashedDraw();
		DBG_DrawLine(m.c,Fvector().sub(m.c,Fvector().mul(dir,1.5)),D3DCOLOR_XRGB(255,0,255));
		DBG_ClosedCashedDraw(1000);
	}
#endif
	const float power_factor	=2.f;// 2.f;
	m.invert();
	m.transform_dir(dr);
//
	Fvector hit_point;
	CA->LL_GetTransform(bi).transform_tiny(hit_point,bone_pos);
	ea.XFORM().transform_tiny(hit_point);
	m.transform_tiny(hit_point);
	Fvector torqu;		
	torqu.crossproduct(dr,hit_point);
	hit_point.x = 0;
	float rotational_ammount = hit_point.magnitude()*power_factor*3;//_abs(torqu.x)
	
	if(torqu.x<0)
		play_cycle(CA,hit_downr,2,block_times[6],1) ;
	else
		play_cycle(CA,hit_downl,2,block_times[6],1) ;

	if(!IsEffected(bi,*CA))
		return;
	if(torqu.x<0)
		play_cycle(CA,turn_right,1,block_times[4],rotational_ammount) ;
	
	else
		play_cycle(CA,turn_left,1,block_times[5],rotational_ammount) ;

	//CA->LL_SetChannelFactor(3,rotational_ammount);

	dr.x = 0;
	dr.normalize_safe();

	const float side_secretive_threshold = 0.2f;
	dr.mul(power_factor);
	if(dr.y>side_secretive_threshold)
		play_cycle(CA,rthit_motion,1,block_times[0],_abs(dr.y)) ;
	else if(dr.y<-side_secretive_threshold)
		play_cycle(CA,lthit_motion,1,block_times[1],_abs(dr.y)) ;

	if(dr.z<0.f)
		play_cycle(CA,fvhit_motion,1,block_times[2],_abs(dr.z)) ;
	else
		play_cycle(CA,bkhit_motion,1,block_times[3],_abs(dr.z)) ;
	CA->LL_SetChannelFactor(1,3.f);
	//CA->LL_SetChannelFactor(1,_abs(dr.z));
	//CA->LL_SetChannelFactor(2,_abs(dr.y));

	//BOOL bMixIn=TRUE, PlayCallback Callback=0, LPVOID CallbackParam=0, u8 channal = 0
	//CA->PlayCycle(hit_motion,TRUE,0,0,1) ;
	//CA->PlayCycle(hit_motion,TRUE,0,0,2) ;
	//CA->PlayCycle(hit_motion,TRUE,0,0,3) ;
	//const float fade = 0.1f;
	//for(u16 ii=0;MAX_PARTS>ii;++ii)
	//CA->LL_FadeCycle(ii,fade,1<<1);
	//smart_cast<CKinematicsAnimated*>(m_EntityAlife.Visual())->LL_MotionID("actor_hit_ani_180_2");
}

bool character_hit_animation_controller::IsEffected( u16	bi, CKinematics &ca )const
{
	u16 root = ca.LL_GetBoneRoot();
	for(;bi != root;)
	{
		CBoneData &bd	= ca.LL_GetData(bi);
		if(bi == base_bone)
			return true;
		bi = bd.GetParentID();
	}
	return false;
}

void character_hit_animation_controller::GetBaseMatrix( Fmatrix &m,CEntityAlive &ea)const
{
	CKinematics* CA = smart_cast<CKinematics*>(ea.Visual());
	m.mul_43(ea.XFORM(),CA->LL_GetTransform(base_bone));
}
