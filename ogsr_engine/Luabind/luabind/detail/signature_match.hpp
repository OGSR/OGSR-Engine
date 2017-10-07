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

#include <luabind/detail/primitives.hpp>
#include <luabind/detail/object_rep.hpp>
#include <luabind/detail/class_rep.hpp>
#include <luabind/detail/policy.hpp>
#include <luabind/detail/policy_cons.hpp>

namespace luabind
{
	template<typename... Ts>
	struct constructor
	{
        static constexpr size_t arity() noexcept
        {
            return sizeof...(Ts);
        }
	};
}

namespace luabind { namespace detail
{
    template <size_t Counter, typename T, typename... Policies>
    int calcResult(lua_State* L, const int startIndex, const int currentIndex)
    {
        using converter_policy = typename find_conversion_policy<Counter + 1, Policies...>::type;
        using converter = typename converter_policy::template generate_converter<T, Direction::lua_to_cpp>::type;
        return converter::match(L, luabind::detail::decorated_type<T>::get(), startIndex + currentIndex);
    }

    template <size_t Counter, typename... ConstructorArgs>
    struct match_constructor_impl;

    template <size_t Counter, typename Head, typename... Tail>
    struct match_constructor_impl<Counter, Head, Tail...>
    {
        template <typename... Policies>
        static int apply(lua_State* L, const int startIndex, const int currentIndex, const int result)
        {
            using converter_policy = typename find_conversion_policy<Counter + 1, Policies...>::type;
            const int r = calcResult<Counter, Head, Policies...>(L, startIndex, currentIndex);

            if (r < 0)
            {
                return -1;
            }
            else
            {
                const int newCurrentIndex = currentIndex + (converter_policy::has_arg ? 1 : 0);
                return match_constructor_impl<Counter + 1, Tail...>::template apply<Policies...>(L, startIndex, newCurrentIndex, result + r);
            }
        }
    };

    template <size_t Counter, typename T>
    struct match_constructor_impl<Counter, T>
    {
        template <typename... Policies>
        static int apply(lua_State* L, const int startIndex, const int currentIndex, const int result)
        {
            const int r = calcResult<Counter, T, Policies...>(L, startIndex, currentIndex);
            return r < 0 ? -1 : result + r;
        }
    };

    template <typename... ConstructorArgs>
    struct match_constructor
    {
        template <typename... Policies>
        static int apply(lua_State* L, const int startIndex)
        {
            return match_constructor_impl<0, ConstructorArgs...>::template apply<Policies...>(L, startIndex, 0, 0);
        }
    };

    template <>
    struct match_constructor<>
    {
        template <typename...>
        static int apply(lua_State*, const int) noexcept
        {
            return 0;
        }
    };

    // this is a function that checks if the lua stack (starting at the given start_index) matches
    // the types in the constructor type given as 3:rd parameter. It uses the Policies given as
    // 4:th parameter to do the matching. It returns the total number of cast-steps that needs to
    // be taken in order to match the parameters on the lua stack to the given parameter-list. Or,
    // if the parameter doesn't match, it returns -1.
    template<typename... ConstructorArgs, typename... Policies>
    int match_params(lua_State* L, const int startIndex, const policy_cons<Policies...>)
    {
        return match_constructor<ConstructorArgs...>::template apply<Policies...>(L, startIndex);
    }

    template<int StartIndex, typename... ConstructorArgs>
    struct constructor_match
    {
        template <typename... Policies>
        static int apply(lua_State* L)
        {
            int top = lua_gettop(L) - StartIndex + 1;
            if (top != sizeof...(ConstructorArgs)) return -1;

            return match_params<ConstructorArgs...>(L, StartIndex, policy_cons<Policies...>());
        }
    };

    // non-const non-member function this as a pointer
    template<typename WrappedClass,
             typename R,
             typename... Args,
             typename... Policies>
    int match(R(*)(Args...), lua_State* L, WrappedClass*, const policy_cons<Policies...> policies)
    {
        return match_params<Args...>(L, 1, policies);
    }

    // non-const member function
    template<typename T,
             typename WrappedClass,
             typename R,
             typename... Args,
             typename... Policies>
    int match(R(T::*)(Args...), lua_State* L, WrappedClass*, const policy_cons<Policies...> policies)
    {
        return match_params<T&, Args...>(L, 1, policies);
    }

    // const member function
    template<typename T,
             typename WrappedClass,
             typename R,
             typename... Args,
             typename... Policies>
    int match(R(T::*)(Args...) const, lua_State* L, WrappedClass*, const policy_cons<Policies...> policies)
    {
        return match_params<T const&, Args...>(L, 1, policies);
    }
}}
