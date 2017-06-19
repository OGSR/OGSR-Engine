#include "stdafx.h"
#include "missile.h"
#include "WeaponHUD.h"
#include "PhysicsShell.h"
#include "actor.h"
#include "../CameraBase.h"
#include "xrserver_objects_alife.h"
#include "ActorEffector.h"
#include "level.h"
#include "xr_level_controller.h"
#include "../skeletoncustom.h"
#include "ai_object_location.h"
#include "ExtendedGeom.h"
#include "MathUtils.h"
#include "characterphysicssupport.h"
#include "inventory.h"
#include "../IGame_Persistent.h"
#ifdef DEBUG
#	include "phdebug.h"
#endif


#define PLAYING_ANIM_TIME 10000

#include "ui/UIProgressShape.h"
#include "ui/UIXmlInit.h"

CUIProgressShape* g_MissileForceShape = NULL;

void create_force_progress()
{
	VERIFY							(!g_MissileForceShape);
	CUIXml uiXml;
	bool xml_result					= uiXml.Init(CONFIG_PATH, UI_PATH, "grenade.xml");
	R_ASSERT3						(xml_result, "xml file not found", "grenade.xml");

	CUIXmlInit xml_init;
	g_MissileForceShape				= xr_new<CUIProgressShape>();
	xml_init.InitProgressShape		(uiXml, "progress", 0, g_MissileForceShape);
}

CMissile::CMissile(void) 
{
}

CMissile::~CMissile(void) 
{
	HUD_SOUND::DestroySound(sndPlaying);
}

void CMissile::reinit		()
{
	inherited::reinit	();
	m_throw				= false;
	m_constpower = false;
	m_fThrowForce		= 0;
	m_dwDestroyTime		= 0xffffffff;
	m_bPending			= false;
	m_fake_missile		= NULL;
	SetHUDmode			(FALSE);
	SetState			( MS_HIDDEN );
}

void CMissile::Load(LPCSTR section) 
{
	inherited::Load		(section);

	m_fMinForce			= pSettings->r_float(section,"force_min");
	m_fConstForce		= pSettings->r_float(section,"force_const");
	m_fMaxForce			= pSettings->r_float(section,"force_max");
	m_fForceGrowSpeed	= pSettings->r_float(section,"force_grow_speed");

	m_dwDestroyTimeMax	= pSettings->r_u32(section,"destroy_time");
	
	m_vThrowPoint		= pSettings->r_fvector3(section,"throw_point");
	m_vThrowDir			= pSettings->r_fvector3(section,"throw_dir");
	m_vHudThrowPoint	= pSettings->r_fvector3(*hud_sect,"throw_point");
	m_vHudThrowDir		= pSettings->r_fvector3(*hud_sect,"throw_dir");

	//загрузить анимации HUD-а
	m_sAnimShow			= pSettings->r_string(*hud_sect, "anim_show");
	m_sAnimHide			= pSettings->r_string(*hud_sect, "anim_hide");
	m_sAnimIdle			= pSettings->r_string(*hud_sect, "anim_idle");
	m_sAnimPlaying		= pSettings->r_string(*hud_sect, "anim_playing");
	m_sAnimThrowBegin	= pSettings->r_string(*hud_sect, "anim_throw_begin");
	m_sAnimThrowIdle	= pSettings->r_string(*hud_sect, "anim_throw_idle");
	m_sAnimThrowAct		= pSettings->r_string(*hud_sect, "anim_throw_act");
	m_sAnimThrowEnd		= pSettings->r_string(*hud_sect, "anim_throw_end");

	if(pSettings->line_exist(section,"snd_playing"))
		HUD_SOUND::LoadSound(section,"snd_playing",sndPlaying);

	m_ef_weapon_type	= READ_IF_EXISTS(pSettings,r_u32,section,"ef_weapon_type",u32(-1));
}

BOOL CMissile::net_Spawn(CSE_Abstract* DC) 
{
	BOOL l_res = inherited::net_Spawn(DC);

	dwXF_Frame					= 0xffffffff;

	m_throw_direction.set(0.0f, 1.0f, 0.0f);
	m_throw_matrix.identity();

	return l_res;
}

void CMissile::net_Destroy() 
{
	inherited::net_Destroy();
	m_fake_missile = 0;
}

void CMissile::OnActiveItem		()
{
	inherited::OnActiveItem	();
	SetState				( MS_IDLE );
	SetNextState			( MS_IDLE );	
	if (m_pHUD) m_pHUD->Show();
}

