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

#pragma once

#include <luabind/config.hpp>
#include <lua.hpp>

namespace luabind { 
    
    struct scope; 

} // namespace luabind

namespace luabind { namespace detail {

    struct LUABIND_API registration
    {
        registration();
        registration(const registration&) = delete;
        virtual ~registration();

    protected:
        virtual void register_(lua_State*) const = 0;

    private:
        friend struct scope;
        registration* m_next;
    };

}} // namespace luabind::detail

namespace luabind {

    struct LUABIND_API scope
    {
        scope() noexcept;
        explicit scope(detail::registration* reg) noexcept;
        scope(scope const& other_) = delete;
        scope& operator= (const scope&) = delete;

        scope(scope&& other_) noexcept
            : m_chain(other_.m_chain)
        {
            other_.m_chain = nullptr;
        }

        scope& operator= (scope&& that) noexcept
        {
            std::swap(m_chain, that.m_chain);
            return *this;
        }

        ~scope();

        scope&& operator,(scope&& s) &&;

        void register_(lua_State* L) const;

    private:
        detail::registration* m_chain;
    };

    class LUABIND_API namespace_ : public scope
    {
    public:
        explicit namespace_(char const* name);
        namespace_(const namespace_&) = delete;

        namespace_(namespace_&& that) noexcept
            : scope(std::move(that)),
              m_registration(that.m_registration)
        {
            that.m_registration = nullptr;
        }

        namespace_&& operator[](scope&& s) &&;

        namespace_& operator= (const namespace_&) = delete;

        namespace_& operator= (namespace_&& that) noexcept
        {
            scope::operator= (std::move(that));
            std::swap(m_registration, that.m_registration);
            return *this;
        }

    private:
        struct registration_;

        namespace_(registration_*);

        registration_* m_registration;
    };

    class LUABIND_API module_
    {
    public:
        module_(lua_State* L_, char const* name);
        void operator[](scope&& s);

        module_(const module_&) = delete;

        module_(module_&& that) noexcept
            : m_state(that.m_state),
              m_name(that.m_name)
        {
            that.m_state = nullptr;
            that.m_name = nullptr;
        }

        module_& operator= (const module_&) = delete;

        module_& operator= (module_&& that) noexcept
        {
            m_state = that.m_state;
            m_name = that.m_name;
            that.m_state = nullptr;
            that.m_name = nullptr;
            return *this;
        }

    private:
        lua_State* m_state;
        char const* m_name;
    };

    inline module_ module(lua_State* L, char const* name = 0)
    {
        return module_(L, name);
    }

} // namespace luabind
