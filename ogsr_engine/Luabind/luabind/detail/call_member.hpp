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
#include <luabind/detail/convert_to_lua.hpp>
#include <luabind/detail/pcall.hpp>
#include <luabind/error.hpp>

#include <tuple>

namespace luabind
{
	namespace detail
	{
		// if the proxy_member_caller returns non-void
			template<typename Ret, typename... Ts>
			class proxy_member_caller
			{
                using tuple_t = std::tuple<Ts...>;
			public:

				proxy_member_caller(lua_State* L_, const tuple_t& args)
					: L(L_)
					, m_args(args)
					, m_called(false)
				{
				}

                proxy_member_caller(lua_State* L_, tuple_t&& args)
                    : L(L_)
                    , m_args(std::move(args))
                    , m_called(false)
                {
                }

				proxy_member_caller(const proxy_member_caller& rhs)
					: L(rhs.L)
					, m_args(rhs.m_args)
					, m_called(rhs.m_called)
				{
					rhs.m_called = true;
				}

                proxy_member_caller(proxy_member_caller&& rhs)
                    : L(rhs.L)
                    , m_args(std::move(rhs.m_args))
                    , m_called(rhs.m_called)
                {
                    rhs.m_called = true;
                }

				~proxy_member_caller() LUABIND_DTOR_NOEXCEPT
				{
					if (m_called) return;

					m_called = true;

					// don't count the function and self-reference
					// since those will be popped by pcall
					const int top = lua_gettop(L) - 2;

					// pcall will pop the function and self reference
					// and all the parameters

					push_args_from_tuple<1>::apply(L, m_args);
					if (pcall(L, sizeof...(Ts) + 1, 0))
					{
						assert(lua_gettop(L) == top + 1);
#ifndef LUABIND_NO_EXCEPTIONS
						throw luabind::error(L);
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);
	
						assert(0 && "the lua function threw an error and exceptions are disabled."
								"If you want to handle this error use luabind::set_error_callback()");
						std::terminate();
#endif
					}
					// pops the return values from the function
					stack_pop pop(L, lua_gettop(L) - top);
				}

				operator Ret()
				{
					typename default_policy::generate_converter<Ret, Direction::lua_to_cpp>::type converter;

					m_called = true;

					// don't count the function and self-reference
					// since those will be popped by pcall
					const int top = lua_gettop(L) - 2;

					// pcall will pop the function and self reference
					// and all the parameters
					push_args_from_tuple<1>::apply(L, m_args);
					if (pcall(L, sizeof...(Ts) + 1, 1))
					{
						assert(lua_gettop(L) == top + 1);
#ifndef LUABIND_NO_EXCEPTIONS
						throw luabind::error(L); 
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);
	
						assert(0 && "the lua function threw an error and exceptions are disabled."
							"If you want to handle this error use luabind::set_error_callback()");
						std::terminate();
#endif
					}

					// pops the return values from the function
					stack_pop pop(L, lua_gettop(L) - top);

#ifndef LUABIND_NO_ERROR_CHECKING

					if (converter.match(L, LUABIND_DECORATE_TYPE(Ret), -1) < 0)
					{
						assert(lua_gettop(L) == top + 1);
#ifndef LUABIND_NO_EXCEPTIONS
						throw cast_failed(L, LUABIND_TYPEID(Ret));
#else
						cast_failed_callback_fun e = get_cast_failed_callback();
						if (e) e(L, LUABIND_TYPEID(Ret));

						assert(0 && "the lua function's return value could not be converted."
							"If you want to handle this error use luabind::set_error_callback()");
						std::terminate();
#endif
					}
#endif
					return converter.apply(L, LUABIND_DECORATE_TYPE(Ret), -1);
				}

				template<typename... Policies>
				Ret operator[](const policy_cons<Policies...> p)
				{
					typedef typename find_conversion_policy<0, Policies...>::type converter_policy;
					typename converter_policy::template generate_converter<Ret, Direction::lua_to_cpp>::type converter;

					m_called = true;

					// don't count the function and self-reference
					// since those will be popped by pcall
					const int top = lua_gettop(L) - 2;

					// pcall will pop the function and self reference
					// and all the parameters

					detail::push_args_from_tuple<1>::apply(L, m_args, p);
					if (pcall(L, sizeof...(Ts) + 1, 1))
					{
						assert(lua_gettop(L) == top + 1);
#ifndef LUABIND_NO_EXCEPTIONS
						throw error(L);
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);
	
						assert(0 && "the lua function threw an error and exceptions are disabled."
							"If you want to handle this error use luabind::set_error_callback()");
						std::terminate();
#endif
					}

