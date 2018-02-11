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

#include <typeinfo>

#include <luabind/detail/class_registry.hpp>
#include <luabind/detail/primitives.hpp>
#include <luabind/detail/object_rep.hpp>
#include <luabind/detail/typetraits.hpp>
#include <luabind/detail/class_cache.hpp>
#include <luabind/detail/debug.hpp>

#include <luabind/detail/decorate_type.hpp>
#include <luabind/object.hpp>
#include <luabind/weak_ref.hpp>
#include <luabind/back_reference_fwd.hpp>
#include <luabind/detail/policy_cons.hpp>

namespace luabind
{
	namespace detail
	{
		struct conversion_policy_base {};
	}

	template<size_t N, bool HasArg = true>
	struct conversion_policy : detail::conversion_policy_base
	{
        static constexpr int index = N;
        static constexpr bool has_arg = HasArg;
	};

	class index_map
	{
	public:

		index_map(const int* m): m_map(m) {}

		int operator[](int index) const
		{
			return m_map[index];
		}

	private:

		const int* m_map;
	};

	namespace converters
	{
		using luabind::detail::by_value;
		using luabind::detail::by_reference;
		using luabind::detail::by_const_reference;
		using luabind::detail::by_pointer;
		using luabind::detail::by_const_pointer;

		std::false_type is_user_defined(...);
	}

	namespace detail
	{
		template<class T>
		struct is_user_defined
		{
            static constexpr bool value = std::is_same_v<decltype(converters::is_user_defined(LUABIND_DECORATE_TYPE(T))), std::true_type>;
		};

		LUABIND_API int implicit_cast(const class_rep* crep, LUABIND_TYPE_INFO const&, int& pointer_offset);
	}

	 template<class T> class functor;
	 class object;
	 class weak_ref;
}

namespace luabind { namespace detail
{
	template<class>
	struct is_primitive;

	template<class T>
	std::true_type is_lua_functor_test(const functor<T>&);

	template<class T>
    std::false_type is_lua_functor_test(const T&);

	template<class T>
	struct is_lua_functor
	{
        static constexpr bool value = decltype(is_lua_functor_test(std::declval<T>()))::value;
	};

	namespace
	{
		static char msvc_fix[64];
	}

	template<class T>
	struct indirect_type
	{
        using result_type = std::conditional_t<
            is_primitive<T>::value,
            const type<T>&,
            std::conditional_t<
                std::is_reference_v<T> || std::is_pointer_v<T>,
                typename identity<T>::type,
                std::add_lvalue_reference_t<T>
            >
        >;

		static result_type get()
		{
			return reinterpret_cast<result_type>(msvc_fix);
		}
	};

	template<class T>
	struct is_primitive
	{
        static constexpr bool value = std::is_array_v<T> && std::is_convertible_v<T, const char*>;
	};

#define LUABIND_INTEGER_TYPE(type) \
	template<> struct is_primitive<type> : std::true_type {}; \
	template<> struct is_primitive<type const> : std::true_type {}; \
	template<> struct is_primitive<type const&> : std::true_type {}; \
	template<> struct is_primitive<unsigned type> : std::true_type {}; \
	template<> struct is_primitive<unsigned type const> : std::true_type {}; \
	template<> struct is_primitive<unsigned type const&> : std::true_type {};

	LUABIND_INTEGER_TYPE(char)
	LUABIND_INTEGER_TYPE(short)
	LUABIND_INTEGER_TYPE(int)
	LUABIND_INTEGER_TYPE(long)
	LUABIND_INTEGER_TYPE(long long)

	template<> struct is_primitive<signed char> : std::true_type {}; \
	template<> struct is_primitive<signed char const> : std::true_type {}; \
	template<> struct is_primitive<signed char const&> : std::true_type {}; \
	
#undef LUABIND_INTEGER_TYPE
	
	template<> struct is_primitive<luabind::object>: std::true_type {};
	template<> struct is_primitive<const luabind::object>: std::true_type {};
	template<> struct is_primitive<const luabind::object&>: std::true_type {};

	template<> struct is_primitive<luabind::weak_ref>: std::true_type {};
	template<> struct is_primitive<const luabind::weak_ref>: std::true_type {};
	template<> struct is_primitive<const luabind::weak_ref&>: std::true_type {};
	
	template<> struct is_primitive<float>: std::true_type {};
	template<> struct is_primitive<double>: std::true_type {};
	template<> struct is_primitive<long double>: std::true_type {};
	template<> struct is_primitive<char*>: std::true_type {};
	template<> struct is_primitive<bool>: std::true_type {};

