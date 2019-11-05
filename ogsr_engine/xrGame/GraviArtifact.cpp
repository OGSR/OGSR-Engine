///////////////////////////////////////////////////////////////
// GraviArtifact.cpp
// GraviArtefact - гравитационный артефакт, прыгает на месте
// и неустойчиво парит над землей
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GraviArtifact.h"
#include "PhysicsShell.h"
#include "level.h"
#include "xrmessages.h"
#include "game_cl_base.h"
#include "../Include/xrRender/Kinematics.h"
#include "phworld.h"

extern CPHWorld*	ph_world;
#define CHOOSE_MAX(x,inst_x,y,inst_y,z,inst_z)\
	if(x>y)\
		if(x>z){inst_x;}\
		else{inst_z;}\
	else\
		if(y>z){inst_y;}\
		else{inst_z;}


CGraviArtefact::CGraviArtefact(void) 
{
	shedule.t_min = 20;
	shedule.t_max = 50;
	
	m_fJumpHeight = 0;
	m_fEnergy = 1.f;
	m_jump_min_height = 0;
}

CGraviArtefact::~CGraviArtefact(void) 
{
}

void CGraviArtefact::Load(LPCSTR section) 
{
	inherited::Load(section);

	if(pSettings->line_exist(section, "jump_height")) m_fJumpHeight = pSettings->r_float(section,"jump_height");
//	m_fEnergy = pSettings->r_float(section,"energy");
	m_jump_min_height = READ_IF_EXISTS( pSettings, r_float, section, "jump_min_height", 0.f );
	m_jump_under_speed = READ_IF_EXISTS( pSettings, r_float, section, "jump_under_speed", 500.f );
	m_jump_raise_speed = READ_IF_EXISTS( pSettings, r_float, section, "jump_raise_speed", 50.f );
	m_jump_keep_speed = READ_IF_EXISTS( pSettings, r_float, section, "jump_keep_speed", 25.f );
	m_jump_time = READ_IF_EXISTS( pSettings, r_u32, section, "jump_time", 0 ) * 1000;
}



void CGraviArtefact::UpdateCLChild() 
{

	VERIFY(!ph_world->Processing());
	if (getVisible() && m_pPhysicsShell) {
	  if (m_fJumpHeight) {
	    if ( m_jump_min_height && !CPHUpdateObject::IsActive() ) {
	      CPHUpdateObject::Activate();
	      process_gravity();
	    }
	    process_jump();
	  }
	} else 
		if(H_Parent()) 
		{
			XFORM().set(H_Parent()->XFORM());
		};
}


BOOL CGraviArtefact::net_Spawn( CSE_Abstract* DC ) {
  BOOL result = inherited::net_Spawn( DC );

  if ( result ) {
    m_jump_jump = m_keep = m_raise = false;
    m_jump_time_end = 0;
  }

  return result;
}


void CGraviArtefact::PhDataUpdate( dReal step ) {
  inherited::PhDataUpdate( step );
  if ( m_activationObj || !( getVisible() && m_pPhysicsShell ) )
    return;
  process_gravity();
}


void CGraviArtefact::OnH_B_Independent( bool just_before_destroy ) {
  inherited::OnH_B_Independent( just_before_destroy );
  m_jump_jump = m_keep = m_raise = false;
  if ( m_jump_time )
    m_jump_time_end = Device.dwTimeGlobal + m_jump_time;
}


void CGraviArtefact::process_gravity() {
  Fvector dir = { 0, -1, 0 };
  Fvector P = Position();
  P.y += Radius();
  collide::rq_result RQ;
  Fbox level_box = Level().ObjectSpace.GetBoundingVolume();
  bool res = Level().ObjectSpace.RayPick( P, dir, _abs( P.y - level_box.y1 ) + 1.f, collide::rqtBoth, RQ, this );
  float raise_speed = m_jump_raise_speed;
  if ( !res ) {
    m_jump_jump = false;
    m_keep  = false;
    m_raise = true;
    raise_speed = m_jump_under_speed;
  }
  else {
    dir.y = -1.f;
    // проверить высоту артефакта
    float range = RQ.range - Radius();
    if ( m_jump_min_height && res && range < m_jump_min_height ) {
      m_jump_jump = false;
      m_keep  = false;
      m_raise = true;
    }
    else {
      if ( m_jump_min_height && fsimilar( m_fJumpHeight, m_jump_min_height ) ) {
        m_jump_jump = false;
        if ( m_keep ) {
          if ( res && fsimilar( range, m_keep_height, 0.01f ) )
            dir.y = 0;
          else
            dir.y = -m_jump_keep_speed;
        }
        else if ( m_raise ) {
          m_keep = true;
          m_keep_height = range;
          dir.y = 0;
        }
      }
      else {
        m_jump_jump = ( range < m_fJumpHeight );
        m_keep = false;
        if ( m_jump_min_height && m_jump_time ) {
          if ( m_raise )
            m_jump_time_end = Device.dwTimeGlobal + m_jump_time;
          else if ( m_jump_time_end < Device.dwTimeGlobal ) {
            m_jump_jump = false;
            m_keep = true;
            dir.y = -m_jump_keep_speed;
          }            
        }
      }
      m_raise = false;
    }
  }

  if ( m_raise || m_keep ) {
    if ( m_pPhysicsShell->get_ApplyByGravity() )
      m_pPhysicsShell->set_ApplyByGravity( FALSE );
    Fvector vel;
    m_pPhysicsShell->get_LinearVel( vel );
    vel.y = 0.f;
    if ( !fsimilar( m_fJumpHeight, m_jump_min_height ) )
      vel.x = vel.z = 0.f;
    m_pPhysicsShell->set_LinearVel( vel );
    if ( m_raise ) dir.y = raise_speed;
    m_pPhysicsShell->applyGravityAccel( dir );
  }
  else if ( !m_pPhysicsShell->get_ApplyByGravity() )
    m_pPhysicsShell->set_ApplyByGravity( TRUE );
}


void CGraviArtefact::process_jump() {
  Fvector dir = { 0, -1.f, 0 };
  if ( !m_jump_min_height ) {
    collide::rq_result RQ;
    // проверить высоту артефакта
    Fvector P = Position();
    P.y += Radius();
    if ( Level().ObjectSpace.RayPick( P, dir, m_fJumpHeight + Radius(), collide::rqtBoth, RQ, this ) )
      m_jump_jump = true;
    else
      m_jump_jump = false;
  }
  if ( m_jump_jump ) {
    dir.y = 1.f;
    m_pPhysicsShell->applyImpulse( dir, 30.f * Device.fTimeDelta * m_pPhysicsShell->getMass() );
  }
}
