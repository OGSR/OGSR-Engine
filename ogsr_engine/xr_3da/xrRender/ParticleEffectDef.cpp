#include "stdafx.h"
#pragma hdrstop

#include "ParticleEffectDef.h"
#ifdef _EDITOR
	#include "UI_ToolsCustom.h"
	#include "ParticleEffectActions.h"
#endif
//---------------------------------------------------------------------------
using namespace PAPI;
using namespace PS;

//------------------------------------------------------------------------------
// class CParticleEffectDef
//------------------------------------------------------------------------------
CPEDef::CPEDef()
{                                          
    m_Frame.InitDefault	();
    m_MaxParticles		= 0;
	m_CachedShader		= 0;
	m_fTimeLimit		= 0.f;
    // collision
    m_fCollideOneMinusFriction 	= 1.f;
    m_fCollideResilience		= 0.f;
    m_fCollideSqrCutoff			= 0.f;
    // velocity scale
    m_VelocityScale.set			(0.f,0.f,0.f);
    // align to path
    m_APDefaultRotation.set		(-PI_DIV_2,0.f,0.f);
	// flags
    m_Flags.zero		();
}

CPEDef::~CPEDef()
{
#ifdef _EDITOR
	for (EPAVecIt it=m_EActionList.begin(); it!=m_EActionList.end(); it++) xr_delete(*it);
#endif
}
void CPEDef::CreateShader()
{
    if (*m_ShaderName&&*m_TextureName)	
        m_CachedShader.create(*m_ShaderName,*m_TextureName);
}
void CPEDef::DestroyShader()
{
    m_CachedShader.destroy();
}
void CPEDef::SetName(LPCSTR name)
{
    m_Name				= name;
}

/*
void CPEDef::pAlignToPath(float rot_x, float rot_y, float rot_z)
{
	m_Flags.set			(dfAlignToPath,TRUE);
	m_APDefaultRotation.set(rot_x,rot_y,rot_z);
}
void CPEDef::pVelocityScale(float scale_x, float scale_y, float scale_z)
{
	m_Flags.set			(dfVelocityScale,TRUE);
	m_VelocityScale.set	(scale_x, scale_y, scale_z);
}
void CPEDef::pCollision(float friction, float resilience, float cutoff, BOOL destroy_on_contact)
{
	m_fCollideOneMinusFriction 	= 1.f-friction;
	m_fCollideResilience		= resilience;
	m_fCollideSqrCutoff			= cutoff*cutoff;
	m_Flags.set					(dfCollision,TRUE);
	m_Flags.set					(dfCollisionDel,destroy_on_contact);
}

void CPEDef::pSprite(string128& sh_name, string128& tex_name)
{
	xr_free(m_ShaderName);	m_ShaderName	= xr_strdup(sh_name);
	xr_free(m_TextureName);	m_TextureName	= xr_strdup(tex_name);
	m_Flags.set	(dfSprite,TRUE);
}
void CPEDef::pFrame(BOOL random_frame, u32 frame_count, u32 tex_width, u32 tex_height, u32 frame_width, u32 frame_height)
{
	m_Flags.set			(dfFramed,TRUE);
	m_Flags.set			(dfRandomFrame,random_frame);
	m_Frame.Set			(frame_count, (float)tex_width, (float)tex_height, (float)frame_width, (float)frame_height);
}
void CPEDef::pAnimate(float speed, BOOL random_playback)
{
	m_Frame.m_fSpeed	= speed;
	m_Flags.set			(dfAnimated,TRUE);
	m_Flags.set			(dfRandomPlayback,random_playback);
}
void CPEDef::pTimeLimit(float time_limit)
{
	m_Flags.set			(dfTimeLimit,TRUE);
	m_fTimeLimit		= time_limit;
}
*/
void CPEDef::ExecuteAnimate(Particle *particles, u32 p_cnt, float dt)
{
	float speedFac = m_Frame.m_fSpeed * dt;
	for(u32 i = 0; i < p_cnt; i++){
		Particle &m = particles[i];
		float f						= (float(m.frame)/255.f+((m.flags.is(Particle::ANIMATE_CCW))?-1.f:1.f)*speedFac);
		if (f>m_Frame.m_iFrameCount)f-=m_Frame.m_iFrameCount;
		if (f<0.f)					f+=m_Frame.m_iFrameCount;
		m.frame						= (u16)iFloor(f*255.f);
	}
}

