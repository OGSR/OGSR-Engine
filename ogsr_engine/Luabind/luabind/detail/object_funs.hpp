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

#include <Utils/imdexlib/option.hpp>

#include <luabind/config.hpp>
#include <luabind/detail/policy.hpp>
#include <luabind/error.hpp>
#include <luabind/detail/convert_to_lua.hpp>
#include <luabind/detail/debug.hpp>
#include <luabind/detail/stack_utils.hpp>

namespace luabind
{

	namespace detail
	{

		template<typename T, typename Obj, typename... Policies>
		T object_cast_impl(const Obj& obj, const policy_cons<Policies...>)
		{
			if (obj.lua_state() == 0) 
			{
#ifndef LUABIND_NO_EXCEPTIONS
				throw cast_failed(nullptr, LUABIND_TYPEID(T));
#else
				lua_State* L = obj.lua_state();
				cast_failed_callback_fun e = get_cast_failed_callback();
				if (e) e(L, LUABIND_TYPEID(T));

				assert(0 && "object_cast failed. If you want to handle this error use luabind::set_error_callback()");
				std::terminate();
#endif
			}

			LUABIND_CHECK_STACK(obj.lua_state());

		    using converter_policy = typename detail::find_conversion_policy<0, Policies...>::type;
			typename converter_policy::template generate_converter<T, Direction::lua_to_cpp>::type converter;

			obj.pushvalue();

			lua_State* L = obj.lua_state();
			detail::stack_pop p(L, 1);

#ifndef LUABIND_NO_ERROR_CHECKING

			if (converter.match(L, LUABIND_DECORATE_TYPE(T), -1) < 0)
			{
#ifndef LUABIND_NO_EXCEPTIONS
				throw cast_failed(L, LUABIND_TYPEID(T));
#else
				cast_failed_callback_fun e = get_cast_failed_callback();
				if (e) e(L, LUABIND_TYPEID(T));

				assert(0 && "object_cast failed. If you want to handle this error use luabind::set_error_callback()");
				std::terminate();
#endif
			}
#endif

			return converter.apply(L, LUABIND_DECORATE_TYPE(T), -1);
		}

		template<typename T, typename Obj, typename... Policies>
		imdexlib::option<T> object_cast_nothrow_impl(const Obj& obj, const policy_cons<Policies...>)
		{
		    using converter_policy = typename detail::find_conversion_policy<0, Policies...>::type;
			typename converter_policy::template generate_converter<T, Direction::lua_to_cpp>::type converter;

			if (obj.lua_state() == 0) return imdexlib::option<T>();
			LUABIND_CHECK_STACK(obj.lua_state());

			obj.pushvalue();

			lua_State* L = obj.lua_state();
			detail::stack_pop p(L, 1);

#ifndef LUABIND_NO_ERROR_CHECKING

			if (converter.match(L, LUABIND_DECORATE_TYPE(T), -1) < 0)
				return imdexlib::option<T>();
#endif

            return imdexlib::some(converter.apply(L, LUABIND_DECORATE_TYPE(T), -1));
		}
	}
	
	template<typename T>
	T object_cast(const object& obj)
	{ return detail::object_cast_impl<T>(obj, detail::policy_cons<>()); }
	
	template<typename T, typename... Policies>
	T object_cast(const object& obj, const detail::policy_cons<Policies...> p)
	{ return detail::object_cast_impl<T>(obj, p); }
	
	template<typename T>
    imdexlib::option<T> object_cast_nothrow(const object& obj)
	{ return detail::object_cast_nothrow_impl<T>(obj, detail::policy_cons<>()); }

	template<typename T, typename... Policies>
    imdexlib::option<T> object_cast_nothrow(const object& obj, const detail::policy_cons<Policies...> p)
	{ return detail::object_cast_nothrow_impl<T>(obj, p); }
	

	template<typename T>
	T object_cast(const detail::proxy_object& obj)
	{ return detail::object_cast_impl<T>(obj, detail::policy_cons<>()); }
	
	template<typename T, typename... Policies>
	T object_cast(const detail::proxy_object& obj, const detail::policy_cons<Policies...> p)
	{ return detail::object_cast_impl<T>(obj, p); }
	
	template<typename T>
    imdexlib::option<T> object_cast_nothrow(const detail::proxy_object& obj)
	{ return detail::object_cast_nothrow_impl<T>(obj, detail::policy_cons<>()); }
	
	template<typename T, typename... Policies>
    imdexlib::option<T> object_cast_nothrow(const detail::proxy_object& obj, const detail::policy_cons<Policies...> p)
	{ return detail::object_cast_nothrow_impl<T>(obj, p); }

	
	template<typename T>
	T object_cast(const detail::proxy_raw_object& obj)
	{ return detail::object_cast_impl<T>(obj, detail::policy_cons<>()); }

	template<typename T, typename... Policies>
	T object_cast(const detail::proxy_raw_object& obj, const detail::policy_cons<Policies...> p)
	{ return detail::object_cast_impl<T>(obj, p); }

	template<typename T>
    imdexlib::option<T> object_cast_nothrow(const detail::proxy_raw_object& obj)
	{ return detail::object_cast_nothrow_impl<T>(obj, detail::policy_cons<>()); }

	template<typename T, typename... Policies>
    imdexlib::option<T> object_cast_nothrow(const detail::proxy_raw_object& obj, const detail::policy_cons<Policies...> p)
	{ return detail::object_cast_nothrow_impl<T>(obj, p); }

	
	template<typename T>
	T object_cast(const detail::proxy_array_object& obj)
	{ return detail::object_cast_impl<T>(obj, detail::policy_cons<>()); }
	
	template<typename T, typename... Policies>
	T object_cast(const detail::proxy_array_object& obj, const detail::policy_cons<Policies...> p)
	{ return detail::object_cast_impl<T>(obj, p); }
	
	template<typename T>
    imdexlib::option<T> object_cast_nothrow(const detail::proxy_array_object& obj)
	{ return detail::object_cast_nothrow_impl<T>(obj, detail::policy_cons<>()); }
	
	template<typename T, typename... Policies>
    imdexlib::option<T> object_cast_nothrow(const detail::proxy_array_object& obj, const detail::policy_cons<Policies...> p)
	{ return detail::object_cast_nothrow_impl<T>(obj, p); }




	inline object get_globals(lua_State* L)
	{
		lua_pushvalue(L, LUA_GLOBALSINDEX);
		detail::lua_reference ref;
		ref.set(L);
		return object(L, ref, true/*object::reference()*/);
	}

	inline object get_registry(lua_State* L)
	{
		lua_pushvalue(L, LUA_REGISTRYINDEX);
		detail::lua_reference ref;
		ref.set(L);
		return object(L, ref, true/*object::reference()*/);
	}

	inline object newtable(lua_State* L)
	{
		lua_newtable(L);
		detail::lua_reference ref;
		ref.set(L);
		return object(L, ref, true/*object::reference()*/);
	}
}
