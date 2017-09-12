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

// Last update: Feb 23, 2003
//
//
// The original visitor implementation depends heavily on the possibility
// to return an expression of type "cv void" from a functions with a return
// type of cv void (6.6.3).
// Unfortunately the MSVC 6.0 does not allow that. Because I could not think
// of any transparent workaround I decided to create a set of complete new
// classes for the void-case.
// Of course this is a very unattractive solution :-(
// If you know of a better solution, please let me know.
//
// The MSVC 6.0 does not allow void to be a default value for a template parameter.
// I therefore changed all defaults to int.

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
	template <class T, typename R = int/* =  void */ >
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

    template <class TList, typename R = int /* =  void */ > class BaseVisitorImpl;

	// class for the void-case
	template <class TList> class BaseVisitorImplVoid;

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
			typedef typename Loki::Select<
					IsVoid<R>::value,
					BaseVisitorImplVoid<TList>,
					BaseVisitorImpl<TList, R>
					>::Result Result;
			//typedef BaseVisitorImpl<TList, R> Result;
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


}

	template <class TList, typename R>
    class BaseVisitorImpl : public Visitor<typename TList::Head, R>,
							public Private::BaseVisitorImplWrap<TList, R>::Result
    {
        ASSERT_TYPELIST(TList);

    public:
        // using BaseVisitorImpl<Tail, R>::Visit;

        virtual R Visit(typename TList::Head&)
        { return R(); }
    };

	// class for the void-case
	template <class TList>
    class BaseVisitorImplVoid : public Visitor<typename TList::Head, void>,
								public Private::BaseVisitorImplWrap<TList, void>::Result
    {
        ASSERT_TYPELIST(TList);

    public:
        // using BaseVisitorImpl<Tail, R>::Visit;

        virtual void Visit(typename TList::Head&)
        {}
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

// template template parameter workaround.
// use Wrapper-Classes like this to instantiate BaseVisitable
struct DefaultCatchAllWrapper
{
	template <class R, class Visited>
	struct In
	{
		typedef DefaultCatchAll<R, Visited> type;
	};
};

template <typename R, typename Visited>
struct DefaultCatchAllVoid
{
    static R OnUnknownVisitor(Visited&, BaseVisitor&)
    {}
};

struct DefaultCatchAllVoidWrapper
{
	template <class R, class Visited>
	struct In
	{
		typedef DefaultCatchAllVoid<R, Visited> type;
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

	struct NonStrictVisitorUnitWrapper
    {
        template <class T, class B>
		struct In
		{
			typedef NonStrictVisitorUnit<T, B> type;
		};
    };


    template <class TList, typename R = int /* =  void */>
    class NonStrictVisitor
        : public GenLinearHierarchy<
            TList,
            NonStrictVisitorUnitWrapper,
            Visitor<TList, R> >
    {
    };

	template <class T, class Base>
    struct NonStrictVisitorUnitVoid : public Base
    {
        typedef void ReturnType;
        ReturnType Visit(T&)
        {
        }
    };

	struct NonStrictVisitorUnitVoidWrapper
    {
        template <class T, class B>
		struct In
		{
			typedef NonStrictVisitorUnitVoid<T, B> type;
		};
    };


    template <class TList>
    class NonStrictVisitorVoid
        : public GenLinearHierarchy<
            TList,
            NonStrictVisitorUnitVoidWrapper,
            Visitor<TList, void> >
    {
    };
////////////////////////////////////////////////////////////////////////////////
// class template BaseVisitable
////////////////////////////////////////////////////////////////////////////////

    template
    <
		typename R = int/* =  void */,
        class CatchAll = DefaultCatchAllWrapper
    >
    class BaseVisitable
    {
    public:
        typedef R ReturnType;
        virtual ~BaseVisitable() {}
        virtual ReturnType Accept(BaseVisitor&) = 0;

    protected: // give access only to the hierarchy
        template <class T>
        static ReturnType AcceptImpl(T& visited, BaseVisitor& guest)
        {
            typedef ApplyInnerType2<CatchAll, R, T>::type CatchA;
			// Apply the Acyclic Visitor
            if (Visitor<T>* p = dynamic_cast<Visitor<T>*>(&guest))
            {
                return p->Visit(visited);
            }
            return CatchA::OnUnknownVisitor(visited, guest);
        }
    };


	// class for the void-case
	template
    <
        class CatchAll = DefaultCatchAllVoidWrapper
    >
    class BaseVisitableVoid
    {
    public:
        typedef void ReturnType;
        virtual ~BaseVisitableVoid() {}
        virtual ReturnType Accept(BaseVisitor&) = 0;

    protected: // give access only to the hierarchy
        template <class T>
        static ReturnType AcceptImpl(T& visited, BaseVisitor& guest)
        {
            typedef ApplyInnerType2<CatchAll, void, T>::type CatchA;
			// Apply the Acyclic Visitor
            if (Visitor<T, void>* p = dynamic_cast<Visitor<T, void>*>(&guest))
            {
                p->Visit(visited);
            }
            CatchA::OnUnknownVisitor(visited, guest);
        }
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
    virtual void Accept(Loki::BaseVisitor& guest) \
    { AcceptImpl(*this, guest); }

////////////////////////////////////////////////////////////////////////////////
// class template CyclicVisitor
// Put it in every class that you want to make visitable (in addition to
//     deriving it from BaseVisitable<R>
////////////////////////////////////////////////////////////////////////////////
    template <typename R, class TList>
    class CyclicVisitor : public Visitor<TList, R>
    {
    public:
        typedef R ReturnType;
        // using Visitor<TList, R>::Visit;

        template <class Visited>
        ReturnType GenericVisit(Visited& host)
        {
            Visitor<Visited, ReturnType>& subObj = *this;
            return subObj.Visit(host);
        }
    };

	template <class TList>
    class CyclicVisitorVoid : public Visitor<TList, void>
    {
    public:
        typedef void ReturnType;
        // using Visitor<TList, R>::Visit;

        template <class Visited>
        ReturnType GenericVisit(Visited& host)
        {
            Visitor<Visited, ReturnType>& subObj = *this;
            subObj.Visit(host);
        }
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
// Feb	23, 2003: Removed superfluous implementation classes and added Loki::
//		qualification to Accept's Paramter (in the macro) B.K.
////////////////////////////////////////////////////////////////////////////////

#endif // VISITOR_INC_

