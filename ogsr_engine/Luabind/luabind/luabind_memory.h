////////////////////////////////////////////////////////////////////////////
//	Module 		: luabind_memory.h
//	Created 	: 24.06.2005
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : luabind memory
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace luabind {
	typedef void* memory_allocation_function_parameter;
	typedef void* (__cdecl* memory_allocation_function_pointer) (memory_allocation_function_parameter parameter, void const*, size_t);

	extern LUABIND_API	memory_allocation_function_pointer		allocator;
	extern LUABIND_API	memory_allocation_function_parameter	allocator_parameter;

	inline void* call_allocator	(void const* buffer, size_t const size)
	{
		return			(allocator(allocator_parameter, buffer, size));
	}
} // namespace luabind

#include <luabind/luabind_types.h>
#include <luabind/luabind_delete.h>
#include <luabind/luabind_memory_manager_generator.h>