void CMissile::OnHiddenItem()
{
	inherited::OnHiddenItem	();
	if (m_pHUD) m_pHUD->Hide();
	SetState				( MS_HIDDEN );
	SetNextState			( MS_HIDDEN );
}


void CMissile::spawn_fake_missile()
{
	if (OnClient()) return;

	if (!getDestroy())
	{
		CSE_Abstract		*object = Level().spawn_item(
			*cNameSect(),
			Position(),
			(g_dedicated_server)?u32(-1):ai_location().level_vertex_id(),
			ID(),
			true
		);

		CSE_ALifeObject				*alife_object = smart_cast<CSE_ALifeObject*>(object);
		VERIFY						(alife_object);
		alife_object->m_flags.set	(CSE_ALifeObject::flCanSave,FALSE);

		NET_Packet			P;
		object->Spawn_Write	(P,TRUE);
		Level().Send		(P,net_flags(TRUE));
		F_entity_Destroy	(object);
	}
}

void CMissile::OnH_A_Chield() 
{
	inherited::OnH_A_Chield();

//	if(!m_fake_missile && !smart_cast<CMissile*>(H_Parent())) 
//		spawn_fake_missile	();
}


void CMissile::OnH_B_Independent(bool just_before_destroy) 
{
	inherited::OnH_B_Independent(just_before_destroy);

	m_pHUD->Hide();

	if(!m_dwDestroyTime && Local()) 
	{
		DestroyObject		();
		return;
	}
}

void CMissile::UpdateCL() 
{
	inherited::UpdateCL();

	if(GetState() == MS_IDLE && m_dwStateTime > PLAYING_ANIM_TIME) 
		OnStateSwitch(MS_PLAYING);
	
	if(GetState() == MS_READY) 
	{
		if(m_throw){ 
			SwitchState(MS_THROW);
		}else 
		{
			CActor	*actor = smart_cast<CActor*>(H_Parent());
			if (actor) {				
				m_fThrowForce		+= (m_fForceGrowSpeed * Device.dwTimeDelta) * .001f;
				clamp(m_fThrowForce, m_fMinForce, m_fMaxForce);
			}
		}
	}

}
void CMissile::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	if(!H_Parent() && getVisible() && m_pPhysicsShell) 
	{
		if(m_dwDestroyTime <= Level().timeServer()) 
		{
			m_dwDestroyTime = 0xffffffff;
			VERIFY	(!m_pCurrentInventory);
			Destroy	();
			return;
		}
	} 
}

void CMissile::StartIdleAnim()
{
	m_pHUD->animDisplay(m_pHUD->animGet(*m_sAnimIdle), TRUE);
}

void CMissile::State(u32 state) 
{
	switch(GetState()) 
	{
	case MS_SHOWING:
        {
			m_bPending = true;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimShow), FALSE, this, GetState());
		} break;
	case MS_IDLE:
		{
			m_bPending = false;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimIdle), TRUE, this, GetState());
		} break;
	case MS_HIDING:
		{
			m_bPending = true;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimHide), TRUE, this, GetState());
		} break;
	case MS_HIDDEN:
		{
			
			if (m_pHUD) 
			{
				m_pHUD->StopCurrentAnimWithoutCallback();
				m_pHUD->Hide();
			};
			
			if (H_Parent())
			{				
				setVisible(FALSE);
				setEnabled(FALSE);				
			};
			m_bPending = false;			
		} break;
	case MS_THREATEN:
		{
			m_bPending = true;
			m_fThrowForce = m_fMinForce;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimThrowBegin), TRUE, this, GetState());
		} break;
	case MS_READY:
		{
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimThrowIdle), TRUE, this, GetState());
		} break;
	case MS_THROW:
		{
			m_bPending = true;
			m_throw = false;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimThrowAct), TRUE, this, GetState());
		} break;
	case MS_END:
		{
			m_bPending = true;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimThrowEnd), TRUE, this, GetState());
		} break;
	case MS_PLAYING:
		{
			PlaySound(sndPlaying,Position());
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimPlaying), TRUE, this, GetState());
		} break;
	}
}

void CMissile::OnStateSwitch	(u32 S)
{
	inherited::OnStateSwitch	(S);
	State						(S);
}


