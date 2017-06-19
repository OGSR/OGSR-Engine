#include "stdafx.h"
#ifdef DEBUG
#include "ode_include.h"
#include "../StatGraph.h"
#include "PHDebug.h"
#endif
#include "alife_space.h"
#include "hit.h"
#include "PHDestroyable.h"
#include "car.h"
#include "../skeletoncustom.h"
#include "PHWorld.h"
extern CPHWorld*	ph_world;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCar::SExhaust::~SExhaust()
{
	
	CParticlesObject::Destroy(p_pgobject);
}

void CCar::SExhaust::Init()
{
	VERIFY(!ph_world->Processing());
	pelement=(bone_map.find(bone_id))->second.element;
	CKinematics* K=smart_cast<CKinematics*>(pcar->Visual());
	CBoneData&	bone_data=K->LL_GetData(u16(bone_id));
	transform.set(bone_data.bind_transform);
	///transform.mulA(pcar->XFORM());
	//Fmatrix element_transform;
	//pelement->InterpolateGlobalTransform(&element_transform);
	//element_transform.invert();
	//transform.mulA(element_transform);
	p_pgobject=CParticlesObject::Create(*pcar->m_exhaust_particles,FALSE);
	Fvector zero_vector;
	zero_vector.set(0.f,0.f,0.f);
	p_pgobject->UpdateParent(pcar->XFORM(), zero_vector );

}

void CCar::SExhaust::Update()
{
	VERIFY(!ph_world->Processing());
	Fmatrix global_transform;
	pelement->InterpolateGlobalTransform(&global_transform);
	global_transform.mulB_43(transform);
	dVector3 res;
	Fvector	 res_vel;
	dBodyGetPointVel(pelement->get_body(),global_transform.c.x,global_transform.c.y,global_transform.c.z,res);
	CopyMemory (&res_vel,res,sizeof(Fvector));
	//velocity.mul(0.95f);
	//res_vel.mul(0.05f);
	//velocity.add(res_vel);
	p_pgobject->UpdateParent(global_transform,res_vel);
}

void CCar::SExhaust::Clear()
{
	CParticlesObject::Destroy(p_pgobject);
}

void CCar::SExhaust::Play()
{
	VERIFY(!ph_world->Processing());
	p_pgobject->Play();
	Update();
}

void CCar::SExhaust::Stop()
{
	VERIFY(!ph_world->Processing());
	p_pgobject->Stop();
}