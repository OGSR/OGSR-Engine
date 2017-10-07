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

namespace luabind { namespace detail
{
	template<int N>
	struct char_array
	{
		char storage[N];
	};

	template<class U>
	char_array<sizeof(U)> indirect_sizeof_test(by_reference<U>);

	template<class U>
	char_array<sizeof(U)> indirect_sizeof_test(by_const_reference<U>);

	template<class U>
	char_array<sizeof(U)> indirect_sizeof_test(by_pointer<U>);

	template<class U>
	char_array<sizeof(U)> indirect_sizeof_test(by_const_pointer<U>);

	template<class U>
	char_array<sizeof(U)> indirect_sizeof_test(by_value<U>);

	template<class T>
	struct indirect_sizeof
	{
        static constexpr int value = sizeof(indirect_sizeof_test(LUABIND_DECORATE_TYPE(T)));
	};

	template<size_t Size>
	struct out_value_converter
	{
		template<typename T>
		T& apply(lua_State* L, by_reference<T>, int index)
		{
		    using converter_policy = find_conversion_policy<1>::type;
			typename converter_policy::generate_converter<T, Direction::lua_to_cpp>::type converter;
			new (m_storage) T(converter.apply(L, LUABIND_DECORATE_TYPE(T), index));
			return *reinterpret_cast<T*>(m_storage);
		}

		template<typename T>
		static int match(lua_State* L, by_reference<T>, int index)
		{
		    using converter_policy = find_conversion_policy<1>::type;
			typedef typename converter_policy::generate_converter<T, Direction::lua_to_cpp>::type converter;
			return converter::match(L, LUABIND_DECORATE_TYPE(T), index);
		}

		template<typename T>
		void converter_postcall(lua_State* L, by_reference<T>, int) 
		{
		    using converter_policy = find_conversion_policy<2>::type;
			typename converter_policy::template generate_converter<T, Direction::cpp_to_lua>::type converter;
			converter.apply(L, *reinterpret_cast<T*>(m_storage));
			reinterpret_cast<T*>(m_storage)->~T();
		}

		template<typename T>
		T* apply(lua_State* L, by_pointer<T>, int index)
		{
		    using converter_policy = find_conversion_policy<1>::type;
			typename converter_policy::generate_converter<T, Direction::lua_to_cpp>::type converter;
			new (m_storage) T(converter.apply(L, LUABIND_DECORATE_TYPE(T), index));
			return reinterpret_cast<T*>(m_storage);
		}

		template<typename T>
		static int match(lua_State* L, by_pointer<T>, int index)
		{
		    using converter_policy = find_conversion_policy<1>::type;
			typedef typename converter_policy::generate_converter<T, Direction::lua_to_cpp>::type converter;
			return converter::match(L, LUABIND_DECORATE_TYPE(T), index);
		}

		template<typename T>
		void converter_postcall(lua_State* L, by_pointer<T>, int)
		{
		    using converter_policy = find_conversion_policy<2>::type;
			typename converter_policy::generate_converter<T, Direction::cpp_to_lua>::type converter;	
			converter.apply(L, *reinterpret_cast<T*>(m_storage));
			reinterpret_cast<T*>(m_storage)->~T();
		}

		char m_storage[Size];
	};

	template<size_t N>
	struct out_value_policy : conversion_policy<N>
	{
		static void precall(lua_State*, const index_map&) {}
		static void postcall(lua_State*, const index_map&) {}

		struct only_accepts_nonconst_references_or_pointers {};
		struct can_only_convert_from_lua_to_cpp {};

		template<typename T, Direction Dir>
		struct generate_converter
		{
            using type = std::conditional_t<
                Dir == Direction::lua_to_cpp,
                std::conditional_t<
                    is_nonconst_reference<T>::value || is_nonconst_pointer<T>::value,
                    out_value_converter<indirect_sizeof<T>::value>,
                    only_accepts_nonconst_references_or_pointers
                >,
                can_only_convert_from_lua_to_cpp
            >;
		};
	};

	template<size_t Size>
	struct pure_out_value_converter
	{
		template<typename T>
		T& apply(lua_State* L, by_reference<T>, int index)
		{
			new (m_storage) T();
			return *reinterpret_cast<T*>(m_storage);
		}

		template<typename T>
		static int match(lua_State* L, by_reference<T>, int index)
		{
			return 0;
		}

		template<typename T>
		void converter_postcall(lua_State* L, by_reference<T>, int) 
		{
		    using converter_policy = find_conversion_policy<1>::type;
			typename converter_policy::generate_converter<T, Direction::cpp_to_lua>::type converter;	
			converter.apply(L, *reinterpret_cast<T*>(m_storage));
			reinterpret_cast<T*>(m_storage)->~T();
		}

		template<typename T>
		T* apply(lua_State* L, by_pointer<T>, int index)
		{
			new (m_storage) T();
			return reinterpret_cast<T*>(m_storage);
		}

		template<typename T>
		static int match(lua_State* L, by_pointer<T>, int index)
		{
			return 0;
		}

		template<typename T>
		void converter_postcall(lua_State* L, by_pointer<T>, int) 
		{
            using converter_policy = find_conversion_policy<1>::type;
			typename converter_policy::generate_converter<T, Direction::cpp_to_lua>::type converter;
			converter.apply(L, *reinterpret_cast<T*>(m_storage));
			reinterpret_cast<T*>(m_storage)->~T();
		}


		char m_storage[Size];
	};

	template<size_t N>
	struct pure_out_value_policy : conversion_policy<N, false>
	{
		static void precall(lua_State*, const index_map&) {}
		static void postcall(lua_State*, const index_map&) {}

		struct only_accepts_nonconst_references_or_pointers {};
		struct can_only_convert_from_lua_to_cpp {};

		template<typename T, Direction Dir>
		struct generate_converter
		{
            using type = std::conditional_t<
                Dir == Direction::lua_to_cpp,
                std::conditional_t<
                    is_nonconst_reference<T>::value || is_nonconst_pointer<T>::value,
                    pure_out_value_converter<indirect_sizeof<T>::value>,
                    only_accepts_nonconst_references_or_pointers
                >,
                can_only_convert_from_lua_to_cpp
            >;
		};
	};
	
}}

namespace luabind
{
	template<size_t N>
	detail::policy_cons<detail::out_value_policy<N>> 
	out_value() { return detail::policy_cons<detail::out_value_policy<N>>(); }

	template<size_t N>
	detail::policy_cons<detail::pure_out_value_policy<N>> 
	pure_out_value() { return detail::policy_cons<detail::pure_out_value_policy<N>>(); }
}
