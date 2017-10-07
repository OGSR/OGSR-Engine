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
#include <lua.hpp>
#include <luabind/luabind.hpp>

#include <luabind/config.hpp>
#include <luabind/detail/policy.hpp>

namespace luabind { namespace detail 
{
	// makes A dependent on B, meaning B will outlive A.
	// internally A stores a reference to B
	template<size_t A, size_t B>
	struct dependency_policy
	{
		static void postcall(lua_State* L, const index_map& indices)
		{
			const int nurse_index = indices[A];
			const int patient = indices[B];

			object_rep* nurse = static_cast<object_rep*>(lua_touserdata(L, nurse_index));
			assert((nurse != 0) && "internal error, please report"); // internal error

			nurse->add_dependency(L, patient);
		}
	};

}}

namespace luabind
{
	template<size_t A, size_t B>
	detail::policy_cons<detail::dependency_policy<A, B>>
	dependency()
	{
		return detail::policy_cons<detail::dependency_policy<A, B>>();
	}

	template<size_t A>
	detail::policy_cons<detail::dependency_policy<0, A>>
	return_internal_reference()
	{
		return detail::policy_cons<detail::dependency_policy<0, A>>();
	}
}
