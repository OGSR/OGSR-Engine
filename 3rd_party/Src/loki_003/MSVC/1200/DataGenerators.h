////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Data Generator by Shannon Barber
// This code DOES NOT accompany the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design 
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
//
// Code covered by the MIT License
// The author makes no representations about the suitability of this software
//  for any purpose. It is provided "as is" without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// Last update: Mar 04, 2003
// MSVC 6.0 version

#ifndef DATAGENERATORS_H
#define DATAGENERATORS_H
#include "Typelist.h"
#include "MSVC6Helpers.h"
namespace Loki
{
	namespace TL
	{
		template<typename T>
		struct nameof_type
		{
			const char* operator()()
			{
				return typeid(T).name();
			}
		};
		template<typename T>
		struct sizeof_type
		{
			size_t operator()()
			{
				return sizeof(T);
			}
		};

		// wrappers to workaround the need for template template parameters
		struct nameof_type_wrapper
		{
			template <class T>
			struct In
			{
				typedef nameof_type<T> type;
			};
		};
		
		struct sizeof_type_wrapper
		{
			template <class T>
			struct In
			{
				typedef sizeof_type<T> type;
			};
			
		};
		template <class TList, class GenFunc>
		struct IterateTypes;
namespace Private
{
	// Specialization for a general typelist
	template <unsigned Tag>
	struct IterateTypesImpl
	{
		template <class TList, class GenFunc>
		struct In
		{
			typedef typename TList::Head T1;
			typedef typename TList::Tail T2;

			typedef IterateTypes<T1, GenFunc> head_t;
			head_t head;
			typedef IterateTypes<T2, GenFunc> tail_t;
			tail_t tail;

			template<class II>
			void operator()(II ii)
			{
				head.operator()(ii);
				tail.operator()(ii);
			}
		protected:
			~In() {}
		};
	};
	
	// Specialization for a single type
	template <>
	struct IterateTypesImpl<TL::Private::NoneList_ID>
	{
		template <class AtomicType, class GenFunc>
		struct In
		{
			template<class II>
			void operator()(II ii)
			{
				typedef typename Loki::ApplyInnerType<GenFunc, AtomicType>::type gFunc;
				gFunc genfunc;
				*ii = genfunc();
				++ii; //Is this even needed?
			}
		protected:
			~In() {}
		};
	};

	// Specialization for NullType
	template <>
	struct IterateTypesImpl<TL::Private::NullType_ID>
	{
		template <class T, class GenFunc>
		struct In
		{
			template<class II>
			void operator()(II ii)
			{}
		protected:
			~In() {}
		};
	};
}	// end ns Private
		
		template <class TList, class GenFunc>
		struct IterateTypes : public 
			Private::IterateTypesImpl
			<	
				TL::Private::IsTypelist<TList>::type_id
			>::template In<TList, GenFunc>
		{};
		
		template<typename Types, class UnitFunc, typename II>
		void iterate_types(II ii)
		{
			Loki::TL::IterateTypes<Types, UnitFunc> it;
			it(ii);
		}
	}//ns TL
}//ns Loki

#endif //DATAGENERATORS_H
////////////////////////////////////////////////////////////////////////////////
// Change log:
// 9/20/02 Named changed from GenData to IterateTypes
// 10/8/02 insertion iterators are passed-by-value, not by-reference (oops)
// 03/04/03 ported by Benjamin Kaufmann to MSVC 6.0 
// 03/06/03 added protected destructors to private implementation classes B.K.
////////////////////////////////////////////////////////////////////////////////
