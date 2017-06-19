#include "stdafx.h"
#include "phcollisiondamagereceiver.h"
#include "PhysicsShellHolder.h"
#include "xr_ini.h"
#include "../skeletoncustom.h"
#include "geometry.h"
#include "PhysicsShell.h"
#include "GameMtlLib.h"
#include "Physics.h"
#include "../../xrNetServer/net_utils.h"
#include "xrMessages.h"
#include "CharacterPhysicsSupport.h"
void CPHCollisionDamageReceiver::BoneInsert(u16 id,float k)
{
	R_ASSERT2(FindBone(id)==m_controled_bones.end(),"duplicate bone!");
	m_controled_bones.push_back(SControledBone(id,k));
}
void CPHCollisionDamageReceiver::Init()
{
	CPhysicsShellHolder *sh	=PPhysicsShellHolder	();
	CKinematics			*K	=smart_cast<CKinematics*>(sh->Visual());
	CInifile			*ini=K->LL_UserData();
	if(ini->section_exist("collision_damage"))
	{
		
		CInifile::Sect& data		= ini->r_section("collision_damage");
		for (CInifile::SectCIt I=data.Data.begin(); I!=data.Data.end(); I++){
			const CInifile::Item& item	= *I;
			u16 index				= K->LL_BoneID(*item.first); 
			R_ASSERT3(index != BI_NONE, "Wrong bone name", *item.first);
			BoneInsert(index,float(atof(*item.second)));
			CODEGeom* og= sh->PPhysicsShell()->get_GeomByID(index);
			//R_ASSERT3(og, "collision damage bone has no physics collision", *item.first);
			if(og)og->add_obj_contact_cb(CollisionCallback);
		}
		
	}
}




void CPHCollisionDamageReceiver::CollisionCallback(bool& do_colide,bool bo1,dContact& c,SGameMtl* material_1,SGameMtl* material_2)
{
	if(material_1->Flags.test(SGameMtl::flPassable)||material_2->Flags.test(SGameMtl::flPassable))return;
	dBodyID						b1					=	dGeomGetBody(c.geom.g1)	;
	dBodyID						b2					=	dGeomGetBody(c.geom.g2) ;
	dxGeomUserData				*ud_self			=	bo1 ? retrieveGeomUserData(c.geom.g1):retrieveGeomUserData(c.geom.g2);
	dxGeomUserData				*ud_damager			=	bo1 ? retrieveGeomUserData(c.geom.g2):retrieveGeomUserData(c.geom.g1);
	
	SGameMtl					*material_self		=	bo1 ? material_1:material_2;
	SGameMtl					*material_damager	=	bo1 ? material_2:material_1;
	VERIFY						(ud_self);
	CPhysicsShellHolder			*o_self			=	ud_self->ph_ref_object;
	CPhysicsShellHolder			*o_damager		=	NULL;if(ud_damager)o_damager=ud_damager->ph_ref_object;
	u16							source_id		=	o_damager ? o_damager->ID():u16(-1);
	CPHCollisionDamageReceiver	*dr	=o_self->PHCollisionDamageReceiver();
	VERIFY2(dr,"wrong callback");
	
	float damager_material_factor=material_damager->fBounceDamageFactor;

	if(ud_damager&&ud_damager->ph_object&&ud_damager->ph_object->CastType()==CPHObject::tpCharacter)
	{
		CCharacterPhysicsSupport* phs=o_damager->character_physics_support();
		if(phs->IsSpecificDamager())damager_material_factor=phs->BonceDamageFactor();
	}

	float dfs=(material_self->fBounceDamageFactor+damager_material_factor);
	if(fis_zero(dfs)) return;
	Fvector dir;dir.set(*(Fvector*)c.geom.normal);
	Fvector pos;
	pos.sub(*(Fvector*)c.geom.pos,*(Fvector*)dGeomGetPosition(bo1 ? c.geom.g1:c.geom.g2));//it is not true pos in bone space
	dr->Hit(source_id,ud_self->bone_id,E_NL(b1,b2,c.geom.normal)*damager_material_factor/dfs,dir,pos);
	
}

const static float hit_threthhold=5.f;
void CPHCollisionDamageReceiver::Hit(u16 source_id,u16 bone_id,float power,const Fvector& dir,Fvector &pos )
{

	DAMAGE_BONES_I i=FindBone(bone_id);
	if(i==m_controled_bones.end())return;
	power*=i->second;
	if(power<hit_threthhold)return;
	
	NET_Packet		P;
	CPhysicsShellHolder *ph=PPhysicsShellHolder();
	SHit	HS;
	
	HS.GenHeader(GE_HIT,ph->ID());					//	ph->u_EventGen(P,GE_HIT,ph->ID());
	HS.whoID			= ph->ID();					//	P.w_u16		(ph->ID());
	HS.weaponID			= source_id;				//	P.w_u16		(source_id);
	HS.dir				= dir;						//	P.w_dir		(dir);
	HS.power			= power;					//	P.w_float	(power);
	HS.boneID			= s16(bone_id);				//	P.w_s16		(s16(bone_id));
	HS.p_in_bone_space	= pos;						//	P.w_vec3	(pos);
	HS.impulse			= 0.f;						//	P.w_float	(0.f);
	HS.hit_type			= (ALife::eHitTypeStrike);	//	P.w_u16		(ALife::eHitTypeStrike);
	HS.Write_Packet(P);						

	ph->u_EventSend(P);
}

void CPHCollisionDamageReceiver::Clear()
{
	//CPhysicsShellHolder *sh	=PPhysicsShellHolder	();
	//xr_map<u16,float>::iterator i=m_controled_bones.begin(),e=m_controled_bones.end();
	//for(;e!=i;++i)
	//{
	//	CODEGeom* og= sh->PPhysicsShell()->get_GeomByID(i->first);
	//	if(og)og->set_obj_contact_cb(NULL);
	//}
		m_controled_bones.clear();
}