	template<> struct is_primitive<const float>: std::true_type {};
	template<> struct is_primitive<const double>: std::true_type {};
	template<> struct is_primitive<const long double>: std::true_type {};
	template<> struct is_primitive<const char*>: std::true_type {};
	template<> struct is_primitive<const char* const>: std::true_type {};
	template<> struct is_primitive<const bool>: std::true_type {};

	// TODO: add more
	template<> struct is_primitive<const float&>: std::true_type {};
	template<> struct is_primitive<const double&>: std::true_type {};
	template<> struct is_primitive<const long double&>: std::true_type {};
	template<> struct is_primitive<const bool&>: std::true_type {};

	template<> struct is_primitive<const string_class&>: std::true_type {};
	template<> struct is_primitive<string_class>: std::true_type {};
	template<> struct is_primitive<const string_class>: std::true_type {};

	template<> struct is_primitive<const std::string&> : std::true_type {};
	template<> struct is_primitive<std::string> : std::true_type {};
	template<> struct is_primitive<const std::string> : std::true_type {};


	template<Direction> struct primitive_converter;
	
	template<>
	struct primitive_converter<Direction::cpp_to_lua>
	{
		void apply(lua_State* L, const luabind::object& v)
		{
			// if the luabind::object is uninitialized
			// treat it as nil.
			if (v.lua_state() == 0)
			{
				lua_pushnil(L);
				return;
			}
			// if you hit this assert you are trying to return a value from one state into another lua state
			assert((v.lua_state() == L) && "you cannot return an uninitilized value "
				"or a value from one lua state into another");
			v.pushvalue();
		}
		void apply(lua_State* L, int v) { lua_pushnumber(L, (lua_Number)v); }
		void apply(lua_State* L, short v) { lua_pushnumber(L, (lua_Number)v); }
		void apply(lua_State* L, char v) { lua_pushnumber(L, (lua_Number)v); }
		void apply(lua_State* L, long v) { lua_pushnumber(L, (lua_Number)v); }
		void apply(lua_State* L, long long v) { lua_pushnumber(L, (lua_Number)v); }
		void apply(lua_State* L, unsigned int v) { lua_pushnumber(L, (lua_Number)v); }
		void apply(lua_State* L, unsigned short v) { lua_pushnumber(L, (lua_Number)v); }
		void apply(lua_State* L, unsigned char v) { lua_pushnumber(L, (lua_Number)v); }
		void apply(lua_State* L, unsigned long v) { lua_pushnumber(L, (lua_Number)v); }
		void apply(lua_State* L, unsigned long long v) { lua_pushnumber(L, (lua_Number)v); }
		void apply(lua_State* L, float v) { lua_pushnumber(L, (lua_Number)v); }
		void apply(lua_State* L, double v) { lua_pushnumber(L, (lua_Number)v); }
		void apply(lua_State* L, long double v) { lua_pushnumber(L, (lua_Number)v); }
		void apply(lua_State* L, const char* v) { lua_pushstring(L, v); }
		void apply(lua_State* L, const string_class& v)
		{ lua_pushlstring(L, v.data(), v.size()); }
		void apply(lua_State* L, const std::string& v)
		{ lua_pushlstring(L, v.data(), v.size()); }
		void apply(lua_State* L, bool b) { lua_pushboolean(L, b); }
	};

	template<>
	struct primitive_converter<Direction::lua_to_cpp>
	{
		#define PRIMITIVE_CONVERTER(prim) \
			prim apply(lua_State* L, luabind::detail::by_const_reference<prim>, int index) { return apply(L, detail::by_value<prim>(), index); } \
			prim apply(lua_State* L, luabind::detail::by_value<const prim>, int index) { return apply(L, detail::by_value<prim>(), index); } \
			prim apply(lua_State* L, luabind::detail::by_value<prim>, int index)

		#define PRIMITIVE_MATCHER(prim) \
			static int match(lua_State* L, luabind::detail::by_const_reference<prim>, int index) { return match(L, detail::by_value<prim>(), index); } \
			static int match(lua_State* L, luabind::detail::by_value<const prim>, int index) { return match(L, detail::by_value<prim>(), index); } \
			static int match(lua_State* L, luabind::detail::by_value<prim>, int index)

		PRIMITIVE_CONVERTER(bool) { return lua_toboolean(L, index) == 1; }
		PRIMITIVE_MATCHER(bool) { if (lua_type(L, index) == LUA_TBOOLEAN) return 0; else return -1; }

