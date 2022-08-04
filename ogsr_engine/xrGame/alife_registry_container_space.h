////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_registry_container_space.h
//	Created 	: 01.07.2004
//  Modified 	: 01.07.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife registry container space
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Utils/imdexlib/typelist.hpp>

#define registry_type_list imdexlib::typelist<>
#define add_to_registry_type_list(a) using registry_##a = imdexlib::ts_prepend_t<a, registry_type_list>;
#define define_constant(a) (a*)nullptr
#define save_registry_type_list(a) registry_##a
