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
#include <luabind/detail/convert_to_lua.hpp>
#include <luabind/detail/pcall.hpp>
#include <luabind/error.hpp>
#include <luabind/detail/stack_utils.hpp>

namespace luabind
{
	namespace detail
	{
		// if the proxy_functor_caller returns non-void
			template<typename Ret, typename... Ts>
			class proxy_functor_caller
			{
                using tuple_t = std::tuple<Ts...>;
			public:

				proxy_functor_caller(luabind::functor<Ret>* o, const tuple_t& args)
					: m_func(o)
					, m_args(args)
					, m_called(false)
				{
				}

                proxy_functor_caller(luabind::functor<Ret>* o, tuple_t&& args)
                    : m_func(o)
                    , m_args(std::move(args))
                    , m_called(false)
                {
                }

				proxy_functor_caller(const proxy_functor_caller& rhs)
					: m_func(rhs.m_func)
					, m_args(rhs.m_args)
					, m_called(rhs.m_called)
				{
					rhs.m_called = true;
				}

                proxy_functor_caller(proxy_functor_caller&& rhs)
                    : m_func(rhs.m_func)
                    , m_args(std::move(rhs.m_args))
                    , m_called(rhs.m_called)
                {
                    rhs.m_called = true;
                }

				~proxy_functor_caller() LUABIND_DTOR_NOEXCEPT
				{
					if (m_called) return;

					m_called = true;
					lua_State* L = m_func->lua_state();

					// get the function
					m_func->pushvalue();

					push_args_from_tuple<1>::apply(L, m_args);
					if (pcall(L, sizeof...(Ts), 0))
					{ 
#ifndef LUABIND_NO_EXCEPTIONS
						throw luabind::error(L);
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);
	
						assert(0 && "the lua function threw an error and exceptions are disabled."
							"if you want to handle this error use luabind::set_error_callback()");
						std::terminate();
#endif
					}
				}

				operator Ret()
				{
					typename default_policy::template generate_converter<Ret, Direction::lua_to_cpp>::type converter;

					m_called = true;
					lua_State* L = m_func->lua_state();
#ifndef LUABIND_NO_ERROR_CHECKING
					if (L == nullptr)
					{
	#ifndef LUABIND_NO_EXCEPTIONS
						throw error(L); 
	#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);
	
						assert(0 && "tried to call uninitialized functor object."
							"if you want to handle this error use luabind::set_error_callback()");
						std::terminate();
	#endif
					}
#endif

					detail::stack_pop p(L, 1); // pop the return value

					// get the function
					m_func->pushvalue();

					push_args_from_tuple<1>::apply(L, m_args);
					if (pcall(L, sizeof...(Ts), 1))
					{ 
#ifndef LUABIND_NO_EXCEPTIONS
						throw luabind::error(L);
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);
	
						assert(0 && "the lua function threw an error and exceptions are disabled."
							"if you want to handle this error use luabind::set_error_callback()");
						std::terminate();
#endif
					}

#ifndef LUABIND_NO_ERROR_CHECKING

					if (converter.match(L, LUABIND_DECORATE_TYPE(Ret), -1) < 0)
					{
#ifndef LUABIND_NO_EXCEPTIONS
						throw cast_failed(L, LUABIND_TYPEID(Ret));
#else
						cast_failed_callback_fun e = get_cast_failed_callback();
						if (e) e(L, LUABIND_TYPEID(Ret));

						assert(0 && "the lua function's return value could not be converted."
								"if you want to handle this error use luabind::set_error_callback()");
						std::terminate();
#endif
					}
#endif
					return converter.apply(L, LUABIND_DECORATE_TYPE(Ret), -1);
				}

				template<typename... Policies>
				Ret operator[](const policy_cons<Policies...> p)
				{
					typedef typename detail::find_conversion_policy<0, Policies...>::type converter_policy;
					typename converter_policy::template generate_converter<Ret, Direction::lua_to_cpp>::type converter;

					m_called = true;
					lua_State* L = m_func->lua_state();
#ifndef LUABIND_NO_ERROR_CHECKING
					if (L == nullptr)
					{
	#ifndef LUABIND_NO_EXCEPTIONS
						throw error(L); 
	#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);
	
						assert(0 && "tried to call uninitialized functor object."
							"if you want to handle this error use luabind::set_error_callback()");
						std::terminate();
	#endif
					}
#endif

					detail::stack_pop popper(L, 1); // pop the return value

					// get the function
					m_func->pushvalue();

					detail::push_args_from_tuple<1>::apply(L, m_args, p);
					if (pcall(L, sizeof...(Ts), 1))
					{ 
#ifndef LUABIND_NO_EXCEPTIONS
						throw error(L);
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);
	
						assert(0 && "the lua function threw an error and exceptions are disabled."
							"if you want to handle this error use luabind::set_error_callback()");
						std::terminate();
#endif
					}

#ifndef LUABIND_NO_ERROR_CHECKING