		PRIMITIVE_CONVERTER(int) { return static_cast<int>(lua_tonumber(L, index)); }
		PRIMITIVE_MATCHER(int)
		{
            const int type = lua_type(L, index);
		    if (type == LUA_TNUMBER) return 0; else return -1;
		}

		PRIMITIVE_CONVERTER(unsigned int) { return static_cast<unsigned int>(lua_tonumber(L, index)); }
		PRIMITIVE_MATCHER(unsigned int) { if (lua_type(L, index) == LUA_TNUMBER) return 0; else return -1; }

		PRIMITIVE_CONVERTER(char) { return static_cast<char>(lua_tonumber(L, index)); }
		PRIMITIVE_MATCHER(char) { if (lua_type(L, index) == LUA_TNUMBER) return 0; else return -1; }

		PRIMITIVE_CONVERTER(signed char) { return static_cast<char>(lua_tonumber(L, index)); }
		PRIMITIVE_MATCHER(signed char) { if (lua_type(L, index) == LUA_TNUMBER) return 0; else return -1; }
		
		PRIMITIVE_CONVERTER(unsigned char) { return static_cast<unsigned char>(lua_tonumber(L, index)); }
		PRIMITIVE_MATCHER(unsigned char) { if (lua_type(L, index) == LUA_TNUMBER) return 0; else return -1; }

		PRIMITIVE_CONVERTER(short) { return static_cast<short>(lua_tonumber(L, index)); }
		PRIMITIVE_MATCHER(short) { if (lua_type(L, index) == LUA_TNUMBER) return 0; else return -1; }

		PRIMITIVE_CONVERTER(unsigned short) { return static_cast<unsigned short>(lua_tonumber(L, index)); }
		PRIMITIVE_MATCHER(unsigned short) { if (lua_type(L, index) == LUA_TNUMBER) return 0; else return -1; }

		PRIMITIVE_CONVERTER(long) { return static_cast<long>(lua_tonumber(L, index)); }
		PRIMITIVE_MATCHER(long) { if (lua_type(L, index) == LUA_TNUMBER) return 0; else return -1; }

		PRIMITIVE_CONVERTER(unsigned long) { return static_cast<unsigned long>(lua_tonumber(L, index)); }
		PRIMITIVE_MATCHER(unsigned long) { if (lua_type(L, index) == LUA_TNUMBER) return 0; else return -1; }

		PRIMITIVE_CONVERTER(long long) { return static_cast<long long>(lua_tonumber(L, index)); }
		PRIMITIVE_MATCHER(long long) { if (lua_type(L, index) == LUA_TNUMBER) return 0; else return -1; }

		PRIMITIVE_CONVERTER(unsigned long long) { return static_cast<unsigned long long>(lua_tonumber(L, index)); }
		PRIMITIVE_MATCHER(unsigned long long) { if (lua_type(L, index) == LUA_TNUMBER) return 0; else return -1; }

		PRIMITIVE_CONVERTER(float) { return static_cast<float>(lua_tonumber(L, index)); }
		PRIMITIVE_MATCHER(float) { if (lua_type(L, index) == LUA_TNUMBER) return 0; else return -1; }

		PRIMITIVE_CONVERTER(double) { return static_cast<double>(lua_tonumber(L, index)); }
		PRIMITIVE_MATCHER(double) { if (lua_type(L, index) == LUA_TNUMBER) return 0; else return -1; }

		PRIMITIVE_CONVERTER(string_class)
		{ return string_class(lua_tostring(L, index), lua_strlen(L, index)); }
		PRIMITIVE_MATCHER(string_class) { if (lua_type(L, index) == LUA_TSTRING) return 0; else return -1; }

		PRIMITIVE_CONVERTER(std::string)
		{ return std::string(lua_tostring(L, index), lua_strlen(L, index)); }
		PRIMITIVE_MATCHER(std::string) { if (lua_type(L, index) == LUA_TSTRING) return 0; else return -1; }

		PRIMITIVE_CONVERTER(luabind::object)
		{
			LUABIND_CHECK_STACK(L);

			lua_pushvalue(L, index);
			detail::lua_reference ref;
			ref.set(L);
			return luabind::object(L, ref, true);
		}

		PRIMITIVE_MATCHER(luabind::object)
		{
			(void)index;
			(void)L;
			//return std::numeric_limits<int>::max() / LUABIND_MAX_ARITY;
            constexpr const int kMaxArity = 1000;
            return std::numeric_limits<int>::max() / kMaxArity;
		}

