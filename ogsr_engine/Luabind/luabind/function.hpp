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

#include <luabind/detail/signature_match.hpp>
#include <luabind/detail/call_function.hpp>
#include <luabind/detail/get_overload_signature.hpp>
#include <luabind/detail/overload_rep_base.hpp>

#include <luabind/scope.hpp>
#include <luabind/detail/calc_has_arg.hpp>

namespace luabind
{
	namespace detail
	{

		namespace free_functions
		{

			struct overload_rep: public overload_rep_base
			{
                template<typename R, typename... Args, typename... Policies>
                overload_rep(R(*f)(Args...), const policy_cons<Policies...>)
                    : fun(reinterpret_cast<void(*)()>(f))
                {
                    m_params_.reserve(sizeof...(Args));

                    const int expander [] = { 0, (m_params_.push_back(&typeid(Args)), 0)... };
                    (void) expander;

                    m_arity = calcHasArg<0, 1, sizeof...(Args), Policies...>();
                }

				bool operator==(const overload_rep& o) const
				{
					if (o.m_arity != m_arity) return false;
					if (o.m_params_.size() != m_params_.size()) return false;
					for (size_t i = 0; i < m_params_.size(); ++i)
						if (!(LUABIND_TYPE_INFO_EQUAL(m_params_[i], o.m_params_[i]))) return false;
					return true;
				}

				typedef int(*call_ptr)(lua_State*, void(*)());

				void set_fun(call_ptr f) { call_fun = f; }
				int call(lua_State* L, void(*f)()) const { return call_fun(L, f); }

				// this is the actual function pointer to be called when this overload is invoked
				void (*fun)();

				call_ptr call_fun;

				// the types of the parameter it takes
				vector_class<LUABIND_TYPE_INFO> m_params_;

                char end;
			};

    		struct LUABIND_API function_rep
			{
				function_rep(const char* name): m_name(name) {}
				void add_overload(const free_functions::overload_rep& o);

				const vector_class<overload_rep>& overloads() const noexcept { return m_overloads; }

				const char* name() const { return m_name; }

			private:
				const char* m_name;

				// this have to be write protected, since each time an overload is
				// added it has to be checked for existence. add_overload() should
				// be used.
#pragma warning(push)
#pragma warning(disable:4251)
				vector_class<free_functions::overload_rep> m_overloads;
#pragma warning(pop)
			};

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

		
			template<typename T>
			struct returns
			{
			private:

                template <typename U, size_t Index, typename... Policies>
                static auto genConverter()
                {
                    using converter_policy = typename find_conversion_policy<Index + 1, Policies...>::type;
                    return typename converter_policy::template generate_converter<U, Direction::lua_to_cpp>::type();
                }

                template <typename... Ts, size_t... Indices, typename... Policies>
                static auto generateConverters(const std::index_sequence<Indices...>, const policy_cons<Policies...>)
                {
                    return std::make_tuple(
                        genConverter<Ts, Indices, Policies...>()...
                    );
                }

                template <typename U, size_t Index, typename Conv, typename... Policies>
                static decltype(auto) apply(lua_State* L, Conv& conv, const policy_cons<Policies...>)
                {
                    return conv.apply(L, decorated_type<U>::get(), calcHasArg<1, 1, Index, Policies...>());
                }

                template <typename U, size_t Index, typename Conv, typename... Policies>
                static void postcall(lua_State* L, Conv& conv, const policy_cons<Policies...>)
                {
                    conv.converter_postcall(L, decorated_type<U>::get(), calcHasArg<1, 1, Index, Policies...>());
                }

                template <typename... Args, typename... Converters, size_t... Indices, typename... Policies>
                static void callApply(T(*f)(Args...), lua_State* L, std::tuple<Converters...>& converters, const std::index_sequence<Indices...>,
                                      const policy_cons<Policies...> policies, std::false_type /*is_void<T>*/)
                {
                    using converter_policy_ret = typename find_conversion_policy<0, Policies...>::type;
                    typename converter_policy_ret::template generate_converter<T, Direction::cpp_to_lua>::type converter_ret;

                    converter_ret.apply(L, f(apply<Args, Indices>(L, std::get<Indices>(converters), policies)...));
                }

                template <typename... Args, typename... Converters, size_t... Indices, typename... Policies>
                static void callApply(T(*f)(Args...), lua_State* L, std::tuple<Converters...>& converters, const std::index_sequence<Indices...>,
                                      const policy_cons<Policies...> policies, std::true_type /*is_void<T>*/)
                {
                    f(apply<Args, Indices>(L, std::get<Indices>(converters), policies)...);
                }

                template <typename... Args, typename... Converters, size_t... Indices, typename... Policies>
                static void callPostcall(lua_State* L, std::tuple<Converters...>& converters, const std::index_sequence<Indices...>, const policy_cons<Policies...> policies)
                {
                    const int expander [] = { 0, (postcall<Args, Indices>(L, std::get<Indices>(converters), policies), 0)... };
                    (void) expander;
                }

                template <typename... Policies, size_t... Indices>
                static void callPolicyListPostcall(lua_State* L, const int second, const std::index_sequence<Indices...>)
                {
                    const int indices [] =
                    {
                        -1,
                        /*nargs + nret*/second,
                        calcHasArg<1, 1, Indices, Policies...>()...
                    };

                    policy_list_postcall<Policies...>::apply(L, indices);
                }

			public:

                template<typename... Args, typename... Policies>
                static int call(T(*f)(Args...), lua_State* L, const policy_cons<Policies...> policies)
                {
                    const int nargs = lua_gettop(L);
                    const auto indices = std::make_index_sequence<sizeof...(Args)>();
                    auto converters = generateConverters<Args...>(indices, policies);

                    callApply(f, L, converters, indices, policies, std::is_void<T>());
                    callPostcall<Args...>(L, converters, indices, policies);

                    const int nret = lua_gettop(L) - nargs;
                    callPolicyListPostcall<Policies...>(L, nargs + nret, indices);

                    return maybe_yield<Policies...>::apply(L, nret);
                }
			};

            template<typename R, typename... Args, typename... Policies>
            int call(R(*f)(Args...), lua_State* L, const policy_cons<Policies...> policies)
            {
                return free_functions::returns<R>::call(f, L, policies);
            }

            template<typename R, typename... Args, typename... Policies>
            static int match(R(*)(Args...), lua_State* L, const policy_cons<Policies...> policies)
            {
                return match_params<Args...>(L, 1, policies);
            }

			template<typename F, typename... Policies>
			struct function_callback_s
			{
				static int apply(lua_State* L, void(*fun)())
				{
					return free_functions::call(reinterpret_cast<F>(fun), L, policy_cons<Policies...>());
				}
			};

			template<typename F, typename... Policies>
			struct match_function_callback_s
			{
				static int apply(lua_State* L)
				{
					F fptr = nullptr;
					return free_functions::match(fptr, L, policy_cons<Policies...>());
				}

				static int callback(lua_State* L)
				{
					F fptr = nullptr;
					return free_functions::match(fptr, L, policy_cons<Policies...>());
				}
			};

			LUABIND_API int function_dispatcher(lua_State* L);
		}
	}

	// deprecated
	template<typename F, typename... Policies>
	void function(lua_State* L, const char* name, F f, const detail::policy_cons<Policies...> p)
	{
		module(L) [ def(name, f, p) ];
	}

	// deprecated
	template<typename F>
	void function(lua_State* L, const char* name, F f)
	{
		luabind::function(L, name, f, detail::policy_cons<>());
	}

	namespace detail
	{
		template<typename F, typename... Policies>
		struct function_commiter : detail::registration
		{
			function_commiter(const char* n, F f, const policy_cons<Policies...> p)
				: m_name(n)
				, fun(f)
				, policies(p)
			{}

			virtual void register_(lua_State* L) const
			{
				detail::free_functions::overload_rep o(fun, policies);

				o.set_match_fun(&detail::free_functions::match_function_callback_s<F, Policies...>::apply);
				o.set_fun(&detail::free_functions::function_callback_s<F, Policies...>::apply);

#ifndef LUABIND_NO_ERROR_CHECKING
				o.set_sig_fun(&detail::get_free_function_signature<F>::apply);
#endif

				lua_pushstring(L, m_name);
				lua_gettable(L, -2);

				detail::free_functions::function_rep* rep = nullptr;
				if (lua_iscfunction(L, -1))
				{
					if (lua_getupvalue(L, -1, 2) != nullptr)
					{
						// check the magic number that identifies luabind's functions
						if (lua_touserdata(L, -1) == (void*)0x1337)
						{
							if (lua_getupvalue(L, -2, 1) != 0)
							{
								rep = static_cast<detail::free_functions::function_rep*>(lua_touserdata(L, -1));
								lua_pop(L, 1);
							}
						}
						lua_pop(L, 1);
					}
				}
				lua_pop(L, 1);

				if (rep == nullptr)
				{
					lua_pushstring(L, m_name);
					// create a new function_rep
					rep = static_cast<detail::free_functions::function_rep*>(lua_newuserdata(L, sizeof(detail::free_functions::function_rep)));
					new(rep) detail::free_functions::function_rep(m_name);

                    // STORE IN REGISTRY
                    lua_pushvalue(L, -1);
                    detail::ref(L);

					detail::class_registry* r = detail::class_registry::get_registry(L);
					assert(r && "you must call luabind::open() prior to any function registrations");
					detail::getref(L, r->lua_function());
					const int ret = lua_setmetatable(L, -2);
					(void)ret;
					assert(ret != 0);

					// this is just a magic number to identify functions that luabind created
					lua_pushlightuserdata(L, (void*)0x1337);

					lua_pushcclosure(L, &free_functions::function_dispatcher, 2);
					lua_settable(L, -3);
				}

				rep->add_overload(o);
			}

			char const* m_name;
			F fun;
            policy_cons<Policies...> policies;
		};
	}


	template <typename C, typename R, typename... A>
	constexpr decltype(auto) cdecl_cast(const C& c, R(C::*f)(A...) const)
	{
		return static_cast<R(__cdecl*)(A...)>(c);
	}

	template<typename Function, typename... Policies>
	scope def(const char* name, const Function f, const detail::policy_cons<Policies...> policies)
	{
		if constexpr (!std::is_function_v<std::remove_pointer_t<Function>>)
		{
			constexpr auto lambda_cast = cdecl_cast(f, &Function::operator());
			return scope(luabind_new<detail::function_commiter<decltype(lambda_cast), Policies...>>(name, std::move(lambda_cast), policies));
		}
		else
		{
			return scope(luabind_new<detail::function_commiter<Function, Policies...>>(name, f, policies));
		}
	}

	template <typename Function>
	scope def(const char* name, const Function f)
	{
		if constexpr (!std::is_function_v<std::remove_pointer_t<Function>>)
		{
			constexpr auto lambda_cast = cdecl_cast(f, &Function::operator());
			return scope(luabind_new<detail::function_commiter<decltype(lambda_cast)>>(name, std::move(lambda_cast), detail::policy_cons<>()));
		}
		else
		{
			return scope(luabind_new<detail::function_commiter<Function>>(name, f, detail::policy_cons<>()));
		}
	}

} // namespace luabind
