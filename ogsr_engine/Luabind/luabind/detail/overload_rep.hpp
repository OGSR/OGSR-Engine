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
#include <luabind/detail/overload_rep_base.hpp>
#include <luabind/detail/class_rep.hpp>
#include <luabind/detail/is_indirect_const.hpp>
#include <luabind/detail/calc_has_arg.hpp>

#include <Utils/imdexlib/typelist.hpp>

namespace luabind { namespace detail
{
	// this class represents a specific overload of a member-function.
	struct LUABIND_API overload_rep : public overload_rep_base
	{
        template<typename R, typename T, typename... Args, typename... Policies>
        overload_rep(R(T::*)(Args...), const policy_cons<Policies...> policies)
            : overload_rep(false, imdexlib::typelist<Args...>(), std::integral_constant<int, 1>(), policies)
        {
            
        }

        template<typename R, typename T, typename... Args, typename... Policies>
        overload_rep(R(T::*)(Args...) const, const policy_cons<Policies...> policies)
            : overload_rep(true, imdexlib::typelist<Args...>(), std::integral_constant<int, 1>(), policies)
        {
        }

        template<typename R, typename... Args, typename... Policies>
        overload_rep(R(*)(Args...), const policy_cons<Policies...> policies)
            : overload_rep(false, imdexlib::typelist<Args...>(), std::integral_constant<int, 0>(), policies)
        {
        }

        overload_rep(const overload_rep&) = default;
        overload_rep(overload_rep&&) = default;

        overload_rep& operator= (const overload_rep&) = default;
        overload_rep& operator= (overload_rep&&) = default;

		bool operator==(const overload_rep& o)
		{
			if (o.m_const != m_const) return false;
			if (o.m_arity != m_arity) return false;
			if (o.m_params_.size() != m_params_.size()) return false;
			for (size_t i = 0; i < m_params_.size(); ++i)
			{
				if (!(LUABIND_TYPE_INFO_EQUAL(m_params_[i], o.m_params_[i]))) 
					return false;
			}
			return true;
		}

        template <typename T>
		void set_fun(T&& fun) 
		{ call_fun = std::forward<T>(fun); }

        template <typename T>
		void set_fun_static(T&& fun) 
		{ call_fun_static = std::forward<T>(fun); }

		int call(lua_State* L, bool force_static_call) const;

		bool has_static() const { return static_cast<bool>(call_fun_static); }

	private:

        template <int ResInit, typename... Args, typename... Policies>
        overload_rep(const bool isConst, const imdexlib::typelist<Args...>, const std::integral_constant<int, ResInit>, const policy_cons<Policies...>)
            : call_fun(),
              call_fun_static(),
              m_const(isConst)
        {
            m_params_.reserve(sizeof...(Args));

            const int expander [] = { 0, (m_params_.push_back(&typeid(Args)), 0)... };
            (void) expander;

            m_arity =  calcHasArg<ResInit, 1, sizeof...(Args) , Policies...>();
        }

		// this is the normal function pointer that may be a virtual
#pragma warning(push)
#pragma warning(disable:4251)
		std::function<int(lua_State*)> call_fun;
#pragma warning(pop)

		// this is the optional function pointer that is only set if
		// the first function pointer is virtual. This must always point
		// to a static function.
#pragma warning(push)
#pragma warning(disable:4251)
		std::function<int(lua_State*)> call_fun_static;
#pragma warning(pop)

		// the types of the parameter it takes
#pragma warning(push)
#pragma warning(disable:4251)
		vector_class<LUABIND_TYPE_INFO> m_params_;
#pragma warning(pop)
		// is true if the overload is const (this is a part of the signature)
		bool m_const;
	};
}} // namespace luabind::detail
