////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author or Addison-Wesley Longman make no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// Last update: Mar 06, 2003
//
// Like the original library, this port now uses void as 
// default value for return types.
//
// This new of visitor.h handles void returns transparently. See
// readme.txt for an explanation of the used technique.
// However there are still two sets of macros. One for return type = void
// (DEFINE_VISITABLE_VOID, DEFINE_CYCLIC_VISITABLE_VOID) and one for return
// type != void (DEFINE_VISITABLE, DEFINE_CYCLIC_VISITABLE)
//
// If you prefer the old version of visitor.h which uses a different set of
// visitor classes for the return type void, define the macro
// USE_VISITOR_OLD_VERSION.
//
#ifdef USE_VISITOR_OLD_VERSION
#include "VisitorOld.h"
#else
#ifndef VISITOR_INC_
#define VISITOR_INC_

#include "Typelist.h"
#include "HierarchyGenerators.h"
#include "MSVC6Helpers.h"

namespace Loki
{

////////////////////////////////////////////////////////////////////////////////
// class template BaseVisitor
// The base class of any Acyclic Visitor
////////////////////////////////////////////////////////////////////////////////

    class BaseVisitor
    {
    public:
        virtual ~BaseVisitor() {}
    };

////////////////////////////////////////////////////////////////////////////////
// class template Visitor
// The building block of Acyclic Visitor
////////////////////////////////////////////////////////////////////////////////
	template <class T, typename R = Loki::Private::VoidWrap::type >
    class Visitor;
////////////////////////////////////////////////////////////////////////////////
// class template Visitor (specialization)
// This specialization is not present in the book. It makes it easier to define
// Visitors for multiple types in a shot by using a typelist. Example:
//
// class SomeVisitor :
//     public BaseVisitor // required
//     public Visitor<TYPELIST_2(RasterBitmap, Paragraph)>,
//     public Visitor<Paragraph>
// {
// public:
//     void Visit(RasterBitmap&); // visit a RasterBitmap
//     void Visit(Paragraph &);   // visit a Paragraph
// };
////////////////////////////////////////////////////////////////////////////////
namespace Private
{
	// helper for Visitor's the left base class
	template <unsigned int ListId>
	struct VisitorImplLeft
	{
		template <class TList, class R>
		struct In
		{
			typedef typename TList::ERROR_WRONG_SPECIALIZATION_SELECTED Result;
		};
	};

	// helper for Visitor's the right base class
	template <unsigned int ListId>
	struct VisitorImplRight
	{
		template <class TList, class R>
		struct In
		{
			typedef typename TList::ERROR_WRONG_SPECIALIZATION_SELECTED Result;
		};
	};

	// simulates specialization
	// class Visitor<Head, R>
	template <>
	struct VisitorImplLeft<TL::Private::NoneList_ID>
	{
		template <class T, class R>
		struct In
		{
            struct Result
            {
                typedef R ReturnType;
                virtual ReturnType Visit(T&) = 0;
            };
		};
	};

	// simulates the left base class for the specialization
	// class Visitor<Typelist<Head, Tail>, R>
	template <>
	struct VisitorImplLeft<TL::Private::Typelist_ID>
	{
		template <class TList, class R>
		struct In
		{
            typedef Visitor<typename TList::Head, R> Result;
		};
	};

	template <>
	struct VisitorImplRight<TL::Private::NoneList_ID>
	{
		template <class TList, class R>
		struct In
		{
			struct Result {};
		};
	};

	// simulates the right base class for the specialization
	// class Visitor<Typelist<Head, Tail>, R>
	template <>
	struct VisitorImplRight<TL::Private::Typelist_ID>
	{
		template <class TList, class R>
		struct In
		{
            typedef Visitor<typename TList::Tail, R> Result;
		};
	};

	template <>
	struct VisitorImplRight<TL::Private::AtomList_ID>
	{
		template <class TList, class R>
		struct In
		{
            struct Result {};
		};
	};

	// MSVC 6.0 will complain if we try to let Visitor inherit
	// directly from VisitorImplLeft/VisitorImplRight
	template <class T, class R>
	struct VisitorImplLeftWrap
	{
		struct Dummy{};
		typedef typename VisitorImplLeft
		<
			TL::Private::IsTypelist<T>::type_id == TL::Private::AtomList_ID ?
			TL::Private::Typelist_ID :
			TL::Private::IsTypelist<T>::type_id
		>::template In<T, R>::Result TempType;

		typedef VC_Base_Workaround<TempType, Dummy> Workaround;
		typedef Workaround::LeftBase Result;
	};

	template <class T, class R>
	struct VisitorImplRightWrap
	{
		struct Dummy{};
		typedef typename VisitorImplRight
		<
			TL::Private::IsTypelist<T>::type_id
		>::template In<T, R>::Result TempType;

		typedef VC_Base_Workaround<TempType, Dummy> Workaround;
		typedef Workaround::LeftBase Result;
	};


}
	template <class T, typename R>
	class Visitor : public Private::VisitorImplLeftWrap<T, R>::Result,
					public Private::VisitorImplRightWrap<T, R>::Result

