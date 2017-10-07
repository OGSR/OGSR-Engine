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

namespace luabind { namespace detail
{
	template<class T>
	struct decorated_type
	{
		static by_value<T> t;
		static by_value<T>& get() { return /*by_value<T>()*/t; }
	};

	template<class T>
	by_value<T> decorated_type<T>::t;

	template<class T>
	struct decorated_type<T*>
	{
		static by_pointer<T> t;
		static by_pointer<T>& get() { return /*by_pointer<T>()*/t; }
	};

	template<class T>
	by_pointer<T> decorated_type<T*>::t;

	template<class T>
	struct decorated_type<const T*>
	{
		static by_const_pointer<T> t;
		static by_const_pointer<T> get() { return /*by_const_pointer<T>()*/t; }
	};

	template<class T>
	by_const_pointer<T> decorated_type<const T*>::t;

	template<class T>
	struct decorated_type<const T* const>
	{
		static by_const_pointer<T> t;
		static by_const_pointer<T>& get() { return /*by_const_pointer<T>()*/t; }
	};

	template<class T>
	by_const_pointer<T> decorated_type<const T* const>::t;

	template<class T>
	struct decorated_type<T&>
	{
		static by_reference<T> t;
		static by_reference<T>& get() { return /*by_reference<T>()*/t; }
	};

	template<class T>
	by_reference<T> decorated_type<T&>::t;

	template<class T>
	struct decorated_type<const T&>
	{
		static by_const_reference<T> t;
		static by_const_reference<T>& get() { return /*by_const_reference<T>()*/t; }
	};

	template<class T>
	by_const_reference<T> decorated_type<const T&>::t;

	#define LUABIND_DECORATE_TYPE(t) luabind::detail::decorated_type<t>::get()
}}