void CPEDef::ExecuteCollision(PAPI::Particle* particles, u32 p_cnt, float dt, CParticleEffect* owner, CollisionCallback cb)
{
	pVector pt,n;
	// Must traverse list in reverse order so Remove will work
	for(int i = p_cnt-1; i >= 0; i--){
		Particle &m = particles[i];

		bool pick_needed;
		int pick_cnt=0;
		do{		
			pick_needed = false;
			Fvector 	dir;
			dir.sub		(m.pos,m.posB);
			float dist 	= dir.magnitude();
			if (dist>=EPS){
				dir.div	(dist);
#ifdef _EDITOR                
				if (Tools->RayPick(m.posB,dir,dist,&pt,&n)){
#else
				collide::rq_result	RQ;
                collide::rq_target	RT = m_Flags.is(dfCollisionDyn)?collide::rqtBoth:collide::rqtStatic;
				if (g_pGameLevel->ObjectSpace.RayPick(m.posB,dir,dist,RT,RQ,NULL)){	
					pt.mad	(m.posB,dir,RQ.range);
					if (RQ.O){
						n.set(0.f,1.f,0.f);
					}else{
						CDB::TRI*	T		=  	g_pGameLevel->ObjectSpace.GetStaticTris()+RQ.element;
						Fvector*	verts	=	g_pGameLevel->ObjectSpace.GetStaticVerts();
						n.mknormal(verts[T->verts[0]],verts[T->verts[1]],verts[T->verts[2]]);
					}
#endif
					pick_cnt++;
					if (cb&&(pick_cnt==1)) if (!cb(owner,m,pt,n)) break;
					if (m_Flags.is(dfCollisionDel)){ 
	                   	ParticleManager()->RemoveParticle(owner->m_HandleEffect,i);
					}else{
						// Compute tangential and normal components of velocity
						float nmag = m.vel * n;
						pVector vn(n * nmag); 	// Normal Vn = (V.N)N
						pVector vt(m.vel - vn);	// Tangent Vt = V - Vn

						// Compute _new velocity heading out:
						// Don't apply friction if tangential velocity < cutoff
						if(vt.length2() <= m_fCollideSqrCutoff){
							m.vel = vt - vn * m_fCollideResilience;
						}else{
							m.vel = vt * m_fCollideOneMinusFriction - vn * m_fCollideResilience;
						}
						m.pos	= m.posB + m.vel * dt; 
						pick_needed = true;
					}
				}
			}else{
				m.pos	= m.posB;
			}
		}while(pick_needed&&(pick_cnt<2));
	}
}

//------------------------------------------------------------------------------
// I/O part
//------------------------------------------------------------------------------
BOOL CPEDef::Load(IReader& F)
{
	R_ASSERT		(F.find_chunk(PED_CHUNK_VERSION));
	u16 version		= F.r_u16();

	if (version!=PED_VERSION)
		return FALSE;

	R_ASSERT		(F.find_chunk(PED_CHUNK_NAME));
	F.r_stringZ		(m_Name);

	R_ASSERT		(F.find_chunk(PED_CHUNK_EFFECTDATA));
	m_MaxParticles	= F.r_u32();

	u32 sz			= F.find_chunk(PED_CHUNK_ACTIONLIST); R_ASSERT(sz);
	m_Actions.w		(F.pointer(),sz);

	F.r_chunk		(PED_CHUNK_FLAGS,&m_Flags);

	if (m_Flags.is(dfSprite)){
		R_ASSERT	(F.find_chunk(PED_CHUNK_SPRITE));
		F.r_stringZ	(m_ShaderName);
		F.r_stringZ	(m_TextureName);
	}

	if (m_Flags.is(dfFramed)){
		R_ASSERT	(F.find_chunk(PED_CHUNK_FRAME));
		F.r			(&m_Frame,sizeof(SFrame));
	}

	if (m_Flags.is(dfTimeLimit)){
		R_ASSERT(F.find_chunk(PED_CHUNK_TIMELIMIT));
		m_fTimeLimit= F.r_float();
	}

	if (m_Flags.is(dfCollision)){
		R_ASSERT(F.find_chunk(PED_CHUNK_COLLISION));
		m_fCollideOneMinusFriction 	= F.r_float();
		m_fCollideResilience		= F.r_float();
		m_fCollideSqrCutoff			= F.r_float();
	}

	if (m_Flags.is(dfVelocityScale)){
		R_ASSERT(F.find_chunk(PED_CHUNK_VEL_SCALE));
		F.r_fvector3				(m_VelocityScale); 
	}

	if (m_Flags.is(dfAlignToPath)){
		if (F.find_chunk(PED_CHUNK_ALIGN_TO_PATH)){
			F.r_fvector3			(m_APDefaultRotation);
		}
	}

#ifdef _EDITOR
	if (F.find_chunk(PED_CHUNK_OWNER)){
		AnsiString tmp;
		F.r_stringZ	(m_OwnerName);
		F.r_stringZ	(m_ModifName);
		F.r			(&m_CreateTime,sizeof(m_CreateTime));
		F.r			(&m_ModifTime,sizeof(m_ModifTime));
	}
    if (pCreateEAction&&F.find_chunk(PED_CHUNK_EDATA)){
        m_EActionList.resize(F.r_u32());
        for (EPAVecIt it=m_EActionList.begin(); it!=m_EActionList.end(); it++){
            PAPI::PActionEnum type = (PAPI::PActionEnum)F.r_u32();
            (*it)	= pCreateEAction(type);
            (*it)->Load		(F);
        }
    } 
#endif

	return TRUE;
}

void CPEDef::Save(IWriter& F)
{
	F.open_chunk	(PED_CHUNK_VERSION);
	F.w_u16			(PED_VERSION);
	F.close_chunk	();

	F.open_chunk	(PED_CHUNK_NAME);
	F.w_stringZ		(m_Name);
	F.close_chunk	();

	F.open_chunk	(PED_CHUNK_EFFECTDATA);
	F.w_u32			(m_MaxParticles);
	F.close_chunk	();

	F.open_chunk	(PED_CHUNK_ACTIONLIST);
	F.w				(m_Actions.pointer(),m_Actions.size());
	F.close_chunk	();

	F.w_chunk		(PED_CHUNK_FLAGS,&m_Flags,sizeof(m_Flags));

	if (m_Flags.is(dfSprite)){
		F.open_chunk	(PED_CHUNK_SPRITE);
		F.w_stringZ		(m_ShaderName);
		F.w_stringZ		(m_TextureName);
		F.close_chunk	();
	}

	if (m_Flags.is(dfFramed)){
		F.open_chunk	(PED_CHUNK_FRAME);
		F.w				(&m_Frame,sizeof(SFrame));
		F.close_chunk	();
	}

	if (m_Flags.is(dfTimeLimit)){
		F.open_chunk	(PED_CHUNK_TIMELIMIT);
		F.w_float		(m_fTimeLimit);
		F.close_chunk	();
	}

	if (m_Flags.is(dfCollision)){
		F.open_chunk	(PED_CHUNK_COLLISION);
		F.w_float		(m_fCollideOneMinusFriction);	
		F.w_float		(m_fCollideResilience);
		F.w_float		(m_fCollideSqrCutoff);
		F.close_chunk	();
	}

	if (m_Flags.is(dfVelocityScale)){
		F.open_chunk	(PED_CHUNK_VEL_SCALE);
		F.w_fvector3	(m_VelocityScale);
		F.close_chunk	();
	}

	if (m_Flags.is(dfAlignToPath)){
		F.open_chunk	(PED_CHUNK_ALIGN_TO_PATH);
		F.w_fvector3	(m_APDefaultRotation);
		F.close_chunk	();
	}
#ifdef _EDITOR
	F.open_chunk	(PED_CHUNK_OWNER);
	F.w_stringZ		(m_OwnerName);
	F.w_stringZ		(m_ModifName);
	F.w				(&m_CreateTime,sizeof(m_CreateTime));
	F.w				(&m_ModifTime,sizeof(m_ModifTime));
	F.close_chunk	();

	F.open_chunk	(PED_CHUNK_EDATA);
    F.w_u32			(m_EActionList.size());
    for (EPAVecIt it=m_EActionList.begin(); it!=m_EActionList.end(); it++){
        F.w_u32		((*it)->type);
    	(*it)->Save	(F);
    }
	F.close_chunk	();
#endif
}