		PRIMITIVE_CONVERTER(luabind::weak_ref)
		{
			LUABIND_CHECK_STACK(L);
			return luabind::weak_ref(L, index);
		}

		PRIMITIVE_MATCHER(luabind::weak_ref) { (void)index; (void)L; return std::numeric_limits<int>::max() - 1; }
		
		const char* apply(lua_State* L, detail::by_const_pointer<char>, int index) { return static_cast<const char*>(lua_tostring(L, index)); }
		const char* apply(lua_State* L, detail::by_const_pointer<const char>, int index) { return static_cast<const char*>(lua_tostring(L, index)); }
		static int match(lua_State* L, by_const_pointer<char>, int index) { if (lua_type(L, index) == LUA_TSTRING) return 0; else return -1;}
		static int match(lua_State* L, by_const_pointer<const char>, int index) { if (lua_type(L, index) == LUA_TSTRING) return 0; else return -1;}

		template<class T>
		void converter_postcall(lua_State*, T, int) {}
		
		#undef PRIMITIVE_MATCHER
		#undef PRIMITIVE_CONVERTER
	};

// ********** user defined converter ***********

	template<Direction Dir> struct user_defined_converter;
	
	template<>
	struct user_defined_converter<Direction::lua_to_cpp>
	{
		template<class T>
		T apply(lua_State* L, detail::by_value<T>, int index) 
		{ 
//			std::cerr << "user_defined_converter\n";
			return converters::convert_lua_to_cpp(L, detail::by_value<T>(), index);
		}

		template<class T>
		T apply(lua_State* L, detail::by_reference<T>, int index) 
		{ 
//			std::cerr << "user_defined_converter\n";
			return converters::convert_lua_to_cpp(L, detail::by_reference<T>(), index);
		}

		template<class T>
		T apply(lua_State* L, detail::by_const_reference<T>, int index) 
		{ 
//			std::cerr << "user_defined_converter\n";
			return converters::convert_lua_to_cpp(L, detail::by_const_reference<T>(), index);
		}

		template<class T>
		T* apply(lua_State* L, detail::by_pointer<T>, int index) 
		{ 
//			std::cerr << "user_defined_converter\n";
			return converters::convert_lua_to_cpp(L, detail::by_pointer<T>(), index);
		}

		template<class T>
		const T* apply(lua_State* L, detail::by_const_pointer<T>, int index) 
		{ 
//			std::cerr << "user_defined_converter\n";
			return converters::convert_lua_to_cpp(L, detail::by_pointer<T>(), index);
		}

		template<class T>
		static int match(lua_State* L, T, int index)
		{
			return converters::match_lua_to_cpp(L, T(), index);
		}

		template<class T>
		void converter_postcall(lua_State*, T, int) {}
	};

	template<>
	struct user_defined_converter<Direction::cpp_to_lua>
	{
			template<class T>
			void apply(lua_State* L, const T& v) 
			{ 
				converters::convert_cpp_to_lua(L, v);
			}
	};

// ********** pointer converter ***********


	template<Direction Dir> struct pointer_converter;

	template<>
	struct pointer_converter<Direction::cpp_to_lua>
	{
		template<class T>
		void apply(lua_State* L, T* ptr)
		{
			if (ptr == 0) 
			{
				lua_pushnil(L);
				return;
			}

			if (back_reference<T>::extract(L, ptr))
				return;

			class_rep* crep = get_class_rep<T>(L);

			// if you get caught in this assert you are
			// trying to use an unregistered type
			assert(crep && "you are trying to use an unregistered type");

			// create the struct to hold the object
			void* obj = lua_newuserdata(L, sizeof(object_rep));
			//new(obj) object_rep(ptr, crep, object_rep::owner, destructor_s<T>::apply);
			new(obj) object_rep(ptr, crep, 0, nullptr);

			// set the meta table
			detail::getref(L, crep->metatable_ref());
			lua_setmetatable(L, -2);

//			make_instance(L, ptr, (pointer_holder<T, T*>*)0);
		}
	};

	template<>
	struct pointer_converter<Direction::lua_to_cpp>
	{
		// TODO: does the pointer converter need this?!
		char target[32];
		void (*destructor)(void *);

		pointer_converter(): destructor(nullptr) {}

