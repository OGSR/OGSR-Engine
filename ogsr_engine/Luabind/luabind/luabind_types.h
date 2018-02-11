////////////////////////////////////////////////////////////////////////////
//	Module 		: luabind_types.h
//	Created 	: 18.03.2007
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : luabind types
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>

#include <luabind/luabind_memory_allocator.h>

namespace luabind {
	typedef void* memory_allocation_function_parameter;
	typedef void* (__cdecl *memory_allocation_function_pointer) (memory_allocation_function_parameter parameter, void const *, size_t);

	template <typename T, typename allocator = memory_allocator<T>>
	using internal_vector = std::vector<T, allocator>;

	template <typename T, typename allocator = memory_allocator<T>>
	using internal_list = std::list<T, allocator>;

	template <typename K, class P = std::less<K>, typename allocator = memory_allocator<K>>
	using internal_set = std::set<K, P, allocator>;

	template <typename K, class P = std::less<K>, typename allocator = memory_allocator<K>>
	using internal_multiset = std::multiset<K, P, allocator>;

	template <typename K, class V, class P = std::less<K>, typename allocator = memory_allocator<std::pair<const K, V>>>
	using internal_map = std::map<K, V, P, allocator>;

	template <typename K, class V, class P = std::less<K>, typename allocator = memory_allocator<std::pair<const K, V>>>
	using internal_multimap = std::multimap<K, V, P, allocator>;

	using internal_string = std::basic_string<char, std::char_traits<char>, memory_allocator<char>>;
} // namespace luabind
