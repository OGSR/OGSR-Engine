////////////////////////////////////////////////////////////////////////////
//	Module 		: luabind_memory.h
//	Created 	: 24.06.2005
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : luabind memory
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef DEBUG
#	ifdef NDEBUG
        static_assert(false, "Do not define NDEBUG macros in DEBUG configuration since luabind classes are sensisitve to it")
#	endif // #ifdef NDEBUG
#endif // #ifdef DEBUG

#include <luabind/luabind_types.h>

namespace luabind {
	extern LUABIND_API	memory_allocation_function_pointer		allocator;
	extern LUABIND_API	memory_allocation_function_parameter	allocator_parameter;

	inline void* call_allocator	(void const* buffer, size_t const size)
	{
		return			(allocator(allocator_parameter, buffer, size));
	}
} // namespace luabind

#include <luabind/luabind_delete.h>
#include <luabind/luabind_memory_manager_generator.h>