void CMissile::OnAnimationEnd(u32 state) 
{
	switch(state) 
	{
	case MS_HIDING:
		{
			setVisible(FALSE);
			OnStateSwitch(MS_HIDDEN);
		} break;
	case MS_SHOWING:
		{
			setVisible(TRUE);
			OnStateSwitch(MS_IDLE);
		} break;
	case MS_THREATEN:
		{
			if (!m_fake_missile) {
				CMissile				*missile = smart_cast<CMissile*>(H_Parent());
				if (!missile) {
					CEntityAlive		*entity_alive = smart_cast<CEntityAlive*>(H_Parent());
					if (!entity_alive || entity_alive->g_Alive())
					{
						spawn_fake_missile	();
					}
				}
			}

			if(m_throw) 
				SwitchState(MS_THROW); 
//				OnStateSwitch(MS_THROW); 
			else 
				SwitchState(MS_READY);
//				OnStateSwitch(MS_READY);
		} break;
	case MS_THROW:
		{
			Throw();
			OnStateSwitch(MS_END);
		} break;
	case MS_END:
		{
			OnStateSwitch(MS_SHOWING);
		} break;
	case MS_PLAYING:
		{
			OnStateSwitch(MS_IDLE);
		} break;
	}
}


void CMissile::UpdatePosition(const Fmatrix& trans)
{
	XFORM().mul		(trans,offset());
}

void CMissile::UpdateXForm	()
{
	if (Device.dwFrame!=dwXF_Frame)
	{
		dwXF_Frame			= Device.dwFrame;

		if (0==H_Parent())	return;

		// Get access to entity and its visual
		CEntityAlive*		E		= smart_cast<CEntityAlive*>(H_Parent());
        
		if(!E)				return	;

		const CInventoryOwner	*parent = smart_cast<const CInventoryOwner*>(E);
		if (parent && parent->use_simplified_visual())
			return;

		if (parent->attached(this))
			return;

		VERIFY				(E);
		CKinematics*		V		= smart_cast<CKinematics*>	(E->Visual());
		VERIFY				(V);

		// Get matrices
		int					boneL,boneR,boneR2;
		E->g_WeaponBones	(boneL,boneR,boneR2);

		boneL = boneR2;

		V->CalculateBones	();
		Fmatrix& mL			= V->LL_GetTransform(u16(boneL));
		Fmatrix& mR			= V->LL_GetTransform(u16(boneR));

		// Calculate
		Fmatrix				mRes;
		Fvector				R,D,N;
		D.sub				(mL.c,mR.c);	D.normalize_safe();
		R.crossproduct		(mR.j,D);		R.normalize_safe();
		N.crossproduct		(D,R);			N.normalize_safe();
		mRes.set			(R,N,D,mR.c);
		mRes.mulA_43		(E->XFORM());
		UpdatePosition		(mRes);
	}
}


void CMissile::Show() 
{
	SwitchState(MS_SHOWING);
}

void CMissile::Hide() 
{
	if(IsGameTypeSingle())
		SwitchState(MS_HIDING);
	else
		SwitchState(MS_HIDDEN);
}

void CMissile::setup_throw_params()
{
	CEntity					*entity = smart_cast<CEntity*>(H_Parent());
	VERIFY					(entity);
	CInventoryOwner			*inventory_owner = smart_cast<CInventoryOwner*>(H_Parent());
	VERIFY					(inventory_owner);
	Fmatrix					trans;
	trans.identity			();
	Fvector					FirePos, FireDir;
	if (this == inventory_owner->inventory().ActiveItem())
	{
		CInventoryOwner* io		= smart_cast<CInventoryOwner*>(H_Parent());
		if(NULL == io->inventory().ActiveItem())
		{
				Log("current_state", GetState() );
				Log("next_state", GetNextState());
				Log("state_time", m_dwStateTime);
				Log("item_sect", cNameSect().c_str());
				Log("H_Parent", H_Parent()->cNameSect().c_str());
		}

		entity->g_fireParams(this, FirePos, FireDir);
	}else{
		FirePos				= XFORM().c;
		FireDir				= XFORM().k;
	}
	trans.k.set				(FireDir);
	Fvector::generate_orthonormal_basis(trans.k, trans.j,trans.i);
	trans.c.set				(FirePos);
	m_throw_matrix.set		(trans);
	m_throw_direction.set	(trans.k);
}