					if (converter.match(L, LUABIND_DECORATE_TYPE(Ret), -1) < 0)
					{
#ifndef LUABIND_NO_EXCEPTIONS
						throw cast_failed(L, LUABIND_TYPEID(Ret));
#else
						cast_failed_callback_fun e = get_cast_failed_callback();
						if (e) e(L, LUABIND_TYPEID(Ret));

						assert(0 && "the lua function's return value could not be converted."
							"if you want to handle this error use luabind::set_error_callback()");
						std::terminate();
#endif
					}
#endif
					return converter.apply(L, LUABIND_DECORATE_TYPE(Ret), -1);
				}

			private:

				luabind::functor<Ret>* m_func;
				tuple_t m_args;
				mutable bool m_called;
			};

		// if the proxy_member_caller returns void
			template<typename... Ts>
			class proxy_functor_void_caller
			{
                using tuple_t = std::tuple<Ts...>;
			public:

				proxy_functor_void_caller(luabind::functor<void>* o, const tuple_t& args)
					: m_func(o)
					, m_args(args)
					, m_called(false)
				{
				}

                proxy_functor_void_caller(luabind::functor<void>* o, tuple_t&& args)
                    : m_func(o)
                    , m_args(std::move(args))
                    , m_called(false)
                {
                }

				proxy_functor_void_caller(const proxy_functor_void_caller& rhs)
					: m_func(rhs.m_func)
					, m_args(rhs.m_args)
					, m_called(rhs.m_called)
				{
					rhs.m_called = true;
				}

                proxy_functor_void_caller(proxy_functor_void_caller&& rhs)
                    : m_func(rhs.m_func)
                    , m_args(std::move(rhs.m_args))
                    , m_called(rhs.m_called)
                {
                    rhs.m_called = true;
                }

				~proxy_functor_void_caller() LUABIND_DTOR_NOEXCEPT
				{
					if (m_called) return;

					m_called = true;
					lua_State* L = m_func->lua_state();
#ifndef LUABIND_NO_ERROR_CHECKING
					if (L == nullptr)
					{
	#ifndef LUABIND_NO_EXCEPTIONS
						throw error(L); 
	#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);
	
						assert(0 && "tried to call uninitialized functor object."
							"if you want to handle this error use luabind::set_error_callback()");
						std::terminate();
	#endif
					}
#endif
					// get the function
					m_func->pushvalue();

					push_args_from_tuple<1>::apply(L, m_args);
					if (pcall(L, sizeof...(Ts), 0))
					{ 
#ifndef LUABIND_NO_EXCEPTIONS
						throw luabind::error(L);
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);
	
						assert(0 && "the lua function threw an error and exceptions are disabled."
							"if you want to handle this error use luabind::set_error_callback()");
						std::terminate();
#endif
					}
				}

				template<typename... Policies>
				void operator[](const policy_cons<Policies...> p)
				{
					m_called = true;
					lua_State* L = m_func->lua_state();
#ifndef LUABIND_NO_ERROR_CHECKING
					if (L == nullptr)
					{
	#ifndef LUABIND_NO_EXCEPTIONS
						throw error(L); 
	#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);
	
						assert(0 && "tried to call uninitialized functor object."
							"if you want to handle this error use luabind::set_error_callback()");
						std::terminate();
	#endif
					}
#endif
					// get the function
					m_func->pushvalue();

					detail::push_args_from_tuple<1>::apply(L, m_args, p);
					if (pcall(L, sizeof...(Ts), 0))
					{ 
#ifndef LUABIND_NO_EXCEPTIONS
						throw error(L); 
#else
						error_callback_fun e = get_error_callback();
						if (e) e(L);
	
						assert(0 && "the lua function threw an error and exceptions are disabled."
							"if you want to handle this error use luabind::set_error_callback()");
						std::terminate();
#endif
					}
				}

			private:

				luabind::functor<void>* m_func;
				tuple_t m_args;
				mutable bool m_called;
			};

	} // detail

	template<typename Ret>
	class functor
	{
	public:

		functor(lua_State* L, const char* name)
			: L_(L)
		{
			lua_pushstring(L, name);
			lua_gettable(L, LUA_GLOBALSINDEX);
			ref_.set(L_);
		}

		functor()
			: L_(nullptr)
		{
		}

		functor(const functor<Ret>& obj)
			: L_(obj.L_)
			, ref_(obj.ref_)
		{
		}

		// this is a safe substitute for an implicit converter to bool
		typedef void (functor::*member_ptr)() const;
		operator member_ptr() const
		{
			if (is_valid()) return &functor::dummy;
			return nullptr;
		}

		const functor<Ret>& operator=(const functor<Ret>& rhs)
		{
			L_ = rhs.L_;
			ref_ = rhs.ref_;
			return *this;
		}

		bool operator==(const functor<Ret>& rhs) const
		{
			if (!ref_.is_valid() || !rhs.ref_.is_valid()) return false;
			pushvalue();
			rhs.pushvalue();
			const bool result = lua_equal(L_, -1, -2) != 0;
			lua_pop(L_, 2);
			return result;
		}

		bool operator!=(const functor<Ret>& rhs) const
		{
			if (!ref_.is_valid() || !rhs.ref_.is_valid()) return true;
			pushvalue();
			rhs.pushvalue();
			const bool result = lua_equal(L_, -1, -2) == 0;
			lua_pop(L_, 2);
			return result;
		}

		bool is_valid() const { return ref_.is_valid(); }
	
		lua_State* lua_state() const { return L_; }
		void pushvalue() const { ref_.get(L_); }

		void reset()
		{
			L_ = nullptr;
			ref_.reset();
		}

        template<typename... Args>
        decltype(auto) operator()(const Args&... args) const
        {
            using proxy_type = std::conditional_t<
                std::is_void_v<Ret>,
                luabind::detail::proxy_functor_void_caller<const Args*...>,
                luabind::detail::proxy_functor_caller<Ret, const Args*...>
            >;

            return proxy_type(const_cast<luabind::functor<Ret>*>(this), std::make_tuple(&args...));
        }

		functor(lua_State* L, detail::lua_reference const& ref)
			: L_(L)
			, ref_(ref)
		{
		}

	private:

		void dummy() const {}

		lua_State* L_;
		detail::lua_reference ref_;
	};
}
