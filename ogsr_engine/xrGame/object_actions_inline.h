////////////////////////////////////////////////////////////////////////////
//	Module 		: object_actions_inline.h
//	Created 	: 12.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object actions inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_handler.h"

//////////////////////////////////////////////////////////////////////////
// CObjectActionBase
//////////////////////////////////////////////////////////////////////////

template <typename _item_type>
IC	CObjectActionBase<_item_type>::CObjectActionBase(_item_type *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name) :
	inherited				(owner,action_name),
	m_item					(item)
{
	m_storage				= storage;
}

template <typename _item_type>
IC	void CObjectActionBase<_item_type>::set_property	(_condition_type condition_id, _value_type value)
{
	VERIFY					(m_storage);
	m_storage->set_property	(condition_id,value);
}

template <typename _item_type>
void CObjectActionBase<_item_type>::initialize	()
{
	inherited::initialize	();
	set_property			(ObjectHandlerSpace::eWorldPropertyAimed1,false);
	set_property			(ObjectHandlerSpace::eWorldPropertyAimed2,false);
}

template <typename _item_type>
IC	CAI_Stalker &CObjectActionBase<_item_type>::object	() const
{
	VERIFY					(m_object);
	return					(*m_object);
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionMember
//////////////////////////////////////////////////////////////////////////

template <typename _item_type>
IC	CObjectActionMember<_item_type>::CObjectActionMember	(_item_type *item, CAI_Stalker *owner, CPropertyStorage *storage, _condition_type condition_id, _value_type value, LPCSTR action_name) :
	inherited				(item,owner,storage,action_name),
	m_condition_id			(condition_id),
	m_value					(value)
{
}

template <typename _item_type>
void CObjectActionMember<_item_type>::execute			()
{
	inherited::execute		();
	if (completed())
		set_property		(m_condition_id,m_value);
}
