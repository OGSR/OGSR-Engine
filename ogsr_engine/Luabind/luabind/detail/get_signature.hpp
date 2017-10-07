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

#ifndef LUABIND_NO_ERROR_CHECKING
#pragma once

#include <luabind/config.hpp>
#include <luabind/detail/signature_match.hpp>

namespace luabind { namespace detail
{

	string_class LUABIND_API get_class_name(lua_State* L, LUABIND_TYPE_INFO i);

	template<class T>
	string_class name_of_type(by_value<T>, lua_State* L, int) { return luabind::detail::get_class_name(L, LUABIND_TYPEID(T)); };
	template<class T>
	string_class name_of_type(by_reference<T>, lua_State* L, int) { return name_of_type(LUABIND_DECORATE_TYPE(T), L, 0L) + "&"; };
	template<class T>
	string_class name_of_type(by_pointer<T>, lua_State* L, int) { return name_of_type(LUABIND_DECORATE_TYPE(T), L, 0L) + "*"; };
	template<class T>
	string_class name_of_type(by_const_reference<T>, lua_State* L, int) { return "const " + name_of_type(LUABIND_DECORATE_TYPE(T), L, 0L) + "&"; };
	template<class T>
	string_class name_of_type(by_const_pointer<T>, lua_State* L, int) { return "const " + name_of_type(LUABIND_DECORATE_TYPE(T), L, 0L) + "*"; };

	inline string_class name_of_type(by_value<luabind::object>, lua_State*, int) { return "object"; };
	inline string_class name_of_type(by_const_reference<luabind::object>, lua_State*, int) { return "object"; };
	inline string_class name_of_type(by_value<bool>, lua_State*, int) { return "boolean"; }
	inline string_class name_of_type(by_value<char>, lua_State*, int) { return "number"; }
	inline string_class name_of_type(by_value<short>, lua_State*, int) { return "number"; }
	inline string_class name_of_type(by_value<int>, lua_State*, int) { return "number"; }
	inline string_class name_of_type(by_value<long>, lua_State*, int) { return "number"; }
	inline string_class name_of_type(by_value<unsigned char>, lua_State*, int) { return "number"; }
	inline string_class name_of_type(by_value<unsigned short>, lua_State*, int) { return "number"; }
	inline string_class name_of_type(by_value<unsigned int>, lua_State*, int) { return "number"; }
	inline string_class name_of_type(by_value<unsigned long>, lua_State*, int) { return "number"; }

	inline string_class name_of_type(by_value<const bool>, lua_State*, int) { return "boolean"; }
	inline string_class name_of_type(by_value<const char>, lua_State*, int) { return "number"; }
	inline string_class name_of_type(by_value<const short>, lua_State*, int) { return "number"; }
	inline string_class name_of_type(by_value<const int>, lua_State*, int) { return "number"; }
	inline string_class name_of_type(by_value<const long>, lua_State*, int) { return "number"; }
	inline string_class name_of_type(by_value<const unsigned char>, lua_State*, int) { return "number"; }
	inline string_class name_of_type(by_value<const unsigned short>, lua_State*, int) { return "number"; }
	inline string_class name_of_type(by_value<const unsigned int>, lua_State*, int) { return "number"; }
	inline string_class name_of_type(by_value<const unsigned long>, lua_State*, int) { return "number"; }

	template<class T>
	string_class name_of_type(by_value<luabind::functor<T> >, lua_State* L, long) { return "function<" + name_of_type(LUABIND_DECORATE_TYPE(T), L, 0L) + ">"; }

	inline string_class name_of_type(by_value<string_class>, lua_State*, int) { return "string"; }
	inline string_class name_of_type(by_const_pointer<char>, lua_State*, int) { return "string"; }
	inline string_class name_of_type(by_pointer<lua_State>, lua_State*, int) { return "lua_State*"; }

    template <typename T>
    struct type_name_unless_void
    {
        static void apply(string_class& s, lua_State* L, bool first)
        {
            if (!first) s += ", ";
            s += name_of_type(LUABIND_DECORATE_TYPE(T), L, 0L);
        }
    };