	{
		public:
			typedef R ReturnType;
	};

////////////////////////////////////////////////////////////////////////////////
// class template BaseVisitorImpl
// Implements non-strict visitation (you can implement only part of the Visit
//     functions)
////////////////////////////////////////////////////////////////////////////////

    template <class TList, typename R = Loki::Private::VoidWrap::type > 
	class BaseVisitorImpl;
namespace Private
{
	template <unsigned int ListTag>
	struct BaseVisitorImplHelper
	{
		template <typename T, typename R>
        struct In
        {
            typedef typename T::ERROR_WRONG_SPECIALIZATION_SELECTED Result;
        };
	};

	template<>
	struct BaseVisitorImplHelper<TL::Private::Typelist_ID>
    {
        template <typename TList, typename R>
        struct In
        {
			typedef BaseVisitorImpl<TList, R> Result;
        };
    };

    template<>
	struct BaseVisitorImplHelper<TL::Private::NullType_ID>
    {
        template <typename TList, typename R>
        struct In
        {
            struct Result {};
        };
    };

	template <class T, class R>
	struct BaseVisitorImplWrap
	{
		struct Dummy {};
		typedef typename BaseVisitorImplHelper
		<
			TL::Private::IsTypelist<typename T::Tail>::
			type_id == TL::Private::AtomList_ID ?
			TL::Private::Typelist_ID :
			TL::Private::IsTypelist<typename T::Tail>::type_id
		>::template In<typename T::Tail, R>::Result TempType;
		typedef VC_Base_Workaround<TempType, Dummy> Workaround;
		typedef Workaround::LeftBase Result;
	};

	template <class TList, class R>
	struct BaseVisitorImplBase : public Visitor<typename TList::Head, R>,
					public Private::BaseVisitorImplWrap<TList, R>::Result
	{
		ASSERT_TYPELIST(TList);
		virtual R Visit(typename TList::Head&)
        { return R(); }
	protected:
		~BaseVisitorImplBase() {}
	};

	template <class TList, class R>
	struct BaseVisitorImplVoidBase : public Visitor<typename TList::Head, R>,
					public Private::BaseVisitorImplWrap<TList, R>::Result
	{

		ASSERT_TYPELIST(TList);
		virtual R Visit(typename TList::Head&)
        {  }
	protected:
		~BaseVisitorImplVoidBase() {}
	};
}


	template <class TList, typename R>
	class BaseVisitorImpl : public Select
							<
								Private::IsVoid<R>::value,
								Private::BaseVisitorImplVoidBase<TList,R>,
								Private::BaseVisitorImplBase<TList, R>
							>::Result
    {
        ASSERT_TYPELIST(TList);

    public:
		// using BaseVisitorImpl<Tail, R>::Visit;

    };

////////////////////////////////////////////////////////////////////////////////
// class template DefaultCatchAll
////////////////////////////////////////////////////////////////////////////////
template <typename R, typename Visited>
struct DefaultCatchAll
{
	static R OnUnknownVisitor(Visited&, BaseVisitor&)
	{ return R(); }
};
template <typename R, typename Visited>
struct DefaultCatchAllVoid
{
	static R OnUnknownVisitor(Visited&, BaseVisitor&)
	{  }
};

// template template parameter workaround.
// use Wrapper-Classes like this to instantiate BaseVisitable
struct DefaultCatchAllWrapper
{
	template <class R, class Visited>
	struct In
	{
		typedef typename Select<Private::IsVoid<R>::value,
			DefaultCatchAllVoid<R, Visited>,
			DefaultCatchAll<R, Visited>
			>::Result type;
	};
};

////////////////////////////////////////////////////////////////////////////////
// class template NonStrictVisitor
// Implements non-strict visitation (you can implement only part of the Visit
//     functions)
////////////////////////////////////////////////////////////////////////////////
    template <class T, class Base>
	struct NonStrictVisitorUnit : public Base
    {
		typedef typename Base::ReturnType ReturnType;
        ReturnType Visit(T&)
        {
			return ReturnType();
		}
    };
	template <class T, class Base>
	struct NonStrictVisitorUnitVoid : public Base
    {
		typedef typename Base::ReturnType ReturnType;
        ReturnType Visit(T&)
        {}
    };

	struct NonStrictVisitorUnitWrapper
    {
        template <class T, class B>
		struct In
		{
			typedef typename B::ReturnType R;
			typedef typename Select<Private::IsVoid<R>::value,
			NonStrictVisitorUnitVoid<T, B>,
			NonStrictVisitorUnit<T, B>
			>::Result type;
		};
    };


