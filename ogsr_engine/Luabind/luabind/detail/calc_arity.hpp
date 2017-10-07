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

#include <type_traits>

namespace luabind { namespace detail
{
    template <int Value, int... Values>
    struct sum_arity
    {
        static constexpr int value = Value + sum_arity<Values...>::value;
    };
    
    template <int Value>
    struct sum_arity<Value>
    {
        static constexpr int value = Value;
    };

	template<size_t N>
    struct calc_arity
	{
	private:

        template <size_t Index, typename... Policies>
        static constexpr int hasArg() noexcept
        {
            using p = typename find_conversion_policy<Index + 1, Policies...>::type;
            return p::has_arg ? 1 : 0;
        }

        template <typename... Policies, size_t... Indices>
        static constexpr int applyImpl(std::index_sequence<Indices...>) noexcept
        {
            return sum_arity<hasArg<Indices, Policies...>()...>::value;
        }

	public:

        template <typename... Policies>
	    static constexpr int apply() noexcept
	    {
            return applyImpl<Policies...>(std::make_index_sequence<N>());
	    }
	};

    template<>
    struct calc_arity<0>
    {
        template <typename...>
        static constexpr int apply() noexcept
        {
            return 0;
        }
    };
}}
