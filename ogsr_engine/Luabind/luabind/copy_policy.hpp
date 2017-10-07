// Copyright (c) 2003 Daniel Wallin and Arvid Norberg

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once
#include <lua.hpp>
#include <luabind/luabind.hpp>

#include <luabind/config.hpp>
#include <luabind/detail/policy.hpp>

#include <type_traits>

namespace luabind { namespace detail {

	struct copy_pointer_to
	{
		template<typename T>
		void apply(lua_State* L, const T* ptr)
		{
			if (ptr == 0) 
			{
				lua_pushnil(L);
				return;
			}

			class_registry* registry = class_registry::get_registry(L);

			class_rep* crep = registry->find_class(LUABIND_TYPEID(T));

			// if you get caught in this assert you are trying
			// to use an unregistered type
			assert(crep && "you are trying to use an unregistered type");

			T* copied_obj = luabind_new<T>(*ptr);

			// create the struct to hold the object
			void* obj = lua_newuserdata(L, sizeof(object_rep));
			// we send 0 as destructor since we know it will never be called
			new(obj) object_rep(copied_obj, crep, object_rep::owner, delete_s<T>::apply);

			// set the meta table
			detail::getref(L, crep->metatable_ref());
			lua_setmetatable(L, -2);
		}
	};

	struct copy_reference_to
	{
		template<typename T>
		void apply(lua_State* L, const T& ref)
		{
			class_registry* registry = class_registry::get_registry(L);
			class_rep* crep = registry->find_class(LUABIND_TYPEID(T));

			// if you get caught in this assert you are trying
			// to use an unregistered type
			assert(crep && "you are trying to use an unregistered type");

			T* copied_obj = luabind_new<T>(ref);

			// create the struct to hold the object
			void* obj = lua_newuserdata(L, sizeof(object_rep));
			// we send 0 as destructor since we know it will never be called
			new(obj) object_rep(copied_obj, crep, object_rep::owner, delete_s<T>::apply);

			// set the meta table
			detail::getref(L, crep->metatable_ref());
			lua_setmetatable(L, -2);
		}
	};

	template<size_t N>
	struct copy_policy : conversion_policy<N>
	{
		struct only_accepts_pointers_or_references {};
		struct only_converts_from_cpp_to_lua {};

		static void precall(lua_State*, const index_map&) {}
		static void postcall(lua_State*, const index_map&) {}

		template<typename T, Direction Dir>
		struct generate_converter
		{
            using type = std::conditional_t<
                Dir == Direction::cpp_to_lua,
                std::conditional_t<
                    std::is_pointer_v<T>,
                    copy_pointer_to,
                    std::conditional_t<
                        std::is_reference_v<T>,
                        copy_reference_to,
                        only_accepts_pointers_or_references
                    >
                >,
                only_converts_from_cpp_to_lua
            >;
		};
	};
}}

namespace luabind
{
	template<size_t N>
	detail::policy_cons<detail::copy_policy<N>> 
	copy() { return detail::policy_cons<detail::copy_policy<N>>(); }
}
