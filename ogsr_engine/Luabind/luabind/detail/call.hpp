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
#include <luabind/yield_policy.hpp>
#include <luabind/detail/calc_has_arg.hpp>

namespace luabind { namespace detail
{
	template<typename... Policies>
	struct maybe_yield
	{
		static int apply(lua_State* L, int nret)
		{
			return ret(L, nret, has_yield<Policies...>());
		}

		static int ret(lua_State* L, int nret, std::true_type)
		{
			return lua_yield(L, nret);
		}

		static int ret(lua_State*, int nret, std::false_type)
		{
			return nret;
		}
	};

	template<typename Class, typename WrappedClass>
	struct most_derived
	{
        using type = std::conditional_t<
            std::is_base_of_v<Class, WrappedClass>,
            WrappedClass,
            Class
        >;
	};

    template<typename Class, typename WrappedClass>
    using most_derived_t = typename most_derived<Class, WrappedClass>::type;

	template<typename T>
	struct returns
	{
	private:

        template <int ResInitWithOffset, typename U, size_t Index, typename... Policies>
        static auto genConverter()
        {
            using converter_policy = typename find_conversion_policy<Index + ResInitWithOffset, Policies...>::type;
            return typename converter_policy::template generate_converter<U, Direction::lua_to_cpp>::type();
        }

        template <int ResInitWithOffset, typename... Ts, size_t... Indices, typename... Policies>
        static auto generateConverters(const std::index_sequence<Indices...>, const policy_cons<Policies...>)
        {
            return std::make_tuple(
                genConverter<ResInitWithOffset, Ts, Indices, Policies...>()...
            );
        }

        template <int ResInitWithOffset, typename U, size_t Index, typename Conv, typename... Policies>
        static decltype(auto) apply(lua_State* L, Conv& conv, const policy_cons<Policies...>)
        {
            return conv.apply(L, decorated_type<U>::get(), calcHasArg<ResInitWithOffset, ResInitWithOffset, Index, Policies...>());
        }

        template <int ResInitWithOffset, typename U, size_t Index, typename Conv, typename... Policies>
        static void postcall(lua_State* L, Conv& conv, const policy_cons<Policies...>)
        {
            conv.converter_postcall(L, decorated_type<U>::get(), calcHasArg<ResInitWithOffset, ResInitWithOffset, Index, Policies...>());
        }

        template <typename WrappedClass, typename C, typename... Args, typename... Ts>
        static decltype(auto) callApply(T(C::*f)(Args...), lua_State* L, Ts&&... args)
        {
            using self_type = most_derived_t<C, WrappedClass>;
            pointer_converter<Direction::lua_to_cpp> self_cv;

            return (self_cv.apply(L, decorated_type<self_type*>::get(), 1)->*f)(std::forward<Ts>(args)...);
        }

        template <typename WrappedClass, typename C, typename... Args, typename... Ts>
        static decltype(auto) callApply(T(C::*f)(Args...) const, lua_State* L, Ts&&... args)
        {
            using self_type = most_derived_t<C, WrappedClass>;
            const_pointer_converter<Direction::lua_to_cpp> self_cv;

            return (self_cv.apply(L, decorated_type<self_type const*>::get(), 1)->*f)(std::forward<Ts>(args)...);
        }

        template <typename WrappedClass, typename... Args, typename... Ts>
        static decltype(auto) callApply(T(*f)(Args...), lua_State*, Ts&&... args)
        {
            return f(std::forward<Ts>(args)...);
        }

        template <int ResInitWithOffset, typename WrappedClass, typename Fn, typename... Converters, typename... Ts, size_t... Indices, typename... Policies>
        static void callApply(Fn&& fn, lua_State* L, std::tuple<Converters...>& converters, const std::index_sequence<Indices...>,
                              const imdexlib::typelist<Ts...>, const policy_cons<Policies...> policies, std::false_type /*is_void<T>*/)
        {
            using converter_policy_ret = typename find_conversion_policy<0, Policies...>::type;
            typename converter_policy_ret::template generate_converter<T, Direction::cpp_to_lua>::type converter_ret;

            converter_ret.apply(L, callApply<WrappedClass>(std::forward<Fn>(fn), L, apply<ResInitWithOffset, Ts, Indices>(L, std::get<Indices>(converters), policies)...));
        }