		template<class T>
		std::add_pointer_t<T> apply(lua_State* L, by_pointer<T>, int index)
		{
			// preconditions:
			//	lua_isuserdata(L, index);
			// getmetatable().__lua_class is true
			// object_rep->flags() & object_rep::constant == 0

			if (lua_isnil(L, index)) return 0;
			
			object_rep* obj = static_cast<object_rep*>(lua_touserdata(L, index));
			assert((obj != nullptr) && "internal error, please report"); // internal error
			const class_rep* crep = obj->crep();

			T* ptr = reinterpret_cast<T*>(crep->convert_to(LUABIND_TYPEID(T), obj, target));

			if ((void*)ptr == (char*)target) destructor = detail::destruct_only_s<T>::apply;
			assert(!destructor || sizeof(T) <= 32);

			return ptr;
		}

		template<class T>
		static int match(lua_State* L, by_pointer<T>, int index)
		{
			if (lua_isnil(L, index)) return 0;
			object_rep* obj = is_class_object(L, index);
			if (obj == nullptr) return -1;
			// cannot cast a constant object to nonconst
			if (obj->flags() & object_rep::constant) return -1;

			if (LUABIND_TYPE_INFO_EQUAL(obj->crep()->holder_type(), LUABIND_TYPEID(T)))
				return (obj->flags() & object_rep::constant)?-1:0;
			if (LUABIND_TYPE_INFO_EQUAL(obj->crep()->const_holder_type(), LUABIND_TYPEID(T)))
				return (obj->flags() & object_rep::constant)?0:-1;

			int d;
			return implicit_cast(obj->crep(), LUABIND_TYPEID(T), d);
		}

		~pointer_converter() noexcept(false)
		{
			if (destructor) destructor(target);
		}

		template<class T>
		void converter_postcall(lua_State*, by_pointer<T>, int) 
		{}
	};

// ******* value converter *******

	template<Direction Dir> struct value_converter;

	template<>
	struct value_converter<Direction::cpp_to_lua>
	{
		template<class T>
		void apply(lua_State* L, const T& ref)
		{
			if (back_reference<T>::extract(L, &ref))
				return;

			class_rep* crep = get_class_rep<T>(L);

			// if you get caught in this assert you are
			// trying to use an unregistered type
			assert(crep && "you are trying to use an unregistered type");

			void* obj_rep;
			void* held;

			std::tie(obj_rep,held) = crep->allocate(L);

			void* object_ptr;
			void(*destructor)(void*);
			destructor = crep->destructor();
			int flags = object_rep::owner;
			if (crep->has_holder())
			{
				new(held) T(ref);
				object_ptr = held;
				if (LUABIND_TYPE_INFO_EQUAL(LUABIND_TYPEID(T), crep->const_holder_type()))
				{
					flags |= object_rep::constant;
					destructor = crep->const_holder_destructor();
				}
			}
			else
			{
				object_ptr = luabind_new<T>(ref);
			}
			new(obj_rep) object_rep(object_ptr, crep, flags, destructor);

			// set the meta table
			detail::getref(L, crep->metatable_ref());
			lua_setmetatable(L, -2);
		}
	};

	template<class T>
	struct destruct_guard
	{
		T* ptr;
		bool dismiss;
		destruct_guard(T* p): ptr(p), dismiss(false) {}

		~destruct_guard() noexcept(std::is_nothrow_destructible_v<T>)
		{
			if (!dismiss)
				ptr->~T();
		}
	};

	template<>
	struct value_converter<Direction::lua_to_cpp>
	{
		template<class T>
		/*typename make_const_reference<T>::type*/T apply(lua_State* L, by_value<T>, int index)
		{
			// preconditions:
			//	lua_isuserdata(L, index);
			// getmetatable().__lua_class is true
			// object_rep->flags() & object_rep::constant == 0

			object_rep* obj = nullptr;
			const class_rep* crep = nullptr;

			// special case if we get nil in, try to convert the holder type
			if (lua_isnil(L, index))
			{
				crep = get_class_rep<T>(L);
				assert(crep);
			}
			else
			{
				obj = static_cast<object_rep*>(lua_touserdata(L, index));
				assert((obj != nullptr) && "internal error, please report"); // internal error
				crep = obj->crep();
			}
			assert(crep);

			// TODO: align!
			char target[sizeof(T)];
			T* ptr = reinterpret_cast<T*>(crep->convert_to(LUABIND_TYPEID(T), obj, target));

			destruct_guard<T> guard(ptr);
			if ((void*)ptr != (void*)target) guard.dismiss = true;

			return *ptr;
		}