	template<>
	struct type_name_unless_void<null_type>
	{
		static void apply(string_class&, lua_State*, const bool) {}
	};

    template<typename T, typename C>
    void get_member_signature_impl(T(C::*)(), lua_State*, luabind::internal_string& s)
    {
        s += "()";
    }

    template<typename T, typename C, typename A, typename... Args>
    void get_member_signature_impl(T(C::*)(A, Args...), lua_State* L, luabind::internal_string& s)
    {
        s += "(";
        s += name_of_type(luabind::detail::decorated_type<A>::get(), L, 0L);
        const int expander[] = { 0, (type_name_unless_void<Args>::apply(s, L, false), 0)... };
        (void) expander;
        s += ")";
    }

    template<typename T, typename C>
    void get_member_signature_impl(T(C::*)() const, lua_State*, luabind::internal_string& s)
    {
        s += "() const";
    }

    template<typename T, typename C, typename A, typename... Args>
    void get_member_signature_impl(T(C::*)(A, Args...) const, lua_State* L, luabind::internal_string& s)
    {
        s += "(";
        s += name_of_type(luabind::detail::decorated_type<A>::get(), L, 0L);
        const int expander [] = { 0, (type_name_unless_void<Args>::apply(s, L, false), 0)... };
        (void) expander;
        s += ") const";
    }

    template<typename T>
    void get_member_signature_impl(T(*f)(), lua_State*, luabind::internal_string& s)
    {
        s += "()";
    }

    template<typename T, typename A, typename... Args>
    void get_member_signature_impl(T(*f)(A, Args...), lua_State* L, luabind::internal_string& s)
    {
        s += "(";
        s += name_of_type(luabind::detail::decorated_type<A>::get(), L, 0L);
        const int expander [] = { 0, (type_name_unless_void<Args>::apply(s, L, false), 0)... };
        (void) expander;
        s += ")";
    }

    template<typename T>
    void get_free_function_signature_impl(T(*f)(), lua_State*, luabind::internal_string& s)
    {
        s += "()";
    }

    template<typename T, typename A, typename... Args>
    void get_free_function_signature_impl(T(*f)(A, Args...), lua_State* L, luabind::internal_string& s)
    {
        s += "(";
        s += name_of_type(luabind::detail::decorated_type<A>::get(), L, 0L);
        const int expander [] = { 0, (type_name_unless_void<Args>::apply(s, L, false), 0)... };
        (void) expander;
        s += ")";
    }

	template<typename F>
	struct get_member_signature
	{
		static void apply(lua_State* L, string_class& s)
		{
			get_member_signature_impl(static_cast<F>(nullptr), L, s);
		}
	};

	template<typename F>
	struct get_free_function_signature
	{
		static void apply(lua_State* L, string_class& s)
		{
			get_free_function_signature_impl(static_cast<F>(nullptr), L, s);
		}
	};

    template<typename... Args>
    struct get_signature;

	template<typename T, typename... Args>
	struct get_signature<T, Args...>
	{
		static void apply(lua_State* L, string_class& s)
		{
            s += "(";
            type_name_unless_void<T>::apply(s, L, true);
            const int expander [] = { 0, (type_name_unless_void<Args>::apply(s, L, false), 0)... };
            (void) expander;
            s += ")";
		}
	};

    template <typename T>
    struct get_signature<T>
    {
        static void apply(lua_State* L, string_class& s)
        {
            s += "(";
            type_name_unless_void<T>::apply(s, L, true);
            s += ")";
        }
    };

    template <>
    struct get_signature<>
    {
        static void apply(lua_State*, string_class& s)
        {
            s += "()";
        }
    };

	template<typename T>
	struct get_setter_signature
	{
		static void apply(lua_State* L, string_class& s)
		{
			s += "(";
			s += name_of_type(LUABIND_DECORATE_TYPE(T), L, 0L);
			s += ")";
		}
	};
}}

#endif // LUABIND_NO_ERROR_CHECKING