        template <int ResInitWithOffset, typename WrappedClass, typename Fn, typename... Converters, typename... Ts, size_t... Indices, typename... Policies>
        static void callApply(Fn&& fn, lua_State* L, std::tuple<Converters...>& converters, const std::index_sequence<Indices...>,
                              const imdexlib::typelist<Ts...>, const policy_cons<Policies...> policies, std::true_type /*is_void<T>*/)
        {
            callApply<WrappedClass>(std::forward<Fn>(fn), L, apply<ResInitWithOffset, Ts, Indices>(L, std::get<Indices>(converters), policies)...);
        }

        template <int ResInitWithOffset, typename... Ts, typename... Converters, size_t... Indices, typename... Policies>
        static void callPostcall(lua_State* L, std::tuple<Converters...>& converters, const std::index_sequence<Indices...>, const policy_cons<Policies...> policies)
        {
            const int expander [] = { 0, (postcall<ResInitWithOffset, Ts, Indices>(L, std::get<Indices>(converters), policies), 0)... };
            (void) expander;
        }

        template <int ResInitWithOffset, typename... Policies, size_t... Indices>
        static void callPolicyListPostcall(lua_State* L, const int first, const std::index_sequence<Indices...>)
        {
            const int indices [] =
            {
                /*nargs + nret*/first,
                1,
                calcHasArg<ResInitWithOffset, ResInitWithOffset, Indices, Policies...>()...
            };

            policy_list_postcall<Policies...>::apply(L, indices);
        }

        template <typename WrappedClass,
                  int ResInitWithOffset,
                  typename Fn,
	              typename... Args,
	              typename... Policies>
        static int call(Fn&& fn, lua_State* L, const imdexlib::typelist<Args...>, const policy_cons<Policies...> policies)
        {
            const int nargs = lua_gettop(L);
            const auto indices = std::make_index_sequence<sizeof...(Args)>();
            auto converters = generateConverters<ResInitWithOffset, Args...>(indices, policies);

            callApply<ResInitWithOffset, WrappedClass>(std::forward<Fn>(fn), L, converters, indices, imdexlib::typelist<Args...>(), policies, std::is_void<T>());
            callPostcall<ResInitWithOffset, Args...>(L, converters, indices, policies);

            const int nret = lua_gettop(L) - nargs;

            callPolicyListPostcall<ResInitWithOffset, Policies...>(L, nargs + nret, indices);

            return maybe_yield<Policies...>::apply(L, nret);
        }

	public:

        template<typename C,
	             typename WrappedClass,
                 typename... Args,
                 typename... Policies>
        static int call(T(C::*f)(Args...), WrappedClass*, lua_State* L, const policy_cons<Policies...> policies)
        {
            return call<WrappedClass, 2>(f, L, imdexlib::typelist<Args...>(), policies);
        }

        template<typename C,
	             typename WrappedClass,
	             typename... Args,
	             typename... Policies>
        static int call(T(C::*f)(Args...) const, WrappedClass*, lua_State* L, const policy_cons<Policies...> policies)
        {
            return call<WrappedClass, 2>(f, L, imdexlib::typelist<Args...>(), policies);
        }

        template<typename WrappedClass,
                 typename... Args,
                 typename... Policies>
        static int call(T(*f)(Args...), WrappedClass*, lua_State* L, const policy_cons<Policies...> policies)
        {
            return call<WrappedClass, 1>(f, L, imdexlib::typelist<Args...>(), policies);
        }
	};

    template<typename WrappedClass, typename R, typename... Args, typename... Policies>
    int call(R(*f)(Args...), WrappedClass*, lua_State* L, const policy_cons<Policies...> policies)
    {
        return returns<R>::call(f, static_cast<WrappedClass*>(nullptr), L, policies);
    }

    template<typename T, typename WrappedClass, typename R, typename... Args, typename... Policies>
    int call(R(T::*f)(Args...), WrappedClass*, lua_State* L, const policy_cons<Policies...> policies)
    {
        return returns<R>::call(f, static_cast<WrappedClass*>(nullptr), L, policies);
    }

    template<typename T, typename WrappedClass, typename R, typename... Args, typename... Policies>
    int call(R(T::*f)(Args...) const, WrappedClass*, lua_State* L, const policy_cons<Policies...> policies)
    {
        return returns<R>::call(f, static_cast<WrappedClass*>(nullptr), L, policies);
    }
}}
