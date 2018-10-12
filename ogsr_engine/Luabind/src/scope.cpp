// Copyright (c) 2004 Daniel Wallin and Arvid Norberg

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
#include "stdafx.h"

#include <luabind/luabind.hpp>

#include <luabind/scope.hpp>
#include <luabind/detail/debug.hpp>
#include <luabind/detail/stack_utils.hpp>

namespace luabind { namespace detail {

    registration::registration()
        : m_next(nullptr)
    {
    }

    registration::~registration()
    {
        luabind_delete	(m_next);
    }

    } // namespace detail
    
    scope::scope() noexcept
        : m_chain(nullptr)
    {
    }
    
    scope::scope(detail::registration* reg) noexcept
        : m_chain(reg)
    {
    }

    scope::~scope()
    {
        luabind_delete	(m_chain);
    }
    
    scope&& scope::operator,(scope&& s) &&
    {
        if (!m_chain) 
        {
            m_chain = s.m_chain;
            s.m_chain = nullptr;
            return std::move(*this);
        }
        
        for (detail::registration* c = m_chain;; c = c->m_next)
        {
            if (!c->m_next)
            {
                c->m_next = s.m_chain;
                s.m_chain = nullptr;
                break;
            }
        }

        return std::move(*this);
    }

    void scope::register_(lua_State* L) const
    {
        for (detail::registration* r = m_chain; r != nullptr; r = r->m_next)
        {
			LUABIND_CHECK_STACK(L);
            r->register_(L);
        }
    }

} // namespace luabind

namespace luabind {

    namespace {

        struct lua_pop_stack
        {
            lua_pop_stack(lua_State* L)
                : m_state(L)
            {
            }

            ~lua_pop_stack()
            {
                lua_pop(m_state, 1);
            }

            lua_State* m_state;
        };

    } // namespace unnamed
    
    module_::module_(lua_State* L, char const* name = 0)
        : m_state(L)
        , m_name(name)
    {
    }

    void module_::operator[](scope&& s)
    {
        if (m_name)
        {
            lua_pushstring(m_state, m_name);
            lua_gettable(m_state, LUA_GLOBALSINDEX);

            if (!lua_istable(m_state, -1))
            {
                lua_pop(m_state, 1);

                lua_newtable(m_state);
                lua_pushstring(m_state, m_name);
                lua_pushvalue(m_state, -2);
                lua_settable(m_state, LUA_GLOBALSINDEX);
            }
        }
        else
        {
            lua_pushvalue(m_state, LUA_GLOBALSINDEX);
        }

        lua_pop_stack guard(m_state);

		if (::luabind::get_pregister_callback())
			::luabind::get_pregister_callback()	(m_state,true);

		s.register_(m_state);

		if (::luabind::get_pregister_callback())
			::luabind::get_pregister_callback()	(m_state,false);
	}

    struct namespace_::registration_ : detail::registration
    {
        registration_(char const* name)
            : m_name(name)
        {
        }

        void register_(lua_State* L) const
        {
			LUABIND_CHECK_STACK(L);
            assert(lua_gettop(L) >= 1);

            lua_pushstring(L, m_name);
            lua_gettable(L, -2);

			detail::stack_pop p(L, 1); // pops the table on exit

            if (!lua_istable(L, -1))
            {
                lua_pop(L, 1);

                lua_newtable(L);
                lua_pushstring(L, m_name);
                lua_pushvalue(L, -2);
                lua_settable(L, -4);
            }

            m_scope.register_(L);
        }

        char const* m_name;
        scope m_scope;
    };

    namespace_::namespace_(char const* name)
        : namespace_(luabind_new<registration_>(name))
    {
    }

    namespace_::namespace_(registration_* reg)
        : scope(reg),
          m_registration(reg)
    {
    }

    namespace_&& namespace_::operator[](scope&& s) &&
    {
        m_registration->m_scope = std::move(m_registration->m_scope).operator,(std::move(s));
        return std::move(*this);
    }

} // namespace luabind

