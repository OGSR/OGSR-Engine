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
#include <luabind/error.hpp>
#include <luabind/detail/convert_to_lua.hpp>
#include <luabind/detail/pcall.hpp>

#include <tuple>

namespace luabind
{
	namespace detail
	{
		// if the proxy_function_caller returns non-void
			template<typename Ret, typename... Ts>
			class proxy_function_caller
			{
                using tuple_t = std::tuple<Ts...>;
			public:

				typedef int(*function_t)(lua_State*, int, int);

				proxy_function_caller(lua_State* L, const int params, function_t fun, const tuple_t& args)
					: m_state(L)
					, m_params(params)
					, m_fun(fun)
					, m_args(args)
					, m_called(false)
				{
				}

                proxy_function_caller(lua_State* L, const int params, function_t fun, tuple_t&& args)
                    : m_state(L)
                    , m_params(params)
                    , m_fun(fun)
                    , m_args(std::move(args))
                    , m_called(false)
                {
                }

				proxy_function_caller(const proxy_function_caller& rhs)
					: m_state(rhs.m_state)
					, m_params(rhs.m_params)
					, m_fun(rhs.m_fun)
					, m_args(rhs.m_args)
					, m_called(rhs.m_called)
				{
					rhs.m_called = true;
				}

                proxy_function_caller(proxy_function_caller&& rhs)
                    : m_state(rhs.m_state)
                    , m_params(rhs.m_params)
                    , m_fun(rhs.m_fun)
                    , m_args(std::move(rhs.m_args))
                    , m_called(rhs.m_called)
                {
                    rhs.m_called = true;
                }

				~proxy_function_caller() LUABIND_DTOR_NOEXCEPT
				{
					if (m_called) return;

					m_called = true;
					lua_State* L = m_state;

					const int top = lua_gettop(L);

					push_args_from_tuple<1>::apply(L, m_args);
					if (m_fun(L, sizeof...(Ts), 0))
					{
						assert(lua_gettop(L) == top - m_params + 1);
#ifndef LUABIND_NO_EXCEPTIONS
						throw luabind::error(L);
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);

						assert(0 && "the lua function threw an error and exceptions are disabled."
									" If you want to handle the error you can use luabind::set_error_callback()");
						std::terminate();
#endif
					}

					// pops the return values from the function call
					stack_pop pop(L, lua_gettop(L) - top + m_params);
				}

				operator Ret()
				{
					typename default_policy::generate_converter<Ret, Direction::lua_to_cpp>::type converter;

					m_called = true;
					lua_State* L = m_state;

					const int top = lua_gettop(L);

					push_args_from_tuple<1>::apply(L, m_args);
					if (m_fun(L, sizeof...(Ts), 1))
					{
						assert(lua_gettop(L) == top - m_params + 1);
#ifndef LUABIND_NO_EXCEPTIONS
						throw luabind::error(L); 
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);
	
						assert(0 && "the lua function threw an error and exceptions are disabled."
								" If you want to handle the error you can use luabind::set_error_callback()");
						std::terminate();
#endif
					}

					// pops the return values from the function call
					stack_pop pop(L, lua_gettop(L) - top + m_params);

#ifndef LUABIND_NO_ERROR_CHECKING

					if (converter.match(L, LUABIND_DECORATE_TYPE(Ret), -1) < 0)
					{
#ifndef LUABIND_NO_EXCEPTIONS
						throw cast_failed(L, LUABIND_TYPEID(Ret));
#else
						cast_failed_callback_fun e = get_cast_failed_callback();
						if (e) e(L, LUABIND_TYPEID(Ret));

						assert(0 && "the lua function's return value could not be converted."
									" If you want to handle the error you can use luabind::set_error_callback()");
						std::terminate();
#endif
					}
#endif
					return converter.apply(L, LUABIND_DECORATE_TYPE(Ret), -1);
				}

				template<typename... Policies>
				Ret operator[](const policy_cons<Policies...> p)
				{
				    using converter_policy = typename detail::find_conversion_policy<0, Policies...>::type;
					typename converter_policy::template generate_converter<Ret, Direction::lua_to_cpp>::type converter;

					m_called = true;
					lua_State* L = m_state;

					const int top = lua_gettop(L);

					detail::push_args_from_tuple<1>::apply(L, m_args, p);
					if (m_fun(L, sizeof...(Ts), 1))
					{ 
						assert(lua_gettop(L) == top - m_params + 1);
#ifndef LUABIND_NO_EXCEPTIONS
						throw error(L);
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);
	
						assert(0 && "the lua function threw an error and exceptions are disabled."
								" If you want to handle the error you can use luabind::set_error_callback()");
						std::terminate();
#endif
					}

					// pops the return values from the function call
					stack_pop pop(L, lua_gettop(L) - top + m_params);

#ifndef LUABIND_NO_ERROR_CHECKING

