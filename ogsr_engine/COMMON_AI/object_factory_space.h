////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory_space.h
//	Created 	: 30.06.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory space
////////////////////////////////////////////////////////////////////////////

#pragma once

class CSE_Abstract;

namespace ObjectFactory
{
typedef DLL_Pure CLIENT_BASE_CLASS;
typedef CSE_Abstract SERVER_BASE_CLASS;

typedef DLL_Pure CLIENT_SCRIPT_BASE_CLASS;
typedef CSE_Abstract SERVER_SCRIPT_BASE_CLASS;
}; // namespace ObjectFactory
