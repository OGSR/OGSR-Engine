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
#include <luabind/wrapper_base.hpp>
#include <luabind/detail/policy.hpp>
#include <luabind/detail/signature_match.hpp>
#include <luabind/detail/call_member.hpp>
#include <luabind/wrapper_base.hpp>
#include <luabind/weak_ref.hpp>

namespace luabind { namespace detail
{
    template<typename T, typename... Policies>
    struct construct_class
    {
    private:

        template <typename U, size_t Index>
        static decltype(auto) applyArg(lua_State* L)
        {
            using converter_policy = typename find_conversion_policy<Index + 1, Policies...>::type;
            using c_t = typename converter_policy::template generate_converter<U, Direction::lua_to_cpp>::type;
            typename converter_policy::template generate_converter<U, Direction::lua_to_cpp>::type c;

            return c.c_t::apply(L, decorated_type<U>::get(), Index + 2);
        }

        template <typename... ConstructorArgs, size_t... Indices>
        static T* applyImpl(lua_State* L, std::index_sequence<Indices...>)
        {
            return luabind::luabind_new<T>(applyArg<ConstructorArgs, Indices>(L)...);
        }

    public:

        template <typename... ConstructorArgs>
        static void* apply(lua_State* L, weak_ref const&)
        {
            return applyImpl<ConstructorArgs...>(L, std::make_index_sequence<sizeof...(ConstructorArgs)>());
        }
    };

    template<typename T, typename W, typename... Policies>
    struct construct_wrapped_class
    {
    private:

        template <typename U, size_t Index>
        static decltype(auto) applyArg(lua_State* L)
        {
            using converter_policy = typename find_conversion_policy<Index + 1, Policies...>::type;
            using c_t = typename converter_policy::template generate_converter<U, Direction::lua_to_cpp>::type;
            typename converter_policy::template generate_converter<U, Direction::lua_to_cpp>::type c;

            return c.c_t::apply(L, decorated_type<U>::get(), Index + 2);
        }

        template <typename... ConstructorArgs, size_t... Indices>
        static T* applyImpl(lua_State* L, weak_ref const& ref, std::index_sequence<Indices...>)
        {
            W* result = luabind::luabind_new<W>(applyArg<ConstructorArgs, Indices>(L)...);
            static_cast<weak_ref&>(wrap_access::ref(*result)) = ref;
            return result;
        }

    public:

        template <typename... ConstructorArgs>
        static void* apply(lua_State* L, weak_ref const& ref)
        {
            return applyImpl<ConstructorArgs...>(L, ref, std::make_index_sequence<sizeof...(ConstructorArgs)>());
        }
    };
}}