					if (converter.match(L, LUABIND_DECORATE_TYPE(Ret), -1) < 0)
					{
#ifndef LUABIND_NO_EXCEPTIONS
						throw cast_failed(L, LUABIND_TYPEID(Ret));
#else
						cast_failed_callback_fun e = get_cast_failed_callback();
						if (e) e(L, LUABIND_TYPEID(Ret));

						assert(0 && "the lua function's return value could not be converted."
									" If you want to handle the error you can use luabind::set_error_callback()");
						std::terminate();
#endif
					}
#endif
					return converter.apply(L, LUABIND_DECORATE_TYPE(Ret), -1);
				}

			private:

				lua_State* m_state;
				int m_params;
				function_t m_fun;
				std::tuple<Ts...> m_args;
				mutable bool m_called;
			};

		// if the proxy_member_caller returns void
			template<typename... Ts>
			class proxy_function_void_caller
			{
			    friend class luabind::object;
                using tuple_t = std::tuple<Ts...>;
			public:

				typedef int(*function_t)(lua_State*, int, int);

				proxy_function_void_caller(lua_State* L, const int params, function_t fun, const tuple_t& args)
					: m_state(L)
					, m_params(params)
					, m_fun(fun)
					, m_args(args)
					, m_called(false)
				{
				}

                proxy_function_void_caller(lua_State* L, const int params, function_t fun, tuple_t&& args)
                    : m_state(L)
                    , m_params(params)
                    , m_fun(fun)
                    , m_args(std::move(args))
                    , m_called(false)
                {
                }

				proxy_function_void_caller(const proxy_function_void_caller& rhs)
					: m_state(rhs.m_state)
					, m_params(rhs.m_params)
					, m_fun(rhs.m_fun)
					, m_args(rhs.m_args)
					, m_called(rhs.m_called)
				{
					rhs.m_called = true;
				}

                proxy_function_void_caller(proxy_function_void_caller&& rhs)
                    : m_state(rhs.m_state)
                    , m_params(rhs.m_params)
                    , m_fun(rhs.m_fun)
                    , m_args(std::move(rhs.m_args))
                    , m_called(rhs.m_called)
                {
                    rhs.m_called = true;
                }

				~proxy_function_void_caller() LUABIND_DTOR_NOEXCEPT
				{
					if (m_called) return;

					m_called = true;
					lua_State* L = m_state;

					const int top = lua_gettop(L);

					push_args_from_tuple<1>::apply(L, m_args);
					if (m_fun(L, sizeof...(Ts), 0))
					{
						assert(lua_gettop(L) == top - m_params + 1);
#ifndef LUABIND_NO_EXCEPTIONS
						throw luabind::error(L);
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);
	
						assert(0 && "the lua function threw an error and exceptions are disabled."
								" If you want to handle the error you can use luabind::set_error_callback()");
						std::terminate();
#endif
					}
					// pops the return values from the function call
					stack_pop pop(L, lua_gettop(L) - top + m_params);
				}

				template<typename... Policies>
				void operator[](const policy_cons<Policies...> p)
				{
					m_called = true;
					lua_State* L = m_state;

					const int top = lua_gettop(L);

					detail::push_args_from_tuple<1>::apply(L, m_args, p);
					if (m_fun(L, sizeof...(Ts), 0))
					{
						assert(lua_gettop(L) == top - m_params + 1);
#ifndef LUABIND_NO_EXCEPTIONS
						throw error(L);
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);
	
						assert(0 && "the lua function threw an error and exceptions are disabled."
							" If you want to handle the error you can use luabind::set_error_callback()");
						std::terminate();
#endif
					}
					// pops the return values from the function call
					stack_pop pop(L, lua_gettop(L) - top + m_params);
				}

			private:

				lua_State* m_state;
				int m_params;
				function_t m_fun;
				tuple_t m_args;
				mutable bool m_called;
			};
	}

    template<typename Ret, typename... Args>
    decltype(auto) call_function(lua_State* L, const char* name, const Args&... args)
    {
        assert(name && "luabind::call_function() expects a function name");

        using proxy_type = std::conditional_t<
            std::is_void_v<Ret>,
            luabind::detail::proxy_function_void_caller<const Args*...>,
            luabind::detail::proxy_function_caller<Ret, const Args*...>
        >;

        lua_pushstring(L, name);
        lua_gettable(L, (-10002));

        return proxy_type(L, 1, &detail::pcall, std::make_tuple(&args...));
    }

    template<typename Ret, typename... Args>
    decltype(auto) call_function(luabind::object const& obj, const Args&... args)
    {
        using proxy_type = std::conditional_t<
            std::is_void_v<Ret>,
            luabind::detail::proxy_function_void_caller<const Args*...>,
            luabind::detail::proxy_function_caller<Ret, const Args*...>
        >;

        obj.pushvalue();
        return proxy_type(obj.lua_state(), 1, &detail::pcall, std::make_tuple(&args...));
    }

    template<typename Ret, typename... Args>
    decltype(auto) resume_function(lua_State* L, const char* name, const Args&... args)
    {
        assert(name && "luabind::resume_function() expects a function name");

        using proxy_type = std::conditional_t<
            std::is_void_v<Ret>,
            luabind::detail::proxy_function_void_caller<const Args*...>,
            luabind::detail::proxy_function_caller<Ret, const Args*...>
        >;

        lua_pushstring(L, name);
        lua_gettable(L, (-10002));

        return proxy_type(L, 1, &detail::resume_impl, std::make_tuple(&args...));
    }

    template<typename Ret, typename... Args>
    decltype(auto) resume_function(luabind::object const& obj, const Args&... args)
    {
        using proxy_type = std::conditional_t<
            std::is_void_v<Ret>,
            luabind::detail::proxy_function_void_caller<const Args*...>,
            luabind::detail::proxy_function_caller<Ret, const Args*...>
        >;

        obj.pushvalue();
        return proxy_type(obj.lua_state(), 1, &detail::resume_impl, std::make_tuple(&args...));
    }

    template<typename Ret, typename... Args>
    decltype(auto) resume(lua_State* L, const Args&... args)
    {
        using proxy_type = std::conditional_t<
            std::is_void_v<Ret>,
            luabind::detail::proxy_function_void_caller<const Args*...>,
            luabind::detail::proxy_function_caller<Ret, const Args*...>
        >;

        return proxy_type(L, 0, &detail::resume_impl, std::make_tuple(&args...));
    }
}