					// pops the return values from the function
					stack_pop pop(L, lua_gettop(L) - top);

#ifndef LUABIND_NO_ERROR_CHECKING

						if (converter.match(L, LUABIND_DECORATE_TYPE(Ret), -1) < 0)
					{
						assert(lua_gettop(L) == top + 1);
#ifndef LUABIND_NO_EXCEPTIONS
						throw cast_failed(L, LUABIND_TYPEID(Ret));
#else
						cast_failed_callback_fun e = get_cast_failed_callback();
						if (e) e(L, LUABIND_TYPEID(Ret));

						assert(0 && "the lua function's return value could not be converted."
							"If you want to handle this error use luabind::set_error_callback()");
						std::terminate();
#endif
					}
#endif
					return converter.apply(L, LUABIND_DECORATE_TYPE(Ret), -1);
				}

			private:

				lua_State* L;
				tuple_t m_args;
				mutable bool m_called;
			};

		// if the proxy_member_caller returns void
			template<typename... Ts>
			class proxy_member_void_caller
			{
                using tuple_t = std::tuple<Ts...>;
			    friend class luabind::object;
			public:

				proxy_member_void_caller(lua_State* L_, const tuple_t& args)
					: L(L_)
					, m_args(args)
					, m_called(false)
				{
				}

                proxy_member_void_caller(lua_State* L_, tuple_t&& args)
                    : L(L_)
                    , m_args(std::move(args))
                    , m_called(false)
                {
                }

				proxy_member_void_caller(const proxy_member_void_caller& rhs)
					: L(rhs.L)
					, m_args(rhs.m_args)
					, m_called(rhs.m_called)
				{
					rhs.m_called = true;
				}

                proxy_member_void_caller(proxy_member_void_caller&& rhs)
                    : L(rhs.L)
                    , m_args(std::move(rhs.m_args))
                    , m_called(rhs.m_called)
                {
                    rhs.m_called = true;
                }

				~proxy_member_void_caller() LUABIND_DTOR_NOEXCEPT
				{
					if (m_called) return;

					m_called = true;

					// don't count the function and self-reference
					// since those will be popped by pcall
					const int top = lua_gettop(L) - 2;

					// pcall will pop the function and self reference
					// and all the parameters

					push_args_from_tuple<1>::apply(L, m_args);
					if (pcall(L, sizeof...(Ts) + 1, 0))
					{
						assert(lua_gettop(L) == top + 1);
#ifndef LUABIND_NO_EXCEPTIONS
						throw luabind::error(L);
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);
	
						assert(0 && "the lua function threw an error and exceptions are disabled."
							"If you want to handle this error use luabind::set_error_callback()");
						std::terminate();
#endif
					}
					// pops the return values from the function
					stack_pop pop(L, lua_gettop(L) - top);
				}

				template<typename... Policies>
				void operator[](const policy_cons<Policies...> p)
				{
					m_called = true;

					// don't count the function and self-reference
					// since those will be popped by pcall
					const int top = lua_gettop(L) - 2;

					// pcall will pop the function and self reference
					// and all the parameters

					detail::push_args_from_tuple<1>::apply(L, m_args, p);
					if (pcall(L, sizeof...(Ts) + 1, 0))
					{
						assert(lua_gettop(L) == top + 1);
#ifndef LUABIND_NO_EXCEPTIONS
						throw error(L);
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);
	
						assert(0 && "the lua function threw an error and exceptions are disabled."
							"If you want to handle this error use luabind::set_error_callback()");
						std::terminate();
#endif
					}
					// pops the return values from the function
					stack_pop pop(L, lua_gettop(L) - top);
				}

			private:
				lua_State* L;
				tuple_t m_args;
				mutable bool m_called;
			};

	} // detail

    template<typename R, typename... Args>
    decltype(auto) call_member(object const& obj, const char* name, const Args&... args)
    {
        using proxy_type = std::conditional_t<
            std::is_void_v<R>,
            luabind::detail::proxy_member_void_caller<const Args*...>,
            luabind::detail::proxy_member_caller<R, const Args*...>
        >;

        obj.pushvalue();
        lua_pushstring(obj.lua_state(), name);
        lua_gettable(obj.lua_state(), -2);

        lua_pushvalue(obj.lua_state(), -2);

        lua_remove(obj.lua_state(), -3);

        return proxy_type(obj.lua_state(), std::make_tuple(&args...));
    }
}
