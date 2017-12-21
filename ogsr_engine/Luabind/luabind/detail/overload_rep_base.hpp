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
#include <functional>

namespace luabind { namespace detail
{
	// this class represents a specific overload of a member-function.
	struct LUABIND_API overload_rep_base
	{
#if !defined(NDEBUG) && !defined(LUABIND_NO_ERROR_CHECKING)
		overload_rep_base(): m_get_signature_fun(nullptr), m_match_fun(), m_arity(-1) {}
#else
        overload_rep_base(): m_match_fun(), m_arity(-1) {}
#endif

        overload_rep_base(const overload_rep_base&) = default;
        overload_rep_base(overload_rep_base&&) = default;

        overload_rep_base& operator= (const overload_rep_base&) = default;
        overload_rep_base& operator= (overload_rep_base&&) = default;

        using match_fun_t = std::function<int(lua_State*)>;
		typedef void(*get_sig_ptr)(lua_State*, string_class&);

		int match(lua_State* L, const int num_params) const
		{
			if (num_params != m_arity) return -1;
			return m_match_fun(L);
		}

        template <typename T>
		void set_match_fun(T&& fn) 
		{
			m_match_fun = std::forward<T>(fn);
		}

#ifndef LUABIND_NO_ERROR_CHECKING
		void get_signature(lua_State* L, string_class& s) const 
		{ 
			m_get_signature_fun(L, s); 
		}

		void set_sig_fun(get_sig_ptr f) 
		{ 
			m_get_signature_fun = f; 
		}
#endif

	protected:

#ifndef LUABIND_NO_ERROR_CHECKING
		get_sig_ptr m_get_signature_fun;
#endif

#pragma warning(push)
#pragma warning(disable:4251)
		match_fun_t m_match_fun;
#pragma warning(pop)
		int m_arity;
	};

}} // namespace luabind::detail
