////////////////////////////////////////////////////////////////////////////
//	Module 		: script_value_container_impl.h
//	Created 	: 16.07.2004
//  Modified 	: 16.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Script value container
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_broker.h"

IC CScriptValueContainer::~CScriptValueContainer() { clear(); }

IC void CScriptValueContainer::add(CScriptValue* new_value) {}

IC void CScriptValueContainer::assign() {}

IC void CScriptValueContainer::clear() { delete_data(m_values); }