		template<class T>
		static int match(lua_State* L, by_value<T>, int index)
		{
			// special case if we get nil in, try to match the holder type
			if (lua_isnil(L, index))
			{
				class_rep* crep = get_class_rep<T>(L);
				if (crep == nullptr) return -1;
				if (LUABIND_TYPE_INFO_EQUAL(crep->holder_type(), LUABIND_TYPEID(T)))
					return 0;
				if (LUABIND_TYPE_INFO_EQUAL(crep->const_holder_type(), LUABIND_TYPEID(T)))
					return 0;
				return -1;
			}

			object_rep* obj = is_class_object(L, index);
			if (obj == nullptr) return -1;
			int d;

			if (LUABIND_TYPE_INFO_EQUAL(obj->crep()->holder_type(), LUABIND_TYPEID(T)))
				return (obj->flags() & object_rep::constant)?-1:0;
			if (LUABIND_TYPE_INFO_EQUAL(obj->crep()->const_holder_type(), LUABIND_TYPEID(T)))
				return (obj->flags() & object_rep::constant)?0:1;

			return implicit_cast(obj->crep(), LUABIND_TYPEID(T), d);	
		}

		template<class T>
		void converter_postcall(lua_State*, T, int) {}
	};

// ******* const pointer converter *******

	template<Direction Dir> struct const_pointer_converter;

	template<>
	struct const_pointer_converter<Direction::cpp_to_lua>
	{
		template<class T>
		void apply(lua_State* L, const T* ptr)
		{
			if (ptr == nullptr)
			{
				lua_pushnil(L);
				return;
			}

			if (back_reference<T>::extract(L, ptr))
				return;

			class_rep* crep = get_class_rep<T>(L);

			// if you get caught in this assert you are
			// trying to use an unregistered type
			assert(crep && "you are trying to use an unregistered type");

			// create the struct to hold the object
			void* obj = lua_newuserdata(L, sizeof(object_rep));
			assert(obj && "internal error, please report");
			// we send 0 as destructor since we know it will never be called
			new(obj) object_rep(const_cast<T*>(ptr), crep, object_rep::constant, nullptr);

			// set the meta table
			detail::getref(L, crep->metatable_ref());
			lua_setmetatable(L, -2);
		}
	};


	template<>
	struct const_pointer_converter<Direction::lua_to_cpp>
		: private pointer_converter<Direction::lua_to_cpp>
	{
		template<class T>
		std::add_pointer_t<std::add_const_t<T>> apply(lua_State* L, by_const_pointer<T>, int index)
		{
//			std::cerr << "const_pointer_converter\n";
			return pointer_converter<Direction::lua_to_cpp>::apply(L, by_pointer<T>(), index);
		}

		template<class T>
		static int match(lua_State* L, by_const_pointer<T>, int index)
		{
			if (lua_isnil(L, index)) return 0;
			object_rep* obj = is_class_object(L, index);
			if (obj == nullptr) return -1; // if the type is not one of our own registered types, classify it as a non-match

			if (LUABIND_TYPE_INFO_EQUAL(obj->crep()->holder_type(), LUABIND_TYPEID(T)))
				return (obj->flags() & object_rep::constant)?-1:0;
			if (LUABIND_TYPE_INFO_EQUAL(obj->crep()->const_holder_type(), LUABIND_TYPEID(T)))
				return (obj->flags() & object_rep::constant)?0:1;

            bool const_ = obj->flags() & object_rep::constant;
			int d;
			return implicit_cast(obj->crep(), LUABIND_TYPEID(T), d) + !const_;
		}

		template<class T>
		void converter_postcall(lua_State* L, by_const_pointer<T>, int index) 
		{
			pointer_converter<Direction::lua_to_cpp>::converter_postcall(L, by_pointer<T>(), index);
		}
	};

// ******* reference converter *******

	template<Direction Dir> struct ref_converter;

	template<>
	struct ref_converter<Direction::cpp_to_lua>
	{
		template<class T>
		void apply(lua_State* L, T& ref)
		{
			if (back_reference<T>::extract(L, &ref))
				return;

			class_rep* crep = get_class_rep<T>(L);

			// if you get caught in this assert you are
			// trying to use an unregistered type
			assert(crep && "you are trying to use an unregistered type");

			T* ptr = &ref;

			// create the struct to hold the object
			void* obj = lua_newuserdata(L, sizeof(object_rep));
			assert(obj && "internal error, please report");
			new(obj) object_rep(ptr, crep, 0, 0);

			// set the meta table
			detail::getref(L, crep->metatable_ref());
			lua_setmetatable(L, -2);
		}
	};

	template<>
	struct ref_converter<Direction::lua_to_cpp>
	{
		template<class T>
		std::add_lvalue_reference_t<T> apply(lua_State* L, by_reference<T>, int index)
		{
			assert(!lua_isnil(L, index));
			return *pointer_converter<Direction::lua_to_cpp>().apply(L, by_pointer<T>(), index);
		}

