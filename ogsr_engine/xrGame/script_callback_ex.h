////////////////////////////////////////////////////////////////////////////
//	Module 		: script_callback_ex.h
//	Created 	: 06.02.2004
//  Modified 	: 11.01.2005
//	Author		: Sergey Zhemeitsev and Dmitriy Iassenev
//	Description : Script callbacks with return value
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_space.h"
#include "script_engine.h"

IC bool compare_safe(const luabind::object& o1, const luabind::object& o2) {
#ifdef LUABIND_09
	if ((luabind::type(o1) == LUA_TNIL) && (luabind::type(o2) == LUA_TNIL))
#else
	if ((o1.type() == LUA_TNIL) && (o2.type() == LUA_TNIL))
#endif
		return true;

	return o1 == o2;
}

template <typename _return_type>
class CScriptCallbackEx_ {
public:
	using return_type = _return_type;

private:
	using functor_type = luabind::functor<_return_type>;
	using object_type = luabind::object;
	using unspecified_bool_type = bool(CScriptCallbackEx_::*)() const;

protected:
	functor_type m_functor;
	object_type m_object;

private:
	bool empty() const {
#ifdef LUABIND_09
		return !!m_functor.interpreter();
#else
		return !!m_functor.lua_state();
#endif
	}

public:
	CScriptCallbackEx_() = default;
	CScriptCallbackEx_(const CScriptCallbackEx_& callback) {
		clear();
		*this = callback;
	}
	virtual ~CScriptCallbackEx_() = default;
	CScriptCallbackEx_& operator=(const CScriptCallbackEx_& callback) {
		clear();

#ifdef LUABIND_09
		if (callback.m_functor.is_valid() && callback.m_functor.interpreter())
#else
		if (callback.m_functor.is_valid() && callback.m_functor.lua_state())
#endif
			m_functor = callback.m_functor;

#ifdef LUABIND_09
		if (callback.m_object.is_valid() && callback.m_object.interpreter())
#else
		if (callback.m_object.is_valid() && callback.m_object.lua_state())
#endif
			m_object = callback.m_object;

		return *this;
	}

	bool operator==(const CScriptCallbackEx_& callback) const {
		return compare_safe(m_object, (callback.m_object)) && m_functor == (callback.m_functor);
	}

	bool operator==(const object_type& object) const { return compare_safe(m_object, object); }
	void set(const functor_type& functor) {
		clear();
		m_functor = functor;
	}

	void set(const functor_type& functor, const object_type& object) {
		clear();

		m_functor = functor;
		m_object = object;
	}

	void clear() {
		m_functor.~functor_type();
		new (&m_functor) functor_type();

		m_object.~object_type();
		new (&m_object) object_type();
	}

	operator unspecified_bool_type() const {
		return !m_functor.is_valid() ? 0 : &CScriptCallbackEx_::empty;
	}
};

template <typename _return_type>
class CScriptCallbackEx : public CScriptCallbackEx_<_return_type> {
	using inherited = CScriptCallbackEx_<_return_type>;
	using return_type = typename inherited::return_type;
	using inherited::m_functor;
	using inherited::m_object;

	template <typename T>
	static void do_return(T&& value, std::true_type /*is_void*/) {
		(void)value;
	}

	template <typename T>
	static return_type do_return(T&& value, std::false_type /*is_void*/) {
#pragma warning(push)
#pragma warning(disable:4800)
		return return_type(std::forward<T>(value));
#pragma warning(pop)
	}

	template <typename T>
	static decltype(auto) do_return(T&& value) {
		return do_return(std::forward<T>(value), std::is_void<return_type>());
	}
public:
	template <typename... Args>
	return_type operator()(Args&&... args) const {
		try {
			try {
				if (m_functor) {
					VERIFY(m_functor.is_valid());
					if (m_object.is_valid()) {
						VERIFY(m_object.is_valid());
#ifdef LUABIND_09
						return m_functor(m_object, std::forward<Args>(args)...);
#else
						return do_return(m_functor(m_object, std::forward<Args>(args)...));
#endif
					}
					else
#ifdef LUABIND_09
						return m_functor(std::forward<Args>(args)...);
#else
						return do_return(m_functor(std::forward<Args>(args)...));
#endif
				}
			}
			catch (std::exception&) {
				ai().script_engine().print_output(ai().script_engine().lua(), "", LUA_ERRRUN);
			}
		}
#ifndef LUABIND_NO_EXCEPTIONS
		catch (luabind::error& e) {
			if (e.state())
				ai().script_engine().print_output(e.state(), "", LUA_ERRRUN);
			else
				ai().script_engine().print_output(ai().script_engine().lua(), "", LUA_ERRRUN);
		}
#endif
		catch (...) {
			const_cast<CScriptCallbackEx<return_type>*>(this)->clear();
		}
		return do_return(0);
	}

	template <typename... Args>
	return_type operator()(Args&&... args) {
		try {
			try {
				if (m_functor) {
					VERIFY(m_functor.is_valid());
					if (m_object.is_valid()) {
						VERIFY(m_object.is_valid());
#ifdef LUABIND_09
						return m_functor(m_object, std::forward<Args>(args)...);
#else
						return do_return(m_functor(m_object, std::forward<Args>(args)...));
#endif
					}
					else
#ifdef LUABIND_09
						return m_functor(std::forward<Args>(args)...);
#else
						return do_return(m_functor(std::forward<Args>(args)...));
#endif
				}
			}
			catch (std::exception&) {
				ai().script_engine().print_output(ai().script_engine().lua(), "", LUA_ERRRUN);
			}
		}
#ifndef LUABIND_NO_EXCEPTIONS
		catch (luabind::error& e) {
			if (e.state())
				ai().script_engine().print_output(e.state(), "", LUA_ERRRUN);
			else
				ai().script_engine().print_output(ai().script_engine().lua(), "", LUA_ERRRUN);
		}
#endif
		catch (...) {
			const_cast<CScriptCallbackEx<return_type>*>(this)->clear();
		}
		return do_return(0);
	}
};
