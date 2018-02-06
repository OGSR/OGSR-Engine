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

#include <Utils/imdexlib/is_reference_wrapper.hpp>

namespace luabind { namespace detail
{
	template<bool IsReferenceWrapper = false>
	struct unwrap_ref
	{
		template<class T>
		static const T& get(const T& r) { return r; }

		template<class T>
		struct apply
		{
			typedef T type;
		};
	};

	template<>
	struct unwrap_ref<true>
	{
		template<class T>
		static T& get(const std::reference_wrapper<T>& r) { return r.get(); }

		template<class T>
		struct apply
		{
			typedef typename T::type& type;
		};
	};

	template<class T>
	void convert_to_lua(lua_State* L, const T& v)
	{
        using unwrap = unwrap_ref<imdexlib::is_reference_wrapper_v<T>>;
	    using value_type = typename unwrap::template apply<T>::type;
		typename default_policy::generate_converter<value_type, Direction::cpp_to_lua>::type converter;

		converter.apply(L, unwrap::get(v));
	}

	template<int Index, class T, typename... Policies>
	void convert_to_lua_p(lua_State* L, const T& v, const policy_cons<Policies...>)
	{
        using unwrap = unwrap_ref<imdexlib::is_reference_wrapper_v<T>>;
	    using value_type = typename unwrap::template apply<T>::type;
	    using converter_policy = typename find_conversion_policy<Index, Policies...>::type;
		typename converter_policy::template generate_converter<value_type, Direction::cpp_to_lua>::type converter;

		converter.apply(L, unwrap::get(v));
	}
}}