		template<class T>
		static int match(lua_State* L, by_reference<T>, int index)
		{
			if (lua_isnil(L, index)) return -1;
			return pointer_converter<Direction::lua_to_cpp>::match(L, by_pointer<T>(), index);
		}

		template<class T>
		void converter_postcall(lua_State*, T, int) {}
	};

// ******** const reference converter *********

	template<Direction Dir> struct const_ref_converter;

	template<>
	struct const_ref_converter<Direction::cpp_to_lua>
	{
		template<class T>
		void apply(lua_State* L, const T& ref)
		{
			if (back_reference<T>::extract(L, &ref))
				return;

			class_rep* crep = get_class_rep<T>(L);

			// if you get caught in this assert you are
			// trying to use an unregistered type
			assert(crep && "you are trying to use an unregistered type");

			T const* ptr = &ref;

			// create the struct to hold the object
			void* obj = lua_newuserdata(L, sizeof(object_rep));
			assert(obj && "internal error, please report");
			new(obj) object_rep(const_cast<T*>(ptr), crep, object_rep::constant, nullptr);

			// set the meta table
			detail::getref(L, crep->metatable_ref());
			lua_setmetatable(L, -2);
		}
	};

	template<>
	struct const_ref_converter<Direction::lua_to_cpp>
	{
		// TODO: align!
		char target[32];
		void (*destructor)(void*);

		const_ref_converter(): destructor(nullptr) {}

		template<class T>
		std::add_lvalue_reference_t<std::add_const_t<T>> apply(lua_State* L, by_const_reference<T>, int index)
		{
			object_rep* obj = nullptr;
			class_rep const * crep = nullptr;

			// special case if we get nil in, try to convert the holder type
			if (lua_isnil(L, index))
			{
				crep = get_class_rep<T>(L);
				assert(crep);
			}
			else
			{
				obj = static_cast<object_rep*>(lua_touserdata(L, index));
				assert((obj != nullptr) && "internal error, please report"); // internal error
				crep = obj->crep();
			}
			assert(crep);

			T* ptr = reinterpret_cast<T*>(crep->convert_to(LUABIND_TYPEID(T), obj, target));
			// if the pointer returned points into the converter storage,
			// we need to destruct it once the converter destructs
			if ((void*)ptr == (void*)target) destructor = detail::destruct_only_s<T>::apply;
			assert(!destructor || sizeof(T) <= 32);

			return *ptr;
		}

		template<class T>
		static int match(lua_State* L, by_const_reference<T>, int index)
		{
			// special case if we get nil in, try to match the holder type
			if (lua_isnil(L, index))
			{
				class_rep* crep = get_class_rep<T>(L);
				if (crep == nullptr) return -1;
				if (LUABIND_TYPE_INFO_EQUAL(crep->holder_type(), LUABIND_TYPEID(T)))
					return 0;
				if (LUABIND_TYPE_INFO_EQUAL(crep->const_holder_type(), LUABIND_TYPEID(T)))
					return 0;
				return -1;
			}

			object_rep* obj = is_class_object(L, index);
			if (obj == nullptr) return -1; // if the type is not one of our own registered types, classify it as a non-match

			if (LUABIND_TYPE_INFO_EQUAL(obj->crep()->holder_type(), LUABIND_TYPEID(T)))
				return (obj->flags() & object_rep::constant)?-1:0;
			if (LUABIND_TYPE_INFO_EQUAL(obj->crep()->const_holder_type(), LUABIND_TYPEID(T)))
				return (obj->flags() & object_rep::constant)?0:1;

            bool const_ = obj->flags() & object_rep::constant;
			int d;
			return implicit_cast(obj->crep(), LUABIND_TYPEID(T), d) + !const_;
		}

		~const_ref_converter() noexcept(false)
		{
			if (destructor) destructor(target);
		}

		template<class T>
		void converter_postcall(lua_State* L, by_const_reference<T>, int index) 
		{
		}
	};

	// ****** enum converter ********

	template<Direction Dir = Direction::cpp_to_lua>
	struct enum_converter
	{
		void apply(lua_State* L, int val)
		{
			lua_pushnumber(L, (lua_Number)val);
		}
	};

	template<>
	struct enum_converter<Direction::lua_to_cpp>
	{
		template<class T>
		T apply(lua_State* L, by_value<T>, int index)
		{
//			std::cerr << "enum_converter\n";
			return static_cast<T>(static_cast<int>(lua_tonumber(L, index)));
		}
		
