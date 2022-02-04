////////////////////////////////////////////////////////////////////////////
//	Module 		: object_cloner.h
//	Created 	: 13.07.2004
//  Modified 	: 13.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Object cloner
////////////////////////////////////////////////////////////////////////////

#pragma once

struct CCloner {
	template <typename T>
	struct CHelper {
		template <bool a>
		IC	static void clone(const T &_1, T &_2)
		{
			_2				= _1;
		}

		template <>
		IC	static void clone<true>(const T &_1, T &_2)
		{
			_2				= xr_new<object_type_traits::remove_pointer<T>::type>(*_1);
			CCloner::clone	(*_1,*_2);
		}
	};

	IC	static void clone(LPCSTR __1, LPCSTR &__2)
	{
		__2							= __1;
	}

	IC	static void clone(LPSTR  __1, LPSTR &__2)
	{
		__2							= xr_strdup(__1);
	}

	IC	static void clone(const shared_str &__1, shared_str &__2)
	{
		__2							= __1;
	}

	template <typename T1, typename T2>
	IC	static void clone(const std::pair<T1,T2> &_1, std::pair<T1,T2> &_2)
	{
		clone(const_cast<typename object_type_traits::remove_const<T1>::type&>(_1.first),const_cast<typename object_type_traits::remove_const<T1>::type&>(_2.first));
		clone(_1.second,_2.second);
	}

	template <typename T, int size>
	IC	static void clone(const svector<T,size> &_1, svector<T,size> &_2)
	{
		_2.resize						(_1.size());
		auto J = _2.begin();
		auto I = _1.begin();
		auto E = _1.end();
		for ( ; I != E; ++I, ++J)
			clone						(*I,*J);
	}

	template <typename T1, typename T2>
	IC	static void clone(const std::queue<T1,T2> &__1, std::queue<T1,T2> &__2)
	{
		std::queue<T1,T2>			_1 = __1;
		std::queue<T1,T2>			_2;
		
		for ( ; !_1.empty(); _1.pop())
			_2.push					(_1.front());

		while (!__2.empty())
			__2.pop();

		for ( ; !_2.empty(); _2.pop()) {
			typename std::queue<T1,T2>::value_type	t;
			CCloner::clone			(_2.front(),t);
			__2.push				(t);
		}
	}

	template <template <typename _1, typename _2> class T1, typename T2, typename T3>
	IC	static void clone(const T1<T2,T3> &__1, T1<T2,T3> &__2, bool)
	{
		T1<T2,T3>					_1 = __1;
		T1<T2,T3>					_2;

		for ( ; !_1.empty(); _1.pop())
			_2.push					(_1.top());

		while (!__2.empty())
			__2.pop();

		for ( ; !_2.empty(); _2.pop()) {
			typename T1<T2,T3>::value_type	t;
			CCloner::clone			(_2.top(),t);
			__2.push				(t);
		}
	}

	template <template <typename _1, typename _2, typename _3> class T1, typename T2, typename T3, typename T4>
	IC	static void clone(const T1<T2,T3,T4> &__1, T1<T2,T3,T4> &__2, bool)
	{
		T1<T2,T3,T4>				_1 = __1;
		T1<T2,T3,T4>				_2;

		for ( ; !_1.empty(); _1.pop())
			_2.push					(_1.top());

		while (!__2.empty())
			__2.pop();

		for ( ; !_2.empty(); _2.pop()) {
			typename T1<T2,T3,T4>::value_type t;
			CCloner::clone			(_2.top(),t);
			__2.push				(t);
		}
	}

	template <typename T1, typename T2>
	IC	static void clone(const xr_stack<T1,T2> &_1, xr_stack<T1,T2> &_2)
	{
		return					(clone(_1,_2,true));
	}

	template <typename T1, typename T2, typename T3>
	IC	static void clone(const std::priority_queue<T1,T2,T3> &_1, std::priority_queue<T1,T2,T3> &_2)
	{
		return					(clone(_1,_2,true));
	}

	struct CHelper3 {
		template <template <typename _1> class T1, typename T2>
		IC	static void add(T1<T2> &data, typename T1<T2>::value_type &value)
		{
			data.push_back		(value);
		}

		template <typename T1, typename T2>
		IC	static void add(T1 &data, typename T2 &value)
		{
			data.insert			(value);
		}

		template <typename T>
		IC	static void clone(const T &_1, T &_2)
		{
			_2.clear			();
			auto I = _1.begin();
			auto E = _1.end();
			for ( ; I != E; ++I) {
				typename T::value_type t;
				CCloner::clone	(*I,t);
				add				(_2,t);
			}
		}
	};

	template <typename T>
	struct CHelper4 {
		template <bool a>
		IC	static void clone(const T &_1, T &_2)
		{
			CHelper<T>::clone<object_type_traits::is_pointer<T>::value>(_1,_2);
		}

		template <>
		IC	static void clone<true>(const T &_1, T &_2)
		{
			CHelper3::clone(_1,_2);
		}
	};

	template <typename T>
	IC	static void clone(const T &_1, T &_2)
	{
		CHelper4<T>::clone<object_type_traits::is_stl_container<T>::value>(_1,_2);
	}
};

IC	void clone(LPCSTR p0, LPSTR &p1)
{
	p1				= xr_strdup(p0);
}

template <typename T>
IC	void clone(const T &p0, T &p1)
{
	CCloner::clone	(p0,p1);
}