    template <class TList, typename R = Loki::Private::VoidWrap::type>
    class NonStrictVisitor
        : public GenLinearHierarchy<
            TList,
            NonStrictVisitorUnitWrapper,
            Visitor<TList, R> >
    {
    };
////////////////////////////////////////////////////////////////////////////////
// class template BaseVisitable
////////////////////////////////////////////////////////////////////////////////
namespace Private
{
	template <class R, class CatchAll>
	class BaseVisitableBase
	{
		typedef R ReturnType;
	protected:
		template <class T>
		static ReturnType AcceptImpl(T& visited, BaseVisitor& guest)
		{
			typedef ApplyInnerType2<CatchAll, R, T>::type CatchA;
			// Apply the Acyclic Visitor
			if (Visitor<T, R>* p = dynamic_cast<Visitor<T, R>*>(&guest))
			{
				return p->Visit(visited);
			}
			return CatchA::OnUnknownVisitor(visited, guest);
		}
		~BaseVisitableBase() {}
			
	};
	template <class R, class CatchAll>
	class BaseVisitableVoidBase
	{
		typedef R ReturnType;
	protected:
		template <class T>
		static ReturnType AcceptImpl(T& visited, BaseVisitor& guest)
		{
			typedef ApplyInnerType2<CatchAll, R, T>::type CatchA;
			// Apply the Acyclic Visitor
			if (Visitor<T>* p = dynamic_cast<Visitor<T>*>(&guest))
			{
				p->Visit(visited);
				return;
			}
			CatchA::OnUnknownVisitor(visited, guest);
		}
		~BaseVisitableVoidBase() {}
	};
}
    template
    <
		typename R = Loki::Private::VoidWrap::type,
        class CatchAll = DefaultCatchAllWrapper
    >
    class BaseVisitable : public Select<Private::IsVoid<R>::value,
			Private::BaseVisitableVoidBase<R, CatchAll>,
			Private::BaseVisitableBase<R, CatchAll>
			>::Result

    {
    public:
        typedef R ReturnType;
        virtual ~BaseVisitable() {}
        virtual ReturnType Accept(BaseVisitor&) = 0;
    };

////////////////////////////////////////////////////////////////////////////////
// macro DEFINE_VISITABLE
// Put it in every class that you want to make visitable (in addition to
//     deriving it from BaseVisitable<R>
////////////////////////////////////////////////////////////////////////////////

#define DEFINE_VISITABLE() \
    virtual ReturnType Accept(Loki::BaseVisitor& guest) \
    { return AcceptImpl(*this, guest); }

#define DEFINE_VISITABLE_VOID() \
    virtual ReturnType Accept(Loki::BaseVisitor& guest) \
    { AcceptImpl(*this, guest); }

////////////////////////////////////////////////////////////////////////////////
// class template CyclicVisitor
// Put it in every class that you want to make visitable (in addition to
//     deriving it from BaseVisitable<R>
////////////////////////////////////////////////////////////////////////////////
namespace Private
{
	template <typename R, class TList>
	class CyclicVisitorBase : public Visitor<TList, R>
	{
	public:
		template <class Visited>
        ReturnType GenericVisit(Visited& host)
        {
            Visitor<Visited, ReturnType>& subObj = *this;
            return subObj.Visit(host);
        }
	protected:
		~CyclicVisitorBase() {}
	};
	template <class TList>
	class CyclicVisitorVoidBase : public Visitor<TList, void>
	{
	public:
		template <class Visited>
        ReturnType GenericVisit(Visited& host)
        {
            Visitor<Visited, ReturnType>& subObj = *this;
            subObj.Visit(host);
        }
	protected:
		~CyclicVisitorVoidBase() {}
	};
}

	template <typename R, class TList>
	class CyclicVisitor : public Select<Private::IsVoid<R>::value,
		Private::CyclicVisitorVoidBase<TList>,
		Private::CyclicVisitorBase<R, TList>
		>::Result
    {
    public:
        typedef R ReturnType;
        // using Visitor<TList, R>::Visit;


    };

////////////////////////////////////////////////////////////////////////////////
// macro DEFINE_CYCLIC_VISITABLE
// Put it in every class that you want to make visitable by a cyclic visitor
////////////////////////////////////////////////////////////////////////////////

#define DEFINE_CYCLIC_VISITABLE(SomeVisitor) \
    virtual SomeVisitor::ReturnType Accept(SomeVisitor& guest) \
    { return guest.GenericVisit(*this); }

#define DEFINE_CYCLIC_VISITABLE_VOID(SomeVisitor) \
    virtual void Accept(SomeVisitor& guest) \
    { guest.GenericVisit(*this); }

} // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// Change log:
// March 20: add default argument DefaultCatchAll to BaseVisitable
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// Oct  27, 2002: ported by Benjamin Kaufmann to MSVC 6.0
// Feb	23, 2003: Removed special visitor classes for return type void.
//		Added Loki:: qualification to Accept's Paramter (in the macro) B.K.
// Mar	06, 2003: Changed default values for return types to void.
//				  Added protected destructors to private implementation classes B.K.				
////////////////////////////////////////////////////////////////////////////////

#endif // VISITOR_INC_
#endif