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
#include <luabind/detail/policy_cons.hpp>

namespace luabind { namespace detail
{
	class object_rep;

	template<typename R, typename C, typename T, typename... Policies>
	int get(R(C::*f)() const, T* obj, lua_State* L, const policy_cons<Policies...> policies)
	{ 
		return returns<R>::call(f, obj, L, policies); 
	}

	template<typename R, typename T, typename U, typename... Policies>
	int get(R(*f)(T), U* obj, lua_State* L, const policy_cons<Policies...> policies)
	{ 
		return returns<R>::call(f, obj, L, policies); 
	}

	template<typename T, typename F, typename... Policies>
	struct get_caller
	{
		get_caller() {}

		int operator()(lua_State* L, int pointer_offset, F f)
		{
			// parameters on the lua stack:
			// 1. object_rep
			// 2. key (property name)
			return get(f, static_cast<T*>(nullptr), L, policy_cons<Policies...>());
		}
	};

	template<typename R, typename C, typename T, typename A1, typename... Policies>
	int set(R(C::*f)(A1), T* obj, lua_State* L, const policy_cons<Policies...> policies)
	{ 
		return returns<void>::call(f, obj, L, policies); 
	}

	template<typename R, typename T, typename U, typename A1, typename... Policies>
	int set(R(*f)(T, A1), U* obj, lua_State* L, const policy_cons<Policies...> policies) { return returns<void>::call(f, obj, L, policies); }

	template<typename T, typename F, typename... Policies>
	struct set_caller
	{
		int operator()(lua_State* L, int pointer_offset, F f)
		{
			// parameters on the lua stack:
			// 1. object_rep
			// 2. key (property name)
			// 3. value

			// and since call() expects it's first
			// parameter on index 2 we need to
			// remove the key-parameter (parameter 2).
			lua_remove(L, 2);
			return luabind::detail::set(f, static_cast<T*>(nullptr), L, policy_cons<Policies...>());
		}
	};

	typedef int (*match_fun_ptr)(lua_State*, int);

	template<typename T, typename... Policies>
	struct set_matcher
	{
		static int apply(lua_State* L, int index)
		{
		    using converter_policy = typename find_conversion_policy<1, Policies...>::type;
			typedef typename converter_policy::template generate_converter<T, Direction::lua_to_cpp>::type converter;
			return converter::match(L, LUABIND_DECORATE_TYPE(T), index);
		}
	};

	template<typename T, typename Param, typename... Policies>
	match_fun_ptr gen_set_matcher(void (*)(T, Param), const policy_cons<Policies...>)
	{
		return set_matcher<Param, Policies...>::apply;
	}

    template<typename T, typename Param, typename... Policies>
	match_fun_ptr gen_set_matcher(void (T::*)(Param), const policy_cons<Policies...>)
	{
		return set_matcher<Param, Policies...>::apply;
	}

	// TODO: add support for policies
	template<typename T, typename D, typename... Policies>
	struct auto_set
	{
		auto_set() {}

		int operator()(lua_State* L, int pointer_offset, D T::*member)
		{
			const int nargs = lua_gettop(L);

			// parameters on the lua stack:
			// 1. object_rep
			// 2. key (property name)
			// 3. value
			object_rep* obj = static_cast<object_rep*>(lua_touserdata(L, 1));
			class_rep* crep = obj->crep();

			void* raw_ptr;

			if (crep->has_holder())
				raw_ptr = crep->extractor()(obj->ptr());
			else
				raw_ptr = obj->ptr();

			T* ptr =  reinterpret_cast<T*>(static_cast<char*>(raw_ptr) + pointer_offset);

			typedef typename find_conversion_policy<1, Policies...>::type converter_policy;
			typename converter_policy::template generate_converter<D, Direction::lua_to_cpp>::type converter;
			ptr->*member = converter.apply(L, LUABIND_DECORATE_TYPE(D), 3);

			const int nret = lua_gettop(L) - nargs;

			const int indices[] = { 1, nargs + nret, 3 };

			policy_list_postcall<Policies...>::apply(L, indices);

			return nret;
		}
	};

	// TODO: add support for policies
	template<typename T, typename D, typename... Policies>
	struct auto_get
	{
		auto_get() {}

		int operator()(lua_State* L, int pointer_offset, D T::*member)
		{
			const int nargs = lua_gettop(L);

			// parameters on the lua stack:
			// 1. object_rep
			// 2. key (property name)
			object_rep* obj = static_cast<object_rep*>(lua_touserdata(L, 1));
			class_rep* crep = obj->crep();

			void* raw_ptr;

			if (crep->has_holder())
				raw_ptr = crep->extractor()(obj->ptr());
			else
				raw_ptr = obj->ptr();

			T* ptr =  reinterpret_cast<T*>(static_cast<char*>(raw_ptr) + pointer_offset);

			typedef typename find_conversion_policy<0, Policies...>::type converter_policy;
			typename converter_policy::template generate_converter<D, Direction::cpp_to_lua>::type converter;
			converter.apply(L, ptr->*member);

			const int nret = lua_gettop(L) - nargs;

			const int indices[] = { 1, nargs + nret };

			policy_list_postcall<Policies...>::apply(L, indices);

			return nret;
		}
	};

}}
