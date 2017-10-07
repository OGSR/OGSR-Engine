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
#include <luabind/weak_ref.hpp>
#include <luabind/detail/ref.hpp>
#include <luabind/detail/call_member.hpp>

namespace luabind
{
	namespace detail
	{
		struct wrap_access;

		// implements the selection between dynamic dispatch
		// or default implementation calls from within a virtual
		// function wrapper. The input is the self reference on
		// the top of the stack. Output is the function to call
		// on the top of the stack (the input self reference will
		// be popped)
		LUABIND_API void do_call_member_selection(lua_State* L, char const* name);
	}

	struct wrapped_self_t: weak_ref
	{
		detail::lua_reference m_strong_ref;
	};

	struct wrap_base
	{
		friend struct detail::wrap_access;
		wrap_base() {}

        template<typename R, typename... Args>
        decltype(auto) call(char const* name, const Args&... args) const
		{
            using proxy_type = std::conditional_t<
                std::is_void_v<R>,
                luabind::detail::proxy_member_void_caller<const Args*...>,
                luabind::detail::proxy_member_caller<R, const Args*...>
            >;

			lua_State* L = m_self.state();
			m_self.get(L);
            assert(!lua_isnil(L, -1));
			detail::do_call_member_selection(L, name);

			m_self.get(L);

			return proxy_type(L, std::make_tuple(&args...));
		}

	private:
		wrapped_self_t m_self;
	};

    template<typename R , typename... Args>
    decltype(auto) call_member(wrap_base const* self, char const* fn, Args&&... args)
    {
        return self->call<R>(fn, std::forward<Args>(args)...);
    }

	namespace detail
	{
		struct wrap_access
		{
			static wrapped_self_t const& ref(wrap_base const& b)
			{
				return b.m_self;
			}

			static wrapped_self_t& ref(wrap_base& b)
			{
				return b.m_self;
			}
		};
	}
}