		template<class T>
		static int match(lua_State* L, by_value<T>, int index)
		{
			if (lua_isnumber(L, index)) return 0; else return -1;
		}

		template<class T>
		void converter_postcall(lua_State*, T, int) {}
	};

	// ****** functor converter ********

	template<Direction Dir> struct functor_converter;

	template<>
	struct functor_converter<Direction::lua_to_cpp>
	{
		template<typename T>
		functor<T> apply(lua_State* L, by_const_reference<functor<T> >, int index)
		{
			if (lua_isnil(L, index))
				return functor<T>();

			lua_pushvalue(L, index);
			detail::lua_reference ref;
			ref.set(L);
			return functor<T>(L, ref);
		}

		template<typename T>
		functor<T> apply(lua_State* L, by_value<functor<T> >, int index)
		{
			if (lua_isnil(L, index))
				return functor<T>();

			lua_pushvalue(L, index);
			detail::lua_reference ref;
			ref.set(L);
			return functor<T>(L, ref);
		}

		template<typename T>
		static int match(lua_State* L, by_const_reference<functor<T> >, int index)
		{
			if (lua_isfunction(L, index) || lua_isnil(L, index)) return 0; else return -1;
		}

		template<typename T>
		static int match(lua_State* L, by_value<functor<T> >, int index)
		{
			if (lua_isfunction(L, index) || lua_isnil(L, index)) return 0; else return -1;
		}

		template<typename T>
		void converter_postcall(lua_State*, T, int) {}
	};

// *********** default_policy *****************

	struct default_policy : converter_policy_tag
	{
        static constexpr bool has_arg = true;

		template<typename T>
		static void precall(lua_State*, T, int) {}

        template<typename T, Direction Dir>
        struct generate_converter
        {
            using type = std::conditional_t<
                is_user_defined<T>::value,
                user_defined_converter<Dir>,
                std::conditional_t<
                    is_primitive<T>::value,
                    primitive_converter<Dir>,
                    std::conditional_t<
                        is_lua_functor<T>::value,
                        functor_converter<Dir>,
                        std::conditional_t<
                            std::is_enum_v<T>,
                            enum_converter<Dir>,
                            std::conditional_t<
                                is_nonconst_pointer<T>::value,
                                pointer_converter<Dir>,
                                std::conditional_t<
                                    is_const_pointer<T>::value,
                                    const_pointer_converter<Dir>,
                                    std::conditional_t<
                                        is_nonconst_reference<T>::value,
                                        ref_converter<Dir>,
                                        std::conditional_t<
                                            is_const_reference<T>::value,
                                            const_ref_converter<Dir>,
                                            value_converter<Dir>
                                        >
                                    >
                                >
                            >
                        >
                    >
                >
            >;
        };
	};

// ============== new policy system =================

    template <bool IsConversionPolicy, typename Policy, size_t Index>
    struct is_conversion_policy_with_index_impl : public std::false_type
    {
    };

    template <typename Policy, size_t Index>
    struct is_conversion_policy_with_index_impl<true, Policy ,Index> : public std::bool_constant<Policy::index == Index>
    {
    };

    template <typename Policy, size_t Index>
    struct is_conversion_policy_with_index : public is_conversion_policy_with_index_impl<std::is_base_of_v<conversion_policy_base, Policy>, Policy, Index>
    {
    };

    template<size_t, typename...>
    struct find_conversion_policy;

	template<size_t Index, typename Policy, typename... Policies>
	struct find_conversion_policy<Index, Policy, Policies...>
	{
        using type = std::conditional_t<
            is_conversion_policy_with_index<Policy, Index>::value,
            Policy,
            typename find_conversion_policy<Index, Policies...>::type
        >;
	};

    template<size_t Index>
    struct find_conversion_policy<Index>
    {
        using type = default_policy;
    };

    template<typename...>
    struct policy_list_postcall;

    template<typename Policy, typename... Policies>
    struct policy_list_postcall<Policy, Policies...>
    {
        static void apply(lua_State* L, const index_map& i)
        {
            Policy::postcall(L, i);
            policy_list_postcall<Policies...>::apply(L, i);
        }
    };

    template<>
    struct policy_list_postcall<>
    {
        static void apply(lua_State*, const index_map&) noexcept {}
    };
}
}

namespace luabind {	 namespace
{
	constexpr size_t return_value = 0;
    constexpr size_t result = return_value;
}}

#include <luabind/detail/object_funs.hpp>
