////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restrictor_inline.h
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restrictor inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CSpaceRestrictor::CSpaceRestrictor		()
{
	b_scheduled             = false;
	m_space_restrictor_type = RestrictionSpace::eRestrictorTypeNone;
}

IC	bool CSpaceRestrictor::actual			() const
{
	return							(m_actuality);
}

IC RestrictionSpace::ERestrictorTypes CSpaceRestrictor::restrictor_type	() const
{
	return RestrictionSpace::ERestrictorTypes(m_space_restrictor_type);
}


IC void CSpaceRestrictor::change_restrictor_type ( RestrictionSpace::ERestrictorTypes restrictor_type ) {
  m_space_restrictor_type = u8( restrictor_type );
}