void CMissile::Throw() 
{
	VERIFY								(smart_cast<CEntity*>(H_Parent()));
	setup_throw_params					();
	
	m_fake_missile->m_throw_direction	= m_throw_direction;
	m_fake_missile->m_throw_matrix		= m_throw_matrix;
		
	CInventoryOwner						*inventory_owner = smart_cast<CInventoryOwner*>(H_Parent());
	VERIFY								(inventory_owner);
	if (inventory_owner->use_default_throw_force())
		m_fake_missile->m_fThrowForce	= m_constpower ? m_fConstForce : m_fThrowForce; 
	else
		m_fake_missile->m_fThrowForce	= inventory_owner->missile_throw_force(); 
	
	m_fThrowForce						= m_fMinForce;

	if (Local() && H_Parent()) 
	{
		NET_Packet						P;
		u_EventGen						(P,GE_OWNERSHIP_REJECT,ID());
		P.w_u16							(u16(m_fake_missile->ID()));
		u_EventSend						(P);
	}
}

void CMissile::OnEvent(NET_Packet& P, u16 type) 
{
	inherited::OnEvent		(P,type);
	u16						id;
	switch (type) {
		case GE_OWNERSHIP_TAKE : {
			P.r_u16(id);
			CMissile		*missile = smart_cast<CMissile*>(Level().Objects.net_Find(id));			
			m_fake_missile	= missile;
			missile->H_SetParent(this);
			missile->Position().set(Position());
			break;
		} 
		case GE_OWNERSHIP_REJECT : {
			P.r_u16			(id);
			bool IsFakeMissile = false;
			if (m_fake_missile && (id == m_fake_missile->ID()))
			{
				m_fake_missile	= NULL;
				IsFakeMissile = true;
			}

			CMissile		*missile = smart_cast<CMissile*>(Level().Objects.net_Find(id));
			if (!missile)
			{
				break;
			}
			missile->H_SetParent(0,!P.r_eof() && P.r_u8());
			if (IsFakeMissile && OnClient()) 
				missile->set_destroy_time(m_dwDestroyTimeMax);
			break;
		}
	}
}

void CMissile::Destroy() 
{
	if (Local())		DestroyObject();
}

bool CMissile::Action(s32 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;

	switch(cmd) 
	{
	case kWPN_FIRE:
		{
			m_constpower = true;			
			if(flags&CMD_START) 
			{
				m_throw = true;
				if(GetState() == MS_IDLE) 
					SwitchState(MS_THREATEN);
			} 
			return true;
		}break;

	case kWPN_ZOOM:
		{
			m_constpower = false;
        	if(flags&CMD_START) 
			{
				m_throw = false;
				if(GetState() == MS_IDLE) 
					SwitchState(MS_THREATEN);
				else if(GetState() == MS_READY)
				{
					m_throw = true; 
				}

			} 
			else if(GetState() == MS_READY || GetState() == MS_THREATEN || GetState() == MS_IDLE) 
			{
				m_throw = true; 
				if(GetState() == MS_READY) SwitchState(MS_THROW);
			}
			return true;
		}break;
	}
	return false;
}

void  CMissile::UpdateFireDependencies_internal	()
{
	if (0==H_Parent())		return;

    if (Device.dwFrame!=dwFP_Frame){
		dwFP_Frame = Device.dwFrame;

		UpdateXForm			();
		
		if (GetHUDmode() && !IsHidden()){
			// 1st person view - skeletoned
			CKinematics* V			= smart_cast<CKinematics*>(m_pHUD->Visual());
			VERIFY					(V);
			V->CalculateBones		();

			// fire point&direction
			Fmatrix& parent			= m_pHUD->Transform	();
			m_throw_direction.set	(parent.k);
		}else{
			// 3rd person
			Fmatrix& parent			= H_Parent()->XFORM();

			m_throw_direction.set	(m_vThrowDir);
			parent.transform_dir	(m_throw_direction);
		}
	}
}

