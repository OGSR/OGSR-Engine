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

namespace luabind { namespace detail 
{
	struct yield_policy
	{
		static void precall(lua_State*, const index_map&) {}
		static void postcall(lua_State*, const index_map&) {}
	};

    template <typename... Policies>
    struct has_yield;

	template<typename Policy, typename... Policies>
	struct has_yield<Policy, Policies...> : public std::conditional_t<
                                                        std::is_same_v<yield_policy, Policy>,
                                                        std::true_type,
                                                        has_yield<Policies...>
                                                   >
	{
	};

    template <typename T>
    struct has_yield<T> : public std::is_same<yield_policy, T>
    {
    };

	template<>
	struct has_yield<> : public std::false_type
	{
	};

    template <typename... Policies>
    constexpr bool has_yield_v = has_yield<Policies...>::value;
}}

namespace luabind
{
	namespace 
	{
		detail::policy_cons<detail::yield_policy> yield;
	}
}
