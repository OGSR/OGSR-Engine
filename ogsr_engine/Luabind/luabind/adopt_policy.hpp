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

#include <luabind/config.hpp>
#include <luabind/detail/policy.hpp>
#include <luabind/detail/implicit_cast.hpp>

namespace luabind { namespace detail 
{
	template<Direction Dir = Direction::lua_to_cpp>
	struct adopt_pointer
	{
		template<typename T>
		T* apply(lua_State* L, by_pointer<T>, int index)
		{
			// preconditions:
			//	lua_isuserdata(L, index);
			// getmetatable().__lua_class is true
			// object_rep->flags() & object_rep::constant == 0

			int offset = 0;
			object_rep* obj = static_cast<object_rep*>(lua_touserdata(L, index));
			assert((obj != nullptr) && "internal error, please report");
			const class_rep* crep = obj->crep();

			int steps = implicit_cast(crep, LUABIND_TYPEID(T), offset);
			(void)steps;

			assert((steps >= 0) && "adopt_pointer used with type that cannot be converted");
			obj->remove_ownership();
			T* ptr = reinterpret_cast<T*>(obj->ptr(offset));

			return ptr;
		}

		template<typename T>
		static int match(lua_State* L, by_pointer<T>, int index)
		{
			object_rep* obj = is_class_object(L, index);
			if (obj == nullptr) return -1;
			// cannot cast a constant object to nonconst
			if (obj->flags() & object_rep::constant) return -1;
			if (!(obj->flags() & object_rep::owner)) return -1;
			int d;
			return implicit_cast(obj->crep(), LUABIND_TYPEID(T), d);	
		}

		template<typename T>
		void converter_postcall(lua_State*, T, int) {}
	};

	template<>
	struct adopt_pointer<Direction::cpp_to_lua>
	{
		template<typename T>
		void apply(lua_State* L, T* ptr)
		{
			if (ptr == nullptr)
			{
				lua_pushnil(L);
				return;
			}

			// if there is a back_reference, then the
			// ownership will be removed from the
			// back reference and put on the lua stack.
			if (back_reference<T>::move(L, ptr))
			{
				object_rep* obj = static_cast<object_rep*>(lua_touserdata(L, -1));
				obj->set_flags(obj->flags() | object_rep::owner);
				return;
			}

			class_registry* registry = class_registry::get_registry(L);
			class_rep* crep = registry->find_class(LUABIND_TYPEID(T));

/*			// create the struct to hold the object
			void* obj = lua_newuserdata(L, sizeof(object_rep));
			// we send 0 as destructor since we know it will never be called
			new(obj) object_rep(ptr, crep, object_rep::owner, delete_s<T>::apply);*/

			void* obj;
			void* held;

			std::tie(obj,held) = crep->allocate(L);

			new(obj) object_rep(ptr, crep, object_rep::owner, delete_s<T>::apply);

			// set the meta table
			detail::getref(L, crep->metatable_ref());
			lua_setmetatable(L, -2);
		}
	};

	template<size_t N>
	struct adopt_policy : conversion_policy<N>
	{
		static void precall(lua_State*, const index_map&) {}
		static void postcall(lua_State*, const index_map&) {}

		struct only_accepts_nonconst_pointers {};

		template<typename T, Direction Dir>
		struct generate_converter
		{
            using type = std::conditional_t<
                is_nonconst_pointer<T>::value,
                adopt_pointer<Dir>,
                only_accepts_nonconst_pointers
            >;
		};
	};

}}

namespace luabind
{
	template<size_t N>
	detail::policy_cons<detail::adopt_policy<N>> 
	adopt() { return detail::policy_cons<detail::adopt_policy<N>>(); }
}
