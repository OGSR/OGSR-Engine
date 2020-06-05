#include "stdafx.h"
#include "pda.h"
#include "hudmanager.h"
#include "PhysicsShell.h"
#include "Entity.h"
#include "actor.h"

#include "xrserver.h"
#include "xrServer_Objects_ALife_Items.h"
#include "level.h"

#include "specific_character.h"
#include "alife_registry_wrappers.h"
#include "UIGameSP.h"
#include "ui/UIPDAWnd.h"


CPda::CPda(void)						
{										
	SetSlot( PDA_SLOT );
	m_flags.set				(Fruck, TRUE);

	m_idOriginalOwner		= u16(-1);
	m_SpecificChracterOwner = NULL;

	
	TurnOff					();
}

CPda::~CPda() 
{}

BOOL CPda::net_Spawn(CSE_Abstract* DC) 
{
	inherited::net_Spawn		(DC);
	CSE_Abstract				*abstract = (CSE_Abstract*)(DC);
	CSE_ALifeItemPDA			*pda = smart_cast<CSE_ALifeItemPDA*>(abstract);
	R_ASSERT					(pda);
	m_idOriginalOwner			= pda->m_original_owner;
	m_SpecificChracterOwner		= pda->m_specific_character;

	return						(TRUE);
}

void CPda::net_Destroy() 
{
	inherited::net_Destroy		();
	TurnOff						();
	feel_touch.clear			();
}

void CPda::Load(LPCSTR section) 
{
	inherited::Load(section);

	m_fRadius = pSettings->r_float(section,"radius");
}

void CPda::shedule_Update(u32 dt)	
{
	inherited::shedule_Update	(dt);

	if(!H_Parent()) return;
	Position().set	(H_Parent()->Position());

	if( IsOn() && Level().CurrentEntity() && Level().CurrentEntity()->ID()==H_Parent()->ID() )
	{
		CEntityAlive* EA = smart_cast<CEntityAlive*>(H_Parent());
		if(!EA || !EA->g_Alive())
		{
			TurnOff();
			return;
		}

                m_changed = false;
		feel_touch_update(Position(),m_fRadius);
		UpdateActiveContacts	();

                if ( m_changed ) {
                  if ( HUD().GetUI() ) {
                    CUIGameSP* pGameSP = smart_cast<CUIGameSP*>( HUD().GetUI()->UIGame() );
                    if ( pGameSP )
                      pGameSP->PdaMenu->PdaContentsChanged( pda_section::contacts );
                  }
                  m_changed = false;
                }
	}
}

void CPda::UpdateActiveContacts	()
{
	if ( !m_changed ) {
          for ( auto& it : m_active_contacts ) {
            CEntityAlive* pEA = smart_cast<CEntityAlive*>( it );
            if ( !pEA->g_Alive() ) {
              m_changed = true;
              break;
            }
          }
	}

	m_active_contacts.clear_not_free();
	xr_vector<CObject*>::iterator it= feel_touch.begin();
	for(;it!=feel_touch.end();++it){
		CEntityAlive* pEA = smart_cast<CEntityAlive*>(*it);
		if(!!pEA->g_Alive())
			m_active_contacts.push_back(*it);
	}
}

void CPda::feel_touch_new(CObject* O) 
{
	CInventoryOwner* pNewContactInvOwner	= smart_cast<CInventoryOwner*>(O);
	CInventoryOwner* pOwner					= smart_cast<CInventoryOwner*>( H_Parent() );VERIFY(pOwner);

	pOwner->NewPdaContact					(pNewContactInvOwner);
        m_changed = true;
}

void CPda::feel_touch_delete(CObject* O) 
{
	if(!H_Parent())							return;
	CInventoryOwner* pLostContactInvOwner	= smart_cast<CInventoryOwner*>(O);
	CInventoryOwner* pOwner					= smart_cast<CInventoryOwner*>( H_Parent() );VERIFY(pOwner);

	pOwner->LostPdaContact					(pLostContactInvOwner);
        m_changed = true;
}

BOOL CPda::feel_touch_contact(CObject* O) 
{
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(O);
	if(pInvOwner){
		if( this!=pInvOwner->GetPDA() )
		{
			CEntityAlive* pEntityAlive = smart_cast<CEntityAlive*>(O);
			if(pEntityAlive && !pEntityAlive->cast_base_monster() )
				return TRUE;
		}else
		return FALSE;
	}

	return FALSE;
}

void CPda::OnH_A_Chield() 
{
	VERIFY(IsOff());

	//включить PDA только если оно находится у первого владельца
	if(H_Parent()->ID() == m_idOriginalOwner){
		TurnOn					();
		if(m_sFullName.empty()){
			m_sFullName.assign(inherited::Name());
			m_sFullName += " ";
			m_sFullName += (smart_cast<CInventoryOwner*>(H_Parent()))->Name();
		}
	};
	inherited::OnH_A_Chield		();
}

void CPda::OnH_B_Independent(bool just_before_destroy) 
{
	inherited::OnH_B_Independent(just_before_destroy);
	
	//выключить
	TurnOff();
}


CInventoryOwner* CPda::GetOriginalOwner()
{
	CObject* pObject =  Level().Objects.net_Find(GetOriginalOwnerID());
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(pObject);

	return pInvOwner;
}



xr_map<u16, CPda*> CPda::ActivePDAContacts()
{
	xr_map<u16, CPda*> res;

	for (auto* Obj : m_active_contacts)
		if (CPda* p = GetPdaFromOwner(Obj))
			res.emplace( Obj->ID(), p );

	return res;
}

void CPda::save(NET_Packet &output_packet)
{
	inherited::save	(output_packet);
	save_data		(m_sFullName, output_packet);
}

void CPda::load(IReader &input_packet)
{
	inherited::load	(input_packet);
	load_data		(m_sFullName, input_packet);
}

CObject* CPda::GetOwnerObject()
{
	return				Level().Objects.net_Find(GetOriginalOwnerID());
}
LPCSTR		CPda::Name				()
{
	if( !m_SpecificChracterOwner.size() )
		return inherited::Name();

	if(m_sFullName.empty())
	{
		m_sFullName.assign(inherited::Name());
		
		CSpecificCharacter spec_char;
		spec_char.Load(m_SpecificChracterOwner);
		m_sFullName += " ";
		m_sFullName += xr_string(spec_char.Name());
	}
	
	return m_sFullName.c_str();
}

CPda* CPda::GetPdaFromOwner(CObject* owner)
{
	return smart_cast<CInventoryOwner*>(owner)->GetPDA			();
}


void CPda::TurnOn() {
  m_bTurnedOff = false;
  m_changed    = true;
}


void CPda::TurnOff() {
  m_bTurnedOff = true;
  m_active_contacts.clear();
}


void CPda::net_Relcase( CObject *O ) {
  inherited::net_Relcase( O );
  if ( m_active_contacts.size() && !Level().is_removing_objects() ) {
      const auto I = std::find( m_active_contacts.begin(), m_active_contacts.end(), O );
      if ( I != m_active_contacts.end() )
        m_active_contacts.erase( I );
  }
}