void CMissile::activate_physic_shell()
{
	if (!smart_cast<CMissile*>(H_Parent())) {
		inherited::activate_physic_shell();
		if(m_pPhysicsShell&&m_pPhysicsShell->isActive()&&!IsGameTypeSingle())
		{
				m_pPhysicsShell->add_ObjectContactCallback		(ExitContactCallback);
				m_pPhysicsShell->set_CallbackData	(smart_cast<CPhysicsShellHolder*>(H_Root()));
		}
		return;
	}

	Fvector				l_vel;
	l_vel.set			(m_throw_direction);
	l_vel.normalize_safe();
	l_vel.mul			(m_fThrowForce);

	Fvector				a_vel;
	CInventoryOwner		*inventory_owner = smart_cast<CInventoryOwner*>(H_Root());
	if (inventory_owner && inventory_owner->use_throw_randomness()) {
		float			fi,teta,r;
		fi				= ::Random.randF(0.f,2.f*M_PI);
		teta			= ::Random.randF(0.f,M_PI);
		r				= ::Random.randF(2.f*M_PI,3.f*M_PI);
		float			rxy = r*_sin(teta);
		a_vel.set		(rxy*_cos(fi),rxy*_sin(fi),r*_cos(teta));
	}
	else
		a_vel.set		(0.f,0.f,0.f);

	XFORM().set			(m_throw_matrix);

	CEntityAlive		*entity_alive = smart_cast<CEntityAlive*>(H_Root());
	if (entity_alive && entity_alive->character_physics_support()){
		Fvector			parent_vel;
		entity_alive->character_physics_support()->movement()->GetCharacterVelocity(parent_vel);
		l_vel.add		(parent_vel);
	}

	VERIFY								(!m_pPhysicsShell);
	create_physic_shell					();
	m_pPhysicsShell->Activate			(m_throw_matrix, l_vel, a_vel);
//	m_pPhysicsShell->AddTracedGeom		();
	m_pPhysicsShell->SetAllGeomTraced	();
	m_pPhysicsShell->add_ObjectContactCallback		(ExitContactCallback);
	m_pPhysicsShell->set_CallbackData	(smart_cast<CPhysicsShellHolder*>(entity_alive));
//	m_pPhysicsShell->remove_ObjectContactCallback	(ExitContactCallback);
	m_pPhysicsShell->SetAirResistance	(0.f,0.f);
	m_pPhysicsShell->set_DynamicScales	(1.f,1.f);

	CKinematics							*kinematics = smart_cast<CKinematics*>(Visual());
	VERIFY								(kinematics);
	kinematics->CalculateBones_Invalidate();
	kinematics->CalculateBones			();
}
void	CMissile::net_Relcase(CObject* O)
{
	inherited::net_Relcase(O);
	if(PPhysicsShell()&&PPhysicsShell()->isActive())
	{
		if(O==smart_cast<CObject*>((CPhysicsShellHolder*)PPhysicsShell()->get_CallbackData()))
		{
			PPhysicsShell()->remove_ObjectContactCallback(ExitContactCallback);
			PPhysicsShell()->set_CallbackData(NULL);
		}
	}

}
void CMissile::create_physic_shell	()
{
	//create_box2sphere_physic_shell();
	CInventoryItemObject::CreatePhysicsShell();
}

void CMissile::setup_physic_shell	()
{
	VERIFY(!m_pPhysicsShell);
	create_physic_shell();
	m_pPhysicsShell->Activate	(XFORM(),0,XFORM());//,true 
	CKinematics					*kinematics = smart_cast<CKinematics*>(Visual());
	VERIFY						(kinematics);
	kinematics->CalculateBones_Invalidate();
	kinematics->CalculateBones			();
}

u32	CMissile::ef_weapon_type		() const
{
	VERIFY	(m_ef_weapon_type != u32(-1));
	return	(m_ef_weapon_type);
}


void CMissile::OnDrawUI()
{
	if(GetState()==MS_READY && !m_throw) 
	{
		CActor	*actor = smart_cast<CActor*>(H_Parent());
		if (actor) {
			if(!g_MissileForceShape) create_force_progress();
			float k = (m_fThrowForce-m_fMinForce)/(m_fMaxForce-m_fMinForce);
			g_MissileForceShape->SetPos	(k);
			g_MissileForceShape->Draw	();
		}
	}	
}

void	 CMissile::ExitContactCallback(bool& do_colide,bool bo1,dContact& c,SGameMtl * /*material_1*/,SGameMtl * /*material_2*/)
{
	dxGeomUserData	*gd1=NULL,	*gd2=NULL;
	if(bo1)
	{
		gd1 =retrieveGeomUserData(c.geom.g1);
		gd2 =retrieveGeomUserData(c.geom.g2);
	}
	else
	{
		gd2 =retrieveGeomUserData(c.geom.g1);
		gd1 =retrieveGeomUserData(c.geom.g2);
	}
	if(gd1&&gd2&&(CPhysicsShellHolder*)gd1->callback_data==gd2->ph_ref_object)	
																				do_colide=false;
}

void CMissile::GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count)
{
	str_name		= NameShort();
	str_count		= "";
	icon_sect_name	= "";
}

u16 CMissile::bone_count_to_synchronize	() const
{
	return CInventoryItem::object().PHGetSyncItemsNumber();
}