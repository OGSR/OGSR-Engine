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
// The author or Addison-Welsey Longman make no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// Last update: Feb 25, 2003
//
// Functor's Template-Ctor now has a Loki::Disambiguate parameter.
// Use it, when the VC complains about an ambiguity. For example if you
// want to create a functor with a functor of different but compatible type.
// Functor<R, Arguments> Fun(AnotherFun, Disambiguate());
//
// Changed BindFirst from
// typename Private::BinderFirstTraits<Fctor>::BoundFunctorType
// BindFirst(const Fctor& fun, typename Fctor::Parm1 bound);
// to
// template <class R, class TList, class Thread>
// Functor<R, Private::BinderFirstTraitsHelper<TList>::ParmList, Thread>
// BindFirst(const Functor<R, TList, Thread>& fun, Functor<R, TList, Thread>::Parm1 bound)
// The old return type was to complicated for the VC and responsible for
// some C1001-Internal Compiler errors.
//
// All functors will return an object of the udt VoidAsType if the template
// parameter R is void.
// *Do not* use this returned object. It is only used as a workaround.

#ifndef FUNCTOR_INC_
#define FUNCTOR_INC_

#include "Typelist.h"
#include "EmptyType.h"
#include "SmallObj.h"
#include "TypeTraits.h"
#include <typeinfo>
#include <memory>
#include "MSVC6Helpers.h"
namespace Loki
{
////////////////////////////////////////////////////////////////////////////////
// class template FunctorImpl (internal)
////////////////////////////////////////////////////////////////////////////////

    namespace Private
    {
    template <typename R, class ThreadingModel>
    struct FunctorImplBase : public SmallObject<ThreadingModel>
    {
        typedef R ResultType;
        typedef typename Select
		<
			IsVoid<R>::value != 0,
			VoidAsType,
			R
		>::Result NewRType;
        typedef EmptyType Parm1;
        typedef EmptyType Parm2;
        typedef EmptyType Parm3;
        typedef EmptyType Parm4;
        typedef EmptyType Parm5;
        typedef EmptyType Parm6;
        typedef EmptyType Parm7;
        typedef EmptyType Parm8;
        typedef EmptyType Parm9;
        typedef EmptyType Parm10;
        typedef EmptyType Parm11;
        typedef EmptyType Parm12;
        typedef EmptyType Parm13;
        typedef EmptyType Parm14;
        typedef EmptyType Parm15;

        virtual FunctorImplBase<R, ThreadingModel>* DoClone() const = 0;
        template <class U>
        static U* Clone(U* pObj)
        {
            if (!pObj) return 0;
			U* pClone = static_cast<U*>(pObj->DoClone());
			assert(typeid(*pClone) == typeid(*pObj));
            return pClone;
        }
		virtual ~FunctorImplBase () {}
    };

////////////////////////////////////////////////////////////////////////////////
// macro DEFINE_CLONE_FUNCTORIMPL
// Implements the DoClone function for a functor implementation
////////////////////////////////////////////////////////////////////////////////

#define DEFINE_CLONE_FUNCTORIMPL(Cls) \
    virtual Private::FunctorImplBase<ResultType, ThreadModel>* \
	DoClone() const { return new Cls(*this); }


////////////////////////////////////////////////////////////////////////////////
// class template FunctorImplHelper
// Base template
////////////////////////////////////////////////////////////////////////////////
    template <unsigned int TListLength>
    struct FunctorImplHelper;

////////////////////////////////////////////////////////////////////////////////
// class template FunctorImplHelper
// Specialization for 0 (zero) parameters
////////////////////////////////////////////////////////////////////////////////

    template <>
    struct FunctorImplHelper<0>
    {
        template <typename R, class TList, class ThreadingModel>
        class In : public Private::FunctorImplBase<R, ThreadingModel>
        {
        public:
            typedef R ResultType;
			typedef typename Select
			<
				IsVoid<R>::value != 0,
				VoidAsType,
				R
			>::Result NewRType;
            virtual NewRType operator()() = 0;
        };
    };


////////////////////////////////////////////////////////////////////////////////
// class template FunctorImplHelper
// Specialization for 1 parameter
////////////////////////////////////////////////////////////////////////////////

    template <>
    struct FunctorImplHelper<1>
    {
        template <typename R, class TList, class ThreadingModel>
        class In : public Private::FunctorImplBase<R, ThreadingModel>
        {
            typedef typename TL::TypeAt<TList, 0>::Result P1;

        public:
            typedef R ResultType;
			typedef typename Select
			<
				IsVoid<R>::value != 0,
				VoidAsType,
				R
			>::Result NewRType;
            typedef typename TypeTraits<P1>::ParameterType Parm1;
            virtual NewRType operator()(Parm1) = 0;
        };
    };


////////////////////////////////////////////////////////////////////////////////
// class template FunctorImplHelper
// Specialization for 2 parameters
////////////////////////////////////////////////////////////////////////////////
   template <>
   struct FunctorImplHelper<2>
   {
       template <typename R, class TList, class ThreadingModel>
       class In : public Private::FunctorImplBase<R, ThreadingModel>
       {
           typedef typename TL::TypeAt<TList, 0>::Result P1;
           typedef typename TL::TypeAt<TList, 1>::Result P2;

       public:
           typedef R ResultType;
		   typedef typename Select
			<
				IsVoid<R>::value != 0,
				VoidAsType,
				R
			>::Result NewRType;
           typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           virtual NewRType operator()(Parm1, Parm2) = 0;
       };
   };
////////////////////////////////////////////////////////////////////////////////
// class template FunctorImplHelper
// Specialization for 3 parameters
////////////////////////////////////////////////////////////////////////////////
   template <>
   struct FunctorImplHelper<3>
   {
       template <typename R, class TList, class ThreadingModel>
       class In : public Private::FunctorImplBase<R, ThreadingModel>
       {
           typedef typename TL::TypeAt<TList, 0>::Result P1;
           typedef typename TL::TypeAt<TList, 1>::Result P2;
           typedef typename TL::TypeAt<TList, 2>::Result P3;

       public:
           typedef R ResultType;
           typedef typename Select
			<
				IsVoid<R>::value != 0,
				VoidAsType,
				R
			>::Result NewRType;
		   typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           virtual NewRType operator()(Parm1, Parm2, Parm3) = 0;
       };
   };
////////////////////////////////////////////////////////////////////////////////
// class template FunctorImplHelper
// Specialization for 4 parameters
////////////////////////////////////////////////////////////////////////////////
   template <>
   struct FunctorImplHelper<4>
   {
       template <typename R, class TList, class ThreadingModel>
       class In : public Private::FunctorImplBase<R, ThreadingModel>
       {
           typedef typename TL::TypeAt<TList, 0>::Result P1;
           typedef typename TL::TypeAt<TList, 1>::Result P2;
           typedef typename TL::TypeAt<TList, 2>::Result P3;
           typedef typename TL::TypeAt<TList, 3>::Result P4;

       public:
           typedef R ResultType;
           typedef typename Select
			<
				IsVoid<R>::value != 0,
				VoidAsType,
				R
			>::Result NewRType;
		   typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           typedef typename TypeTraits<P4>::ParameterType Parm4;
           virtual NewRType operator()(Parm1, Parm2, Parm3, Parm4) = 0;
       };
   };
////////////////////////////////////////////////////////////////////////////////
// class template FunctorImplHelper
// Specialization for 5 parameters
////////////////////////////////////////////////////////////////////////////////
   template <>
   struct FunctorImplHelper<5>
   {
       template <typename R, class TList, class ThreadingModel>
       class In : public Private::FunctorImplBase<R, ThreadingModel>
       {
           typedef typename TL::TypeAt<TList, 0>::Result P1;
           typedef typename TL::TypeAt<TList, 1>::Result P2;
           typedef typename TL::TypeAt<TList, 2>::Result P3;
           typedef typename TL::TypeAt<TList, 3>::Result P4;
           typedef typename TL::TypeAt<TList, 4>::Result P5;

       public:
           typedef R ResultType;
		   typedef typename Select
			<
				IsVoid<R>::value != 0,
				VoidAsType,
				R
			>::Result NewRType;
           typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           typedef typename TypeTraits<P4>::ParameterType Parm4;
           typedef typename TypeTraits<P5>::ParameterType Parm5;
           virtual NewRType operator()(Parm1, Parm2, Parm3, Parm4, Parm5) = 0;
       };
   };
////////////////////////////////////////////////////////////////////////////////
// class template FunctorImplHelper
// Specialization for 6 parameters
////////////////////////////////////////////////////////////////////////////////
   template <>
   struct FunctorImplHelper<6>
   {
       template <typename R, class TList, class ThreadingModel>
       class In : public Private::FunctorImplBase<R, ThreadingModel>
       {
           typedef typename TL::TypeAt<TList, 0>::Result P1;
           typedef typename TL::TypeAt<TList, 1>::Result P2;
           typedef typename TL::TypeAt<TList, 2>::Result P3;
           typedef typename TL::TypeAt<TList, 3>::Result P4;
           typedef typename TL::TypeAt<TList, 4>::Result P5;
           typedef typename TL::TypeAt<TList, 5>::Result P6;

       public:
           typedef R ResultType;
           typedef typename Select
			<
				IsVoid<R>::value != 0,
				VoidAsType,
				R
			>::Result NewRType;
		   typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           typedef typename TypeTraits<P4>::ParameterType Parm4;
           typedef typename TypeTraits<P5>::ParameterType Parm5;
           typedef typename TypeTraits<P6>::ParameterType Parm6;
           virtual NewRType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6) = 0;
       };
   };
////////////////////////////////////////////////////////////////////////////////
// class template FunctorImplHelper
// Specialization for 7 parameters
////////////////////////////////////////////////////////////////////////////////
   template <>
   struct FunctorImplHelper<7>
   {
       template <typename R, class TList, class ThreadingModel>
       class In : public Private::FunctorImplBase<R, ThreadingModel>
       {
           typedef typename TL::TypeAt<TList, 0>::Result P1;
           typedef typename TL::TypeAt<TList, 1>::Result P2;
           typedef typename TL::TypeAt<TList, 2>::Result P3;
           typedef typename TL::TypeAt<TList, 3>::Result P4;
           typedef typename TL::TypeAt<TList, 4>::Result P5;
           typedef typename TL::TypeAt<TList, 5>::Result P6;
           typedef typename TL::TypeAt<TList, 6>::Result P7;

       public:
           typedef R ResultType;
		   typedef typename Select
			<
				IsVoid<R>::value != 0,
				VoidAsType,
				R
			>::Result NewRType;
           typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           typedef typename TypeTraits<P4>::ParameterType Parm4;
           typedef typename TypeTraits<P5>::ParameterType Parm5;
           typedef typename TypeTraits<P6>::ParameterType Parm6;
           typedef typename TypeTraits<P7>::ParameterType Parm7;
           virtual NewRType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7) = 0;
       };
   };
////////////////////////////////////////////////////////////////////////////////
// class template FunctorImplHelper
// Specialization for 8 parameters
////////////////////////////////////////////////////////////////////////////////
   template <>
   struct FunctorImplHelper<8>
   {
       template <typename R, class TList, class ThreadingModel>
       class In : public Private::FunctorImplBase<R, ThreadingModel>
       {
           typedef typename TL::TypeAt<TList, 0>::Result P1;
           typedef typename TL::TypeAt<TList, 1>::Result P2;
           typedef typename TL::TypeAt<TList, 2>::Result P3;
           typedef typename TL::TypeAt<TList, 3>::Result P4;
           typedef typename TL::TypeAt<TList, 4>::Result P5;
           typedef typename TL::TypeAt<TList, 5>::Result P6;
           typedef typename TL::TypeAt<TList, 6>::Result P7;
           typedef typename TL::TypeAt<TList, 7>::Result P8;

       public:
           typedef R ResultType;
		   typedef typename Select
			<
				IsVoid<R>::value != 0,
				VoidAsType,
				R
			>::Result NewRType;
           typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           typedef typename TypeTraits<P4>::ParameterType Parm4;
           typedef typename TypeTraits<P5>::ParameterType Parm5;
           typedef typename TypeTraits<P6>::ParameterType Parm6;
           typedef typename TypeTraits<P7>::ParameterType Parm7;
           typedef typename TypeTraits<P8>::ParameterType Parm8;
           virtual NewRType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8) = 0;
       };
   };
////////////////////////////////////////////////////////////////////////////////
// class template FunctorImplHelper
// Specialization for 9 parameters
////////////////////////////////////////////////////////////////////////////////
   template <>
   struct FunctorImplHelper<9>
   {
       template <typename R, class TList, class ThreadingModel>
       class In : public Private::FunctorImplBase<R, ThreadingModel>
       {
           typedef typename TL::TypeAt<TList, 0>::Result P1;
           typedef typename TL::TypeAt<TList, 1>::Result P2;
           typedef typename TL::TypeAt<TList, 2>::Result P3;
           typedef typename TL::TypeAt<TList, 3>::Result P4;
           typedef typename TL::TypeAt<TList, 4>::Result P5;
           typedef typename TL::TypeAt<TList, 5>::Result P6;
           typedef typename TL::TypeAt<TList, 6>::Result P7;
           typedef typename TL::TypeAt<TList, 7>::Result P8;
           typedef typename TL::TypeAt<TList, 8>::Result P9;

       public:
           typedef R ResultType;
		   typedef typename Select
			<
				IsVoid<R>::value != 0,
				VoidAsType,
				R
			>::Result NewRType;
           typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           typedef typename TypeTraits<P4>::ParameterType Parm4;
           typedef typename TypeTraits<P5>::ParameterType Parm5;
           typedef typename TypeTraits<P6>::ParameterType Parm6;
           typedef typename TypeTraits<P7>::ParameterType Parm7;
           typedef typename TypeTraits<P8>::ParameterType Parm8;
           typedef typename TypeTraits<P9>::ParameterType Parm9;
           virtual NewRType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9) = 0;
       };
   };
////////////////////////////////////////////////////////////////////////////////
// class template FunctorImplHelper
// Specialization for 10 parameters
////////////////////////////////////////////////////////////////////////////////
   template <>
   struct FunctorImplHelper<10>
   {
       template <typename R, class TList, class ThreadingModel>
       class In : public Private::FunctorImplBase<R, ThreadingModel>
       {
           typedef typename TL::TypeAt<TList, 0>::Result P1;
           typedef typename TL::TypeAt<TList, 1>::Result P2;
           typedef typename TL::TypeAt<TList, 2>::Result P3;
           typedef typename TL::TypeAt<TList, 3>::Result P4;
           typedef typename TL::TypeAt<TList, 4>::Result P5;
           typedef typename TL::TypeAt<TList, 5>::Result P6;
           typedef typename TL::TypeAt<TList, 6>::Result P7;
           typedef typename TL::TypeAt<TList, 7>::Result P8;
           typedef typename TL::TypeAt<TList, 8>::Result P9;
           typedef typename TL::TypeAt<TList, 9>::Result P10;

       public:
           typedef R ResultType;
		   typedef typename Select
			<
				IsVoid<R>::value != 0,
				VoidAsType,
				R
			>::Result NewRType;
           typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           typedef typename TypeTraits<P4>::ParameterType Parm4;
           typedef typename TypeTraits<P5>::ParameterType Parm5;
           typedef typename TypeTraits<P6>::ParameterType Parm6;
           typedef typename TypeTraits<P7>::ParameterType Parm7;
           typedef typename TypeTraits<P8>::ParameterType Parm8;
           typedef typename TypeTraits<P9>::ParameterType Parm9;
           typedef typename TypeTraits<P10>::ParameterType Parm10;
           virtual NewRType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9, Parm10) = 0;
       };
   };
////////////////////////////////////////////////////////////////////////////////
// class template FunctorImplHelper
// Specialization for 11 parameters
////////////////////////////////////////////////////////////////////////////////
   template <>
   struct FunctorImplHelper<11>
   {
       template <typename R, class TList, class ThreadingModel>
       class In : public Private::FunctorImplBase<R, ThreadingModel>
       {
           typedef typename TL::TypeAt<TList, 0>::Result P1;
           typedef typename TL::TypeAt<TList, 1>::Result P2;
           typedef typename TL::TypeAt<TList, 2>::Result P3;
           typedef typename TL::TypeAt<TList, 3>::Result P4;
           typedef typename TL::TypeAt<TList, 4>::Result P5;
           typedef typename TL::TypeAt<TList, 5>::Result P6;
           typedef typename TL::TypeAt<TList, 6>::Result P7;
           typedef typename TL::TypeAt<TList, 7>::Result P8;
           typedef typename TL::TypeAt<TList, 8>::Result P9;
           typedef typename TL::TypeAt<TList, 9>::Result P10;
           typedef typename TL::TypeAt<TList, 10>::Result P11;

       public:
           typedef R ResultType;
		   typedef typename Select
			<
				IsVoid<R>::value != 0,
				VoidAsType,
				R
			>::Result NewRType;
           typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           typedef typename TypeTraits<P4>::ParameterType Parm4;
           typedef typename TypeTraits<P5>::ParameterType Parm5;
           typedef typename TypeTraits<P6>::ParameterType Parm6;
           typedef typename TypeTraits<P7>::ParameterType Parm7;
           typedef typename TypeTraits<P8>::ParameterType Parm8;
           typedef typename TypeTraits<P9>::ParameterType Parm9;
           typedef typename TypeTraits<P10>::ParameterType Parm10;
           typedef typename TypeTraits<P11>::ParameterType Parm11;
           virtual NewRType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9, Parm10, Parm11) = 0;
       };
   };
////////////////////////////////////////////////////////////////////////////////
// class template FunctorImplHelper
// Specialization for 12 parameters
////////////////////////////////////////////////////////////////////////////////
   template <>
   struct FunctorImplHelper<12>
   {
       template <typename R, class TList, class ThreadingModel>
       class In : public Private::FunctorImplBase<R, ThreadingModel>
       {
           typedef typename TL::TypeAt<TList, 0>::Result P1;
           typedef typename TL::TypeAt<TList, 1>::Result P2;
           typedef typename TL::TypeAt<TList, 2>::Result P3;
           typedef typename TL::TypeAt<TList, 3>::Result P4;
           typedef typename TL::TypeAt<TList, 4>::Result P5;
           typedef typename TL::TypeAt<TList, 5>::Result P6;
           typedef typename TL::TypeAt<TList, 6>::Result P7;
           typedef typename TL::TypeAt<TList, 7>::Result P8;
           typedef typename TL::TypeAt<TList, 8>::Result P9;
           typedef typename TL::TypeAt<TList, 9>::Result P10;
           typedef typename TL::TypeAt<TList, 10>::Result P11;
           typedef typename TL::TypeAt<TList, 11>::Result P12;

       public:
           typedef R ResultType;
		   typedef typename Select
			<
				IsVoid<R>::value != 0,
				VoidAsType,
				R
			>::Result NewRType;
           typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           typedef typename TypeTraits<P4>::ParameterType Parm4;
           typedef typename TypeTraits<P5>::ParameterType Parm5;
           typedef typename TypeTraits<P6>::ParameterType Parm6;
           typedef typename TypeTraits<P7>::ParameterType Parm7;
           typedef typename TypeTraits<P8>::ParameterType Parm8;
           typedef typename TypeTraits<P9>::ParameterType Parm9;
           typedef typename TypeTraits<P10>::ParameterType Parm10;
           typedef typename TypeTraits<P11>::ParameterType Parm11;
           typedef typename TypeTraits<P12>::ParameterType Parm12;
           virtual NewRType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9, Parm10, Parm11, Parm12) = 0;
       };
   };
////////////////////////////////////////////////////////////////////////////////
// class template FunctorImplHelper
// Specialization for 13 parameters
////////////////////////////////////////////////////////////////////////////////
   template <>
   struct FunctorImplHelper<13>
   {
       template <typename R, class TList, class ThreadingModel>
       class In : public Private::FunctorImplBase<R, ThreadingModel>
       {
           typedef typename TL::TypeAt<TList, 0>::Result P1;
           typedef typename TL::TypeAt<TList, 1>::Result P2;
           typedef typename TL::TypeAt<TList, 2>::Result P3;
           typedef typename TL::TypeAt<TList, 3>::Result P4;
           typedef typename TL::TypeAt<TList, 4>::Result P5;
           typedef typename TL::TypeAt<TList, 5>::Result P6;
           typedef typename TL::TypeAt<TList, 6>::Result P7;
           typedef typename TL::TypeAt<TList, 7>::Result P8;
           typedef typename TL::TypeAt<TList, 8>::Result P9;
           typedef typename TL::TypeAt<TList, 9>::Result P10;
           typedef typename TL::TypeAt<TList, 10>::Result P11;
           typedef typename TL::TypeAt<TList, 11>::Result P12;
           typedef typename TL::TypeAt<TList, 12>::Result P13;

       public:
           typedef R ResultType;
		   typedef typename Select
			<
				IsVoid<R>::value != 0,
				VoidAsType,
				R
			>::Result NewRType;
           typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           typedef typename TypeTraits<P4>::ParameterType Parm4;
           typedef typename TypeTraits<P5>::ParameterType Parm5;
           typedef typename TypeTraits<P6>::ParameterType Parm6;
           typedef typename TypeTraits<P7>::ParameterType Parm7;
           typedef typename TypeTraits<P8>::ParameterType Parm8;
           typedef typename TypeTraits<P9>::ParameterType Parm9;
           typedef typename TypeTraits<P10>::ParameterType Parm10;
           typedef typename TypeTraits<P11>::ParameterType Parm11;
           typedef typename TypeTraits<P12>::ParameterType Parm12;
           typedef typename TypeTraits<P13>::ParameterType Parm13;
           virtual NewRType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13) = 0;
       };
   };
////////////////////////////////////////////////////////////////////////////////
// class template FunctorImplHelper
// Specialization for 14 parameters
////////////////////////////////////////////////////////////////////////////////
   template <>
   struct FunctorImplHelper<14>
   {
       template <typename R, class TList, class ThreadingModel>
       class In : public Private::FunctorImplBase<R, ThreadingModel>
       {
           typedef typename TL::TypeAt<TList, 0>::Result P1;
           typedef typename TL::TypeAt<TList, 1>::Result P2;
           typedef typename TL::TypeAt<TList, 2>::Result P3;
           typedef typename TL::TypeAt<TList, 3>::Result P4;
           typedef typename TL::TypeAt<TList, 4>::Result P5;
           typedef typename TL::TypeAt<TList, 5>::Result P6;
           typedef typename TL::TypeAt<TList, 6>::Result P7;
           typedef typename TL::TypeAt<TList, 7>::Result P8;
           typedef typename TL::TypeAt<TList, 8>::Result P9;
           typedef typename TL::TypeAt<TList, 9>::Result P10;
           typedef typename TL::TypeAt<TList, 10>::Result P11;
           typedef typename TL::TypeAt<TList, 11>::Result P12;
           typedef typename TL::TypeAt<TList, 12>::Result P13;
           typedef typename TL::TypeAt<TList, 13>::Result P14;

       public:
           typedef R ResultType;
		   typedef typename Select
			<
				IsVoid<R>::value != 0,
				VoidAsType,
				R
			>::Result NewRType;
           typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           typedef typename TypeTraits<P4>::ParameterType Parm4;
           typedef typename TypeTraits<P5>::ParameterType Parm5;
           typedef typename TypeTraits<P6>::ParameterType Parm6;
           typedef typename TypeTraits<P7>::ParameterType Parm7;
           typedef typename TypeTraits<P8>::ParameterType Parm8;
           typedef typename TypeTraits<P9>::ParameterType Parm9;
           typedef typename TypeTraits<P10>::ParameterType Parm10;
           typedef typename TypeTraits<P11>::ParameterType Parm11;
           typedef typename TypeTraits<P12>::ParameterType Parm12;
           typedef typename TypeTraits<P13>::ParameterType Parm13;
           typedef typename TypeTraits<P14>::ParameterType Parm14;
           virtual NewRType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13, Parm14) = 0;
       };
   };
////////////////////////////////////////////////////////////////////////////////
// class template FunctorImplHelper
// Specialization for 15 parameters
////////////////////////////////////////////////////////////////////////////////
   template <>
   struct FunctorImplHelper<15>
   {
       template <typename R, class TList, class ThreadingModel>
       class In : public Private::FunctorImplBase<R, ThreadingModel>
       {
           typedef typename TL::TypeAt<TList, 0>::Result P1;
           typedef typename TL::TypeAt<TList, 1>::Result P2;
           typedef typename TL::TypeAt<TList, 2>::Result P3;
           typedef typename TL::TypeAt<TList, 3>::Result P4;
           typedef typename TL::TypeAt<TList, 4>::Result P5;
           typedef typename TL::TypeAt<TList, 5>::Result P6;
           typedef typename TL::TypeAt<TList, 6>::Result P7;
           typedef typename TL::TypeAt<TList, 7>::Result P8;
           typedef typename TL::TypeAt<TList, 8>::Result P9;
           typedef typename TL::TypeAt<TList, 9>::Result P10;
           typedef typename TL::TypeAt<TList, 10>::Result P11;
           typedef typename TL::TypeAt<TList, 11>::Result P12;
           typedef typename TL::TypeAt<TList, 12>::Result P13;
           typedef typename TL::TypeAt<TList, 13>::Result P14;
           typedef typename TL::TypeAt<TList, 14>::Result P15;

       public:
           typedef R ResultType;
		   typedef typename Select
			<
				IsVoid<R>::value != 0,
				VoidAsType,
				R
			>::Result NewRType;
           typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           typedef typename TypeTraits<P4>::ParameterType Parm4;
           typedef typename TypeTraits<P5>::ParameterType Parm5;
           typedef typename TypeTraits<P6>::ParameterType Parm6;
           typedef typename TypeTraits<P7>::ParameterType Parm7;
           typedef typename TypeTraits<P8>::ParameterType Parm8;
           typedef typename TypeTraits<P9>::ParameterType Parm9;
           typedef typename TypeTraits<P10>::ParameterType Parm10;
           typedef typename TypeTraits<P11>::ParameterType Parm11;
           typedef typename TypeTraits<P12>::ParameterType Parm12;
           typedef typename TypeTraits<P13>::ParameterType Parm13;
           typedef typename TypeTraits<P14>::ParameterType Parm14;
           typedef typename TypeTraits<P15>::ParameterType Parm15;
           virtual NewRType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13, Parm14, Parm15) = 0;
       };
   };

   } // namespace Private

////////////////////////////////////////////////////////////////////////////////
// class template FunctorImpl
// The base class for a hierarchy of functors. The FunctorImpl class is not used
//     directly; rather, the Functor class manages and forwards to a pointer to
//     FunctorImpl
// You may want to derive your own functors from FunctorImpl.
// Specializations of FunctorImpl for up to 15 parameters follow
////////////////////////////////////////////////////////////////////////////////

    template
    <
        typename R,
        class TList,
        class ThreadingModel = DEFAULT_THREADING
    >
    class FunctorImpl
        : public Private::FunctorImplHelper
          <
              TL::Length<TList>::value
          >
          ::template In<R, TList, ThreadingModel>
    {
        ASSERT_TYPELIST(TList);
		public:
			virtual ~FunctorImpl() {}
    };

////////////////////////////////////////////////////////////////////////////////
namespace Private
{
	template <class R>
	struct FunCallWrapper
	{
		template <class ParentFunctor, class Fun>
		struct In
		{
			typedef typename ParentFunctor::Impl Base;
			typedef typename Base::Parm1 A0;
			typedef typename Base::Parm2 A1;
			typedef typename Base::Parm3 A2;
			typedef typename Base::Parm4 A3;
			typedef typename Base::Parm5 A4;
			typedef typename Base::Parm6 A5;
			typedef typename Base::Parm7 A6;
			typedef typename Base::Parm8 A7;
			typedef typename Base::Parm9 A8;
			typedef typename Base::Parm10 A9;
			typedef typename Base::Parm11 A10;
			typedef typename Base::Parm12 A11;
			typedef typename Base::Parm13 A12;
			typedef typename Base::Parm14 A13;
			typedef typename Base::Parm15 A14;
			private:
				Fun f_;
			public:
				In(Fun f) : f_(f) {}


				R operator() (){return f_();}
				R operator()(A0 p0) {return f_(p0);}
				R operator()(A0 p0, A1 p1) {return f_(p0, p1);}


				R operator()(A0 p0, A1 p1, A2 p2)
				{return f_(p0, p1, p2);}


				R operator()(A0 p0, A1 p1, A2 p2, A3 p3)
				{return f_(p0, p1, p2, p3);}


				R operator()(A0 p0, A1 p1, A2 p2, A3 p3, A4 p4)
				{return f_(p0, p1, p2, p3, p4);}

				R operator()(A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5)
				{return f_(p0, p1, p2, p3, p4, p5);}

				R operator()(A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6)
				{return f_(p0, p1, p2, p3, p4, p5, p6);}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7)
				{return f_(p0, p1, p2, p3, p4, p5, p6, p7);}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8)
				{return f_(p0, p1, p2, p3, p4, p5, p6, p7, p8);}


				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9)
				{return f_(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9);}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10)
				{return f_(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10, A11 p11)
				{return f_(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10, A11 p11, A12 p12)
				{
					return f_(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,
								p12);
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10, A11 p11, A12 p12,
								A13 p13)
				{
					return f_(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,
								p12, p13);
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10, A11 p11, A12 p12,
								A13 p13, A14 p14)
				{
					return f_(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,
								p12, p13, p14);
				}

		};
	};

	template <>
	struct FunCallWrapper<void>
	{
		template <class ParentFunctor, class Fun>
		struct In
		{
			typedef typename ParentFunctor::Impl Base;
			typedef typename Base::Parm1 A0;
			typedef typename Base::Parm2 A1;
			typedef typename Base::Parm3 A2;
			typedef typename Base::Parm4 A3;
			typedef typename Base::Parm5 A4;
			typedef typename Base::Parm6 A5;
			typedef typename Base::Parm7 A6;
			typedef typename Base::Parm8 A7;
			typedef typename Base::Parm9 A8;
			typedef typename Base::Parm10 A9;
			typedef typename Base::Parm11 A10;
			typedef typename Base::Parm12 A11;
			typedef typename Base::Parm13 A12;
			typedef typename Base::Parm14 A13;
			typedef typename Base::Parm15 A14;
			typedef VoidAsType R;
			private:
				Fun f_;
			public:

				In(Fun f) : f_(f) {}

				R operator() (){f_(); return VoidAsType();}
				R operator()(A0 p0) {f_(p0); return VoidAsType();}
				R operator()(A0 p0, A1 p1) {f_(p0, p1); return VoidAsType();}
				R operator()(A0 p0, A1 p1, A2 p2)
				{f_(p0, p1, p2); return VoidAsType();}

				R operator()(A0 p0, A1 p1, A2 p2, A3 p3)
				{f_(p0, p1, p2, p3); return VoidAsType();}

				R operator()(A0 p0, A1 p1, A2 p2, A3 p3, A4 p4)
				{f_(p0, p1, p2, p3, p4); return VoidAsType();}

				R operator()(A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5)
				{f_(p0, p1, p2, p3, p4, p5); return VoidAsType();}

				R operator()(A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6)
				{f_(p0, p1, p2, p3, p4, p5, p6); return VoidAsType();}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7)
				{f_(p0, p1, p2, p3, p4, p5, p6, p7); return VoidAsType();}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8)
				{f_(p0, p1, p2, p3, p4, p5, p6, p7, p8); return VoidAsType();}


				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9)
				{f_(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9); return VoidAsType();}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10)
				{
					f_(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
					return VoidAsType();
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10, A11 p11)
				{
					f_(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
					return VoidAsType();
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10, A11 p11, A12 p12)
				{
					f_(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
					return VoidAsType();
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10, A11 p11, A12 p12,
								A13 p13)
				{
					f_(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,
							p13);
					return VoidAsType();
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10, A11 p11, A12 p12,
								A13 p13, A14 p14)
				{
					f_(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,
							p13, p14);
					return VoidAsType();
				}
		};
	};

	template <class R>
	struct MemFunCallWrapper
	{
		template <class ParentFunctor, class pToObj, class pToMemFun>
		struct In
		{
			typedef typename ParentFunctor::Impl Base;
			typedef typename Base::Parm1 A0;
			typedef typename Base::Parm2 A1;
			typedef typename Base::Parm3 A2;
			typedef typename Base::Parm4 A3;
			typedef typename Base::Parm5 A4;
			typedef typename Base::Parm6 A5;
			typedef typename Base::Parm7 A6;
			typedef typename Base::Parm8 A7;
			typedef typename Base::Parm9 A8;
			typedef typename Base::Parm10 A9;
			typedef typename Base::Parm11 A10;
			typedef typename Base::Parm12 A11;
			typedef typename Base::Parm13 A12;
			typedef typename Base::Parm14 A13;
			typedef typename Base::Parm15 A14;
			private:
				pToObj		pObj_;
				pToMemFun	pMemFun_;
			public:
				In(const pToObj& pObj, pToMemFun pMemFun)
					: pObj_(pObj)
					, pMemFun_(pMemFun)
				{}

				R operator() (){return ((*pObj_).*pMemFun_)();}
				R operator()(A0 p0) {return ((*pObj_).*pMemFun_)(p0);}
				R operator()(A0 p0, A1 p1)
				{
					return ((*pObj_).*pMemFun_)(p0, p1);
				}


				R operator()(A0 p0, A1 p1, A2 p2)
				{
					return ((*pObj_).*pMemFun_)(p0, p1, p2);
				}

				R operator()(A0 p0, A1 p1, A2 p2, A3 p3)
				{
					return ((*pObj_).*pMemFun_)(p0, p1, p2, p3);
				}

				R operator()(A0 p0, A1 p1, A2 p2, A3 p3, A4 p4)
				{
					return ((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4);
				}

				R operator()(A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5)
				{
					return ((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5);
				}

				R operator()(A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6)
				{
					return ((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5, p6);
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7)
				{
					return ((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5, p6,
							p7);
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8)
				{
					return ((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5, p6,
							p7, p8);
				}


				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9)
				{
					return ((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5, p6,
							p7, p8, p9);
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10)
				{
					return ((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5, p6,
							p7, p8, p9, p10);
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10, A11 p11)
				{
					return ((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5, p6,
							p7, p8, p9, p10, p11);
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10, A11 p11, A12 p12)
				{
					return ((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5, p6,
							p7, p8, p9, p10, p11, p12);
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10, A11 p11, A12 p12,
								A13 p13)
				{
					return ((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5, p6,
							p7, p8, p9, p10, p11, p12, p13);
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10, A11 p11, A12 p12,
								A13 p13, A14 p14)
				{
					return ((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5, p6,
							p7, p8, p9, p10, p11, p12, p13, p14);
				}

		};
	};

	template <>
	struct MemFunCallWrapper<void>
	{
		template <class ParentFunctor, class pToObj, class pToMemFun>
		struct In
		{
			typedef VoidAsType R;
			typedef typename ParentFunctor::Impl Base;
			typedef typename Base::Parm1 A0;
			typedef typename Base::Parm2 A1;
			typedef typename Base::Parm3 A2;
			typedef typename Base::Parm4 A3;
			typedef typename Base::Parm5 A4;
			typedef typename Base::Parm6 A5;
			typedef typename Base::Parm7 A6;
			typedef typename Base::Parm8 A7;
			typedef typename Base::Parm9 A8;
			typedef typename Base::Parm10 A9;
			typedef typename Base::Parm11 A10;
			typedef typename Base::Parm12 A11;
			typedef typename Base::Parm13 A12;
			typedef typename Base::Parm14 A13;
			typedef typename Base::Parm15 A14;
			private:
				pToObj		pObj_;
				pToMemFun	pMemFun_;
			public:
				In(const pToObj& pObj, pToMemFun pMemFun)
					: pObj_(pObj)
					, pMemFun_(pMemFun)
				{}

				R operator() (){((*pObj_).*pMemFun_)(); return VoidAsType();}
				R operator()(A0 p0)
				{((*pObj_).*pMemFun_)(p0); return VoidAsType();}
				R operator()(A0 p0, A1 p1)
				{
					((*pObj_).*pMemFun_)(p0, p1);
					return VoidAsType();
				}


				R operator()(A0 p0, A1 p1, A2 p2)
				{
					((*pObj_).*pMemFun_)(p0, p1, p2);
					return VoidAsType();
				}

				R operator()(A0 p0, A1 p1, A2 p2, A3 p3)
				{
					((*pObj_).*pMemFun_)(p0, p1, p2, p3);
					return VoidAsType();
				}

				R operator()(A0 p0, A1 p1, A2 p2, A3 p3, A4 p4)
				{
					((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4);
					return VoidAsType();
				}

				R operator()(A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5)
				{
					((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5);
					return VoidAsType();
				}

				R operator()(A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6)
				{
					((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5, p6);
					return VoidAsType();
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7)
				{
					((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5, p6,
							p7);
					return VoidAsType();
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8)
				{
					((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5, p6,
							p7, p8);
					return VoidAsType();
				}


				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9)
				{
					((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5, p6,
							p7, p8, p9);
					return VoidAsType();
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10)
				{
					((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5, p6,
							p7, p8, p9, p10);
					return VoidAsType();
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10, A11 p11)
				{
					((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5, p6,
							p7, p8, p9, p10, p11);
					return VoidAsType();
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10, A11 p11, A12 p12)
				{
					((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5, p6,
							p7, p8, p9, p10, p11, p12);
					return VoidAsType();
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10, A11 p11, A12 p12,
								A13 p13)
				{
					((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5, p6,
							p7, p8, p9, p10, p11, p12, p13);
					return VoidAsType();
				}

				R operator()(	A0 p0, A1 p1, A2 p2, A3 p3, A4 p4, A5 p5, A6 p6,
								A7 p7, A8 p8, A9 p9, A10 p10, A11 p11, A12 p12,
								A13 p13, A14 p14)
				{
					((*pObj_).*pMemFun_)(p0, p1, p2, p3, p4, p5, p6,
							p7, p8, p9, p10, p11, p12, p13, p14);
					return VoidAsType();
				}

		};
	};

}

////////////////////////////////////////////////////////////////////////////////
// class template FunctorHandler
// Wraps functors and pointers to functions
////////////////////////////////////////////////////////////////////////////////
	template <class ParentFunctor, typename Fun>
	class FunctorHandler : public ParentFunctor::Impl
    {
        typedef typename ParentFunctor::Impl Base;

    public:
		typedef typename Base::ResultType ResultType;
		typedef typename Base::NewRType NewRType;
		typedef typename ParentFunctor::ThreadModel ThreadModel;
        typedef typename Base::Parm1 Parm1;
        typedef typename Base::Parm2 Parm2;
        typedef typename Base::Parm3 Parm3;
        typedef typename Base::Parm4 Parm4;
        typedef typename Base::Parm5 Parm5;
        typedef typename Base::Parm6 Parm6;
        typedef typename Base::Parm7 Parm7;
        typedef typename Base::Parm8 Parm8;
        typedef typename Base::Parm9 Parm9;
        typedef typename Base::Parm10 Parm10;
        typedef typename Base::Parm11 Parm11;
        typedef typename Base::Parm12 Parm12;
        typedef typename Base::Parm13 Parm13;
        typedef typename Base::Parm14 Parm14;
        typedef typename Base::Parm15 Parm15;
	private:
		typedef typename
		Private::FunCallWrapper<ResultType>::template In
		<ParentFunctor, Fun> CallImplType;
	public:
        FunctorHandler(const Fun& fun) : CallImpl_(fun) {}

        DEFINE_CLONE_FUNCTORIMPL(FunctorHandler)

        // operator() implementations for up to 15 arguments

        NewRType operator()()
        {
			return CallImpl_();
		}

        NewRType operator()(Parm1 p1)
        { return CallImpl_(p1);}

        NewRType operator()(Parm1 p1, Parm2 p2)
        { return CallImpl_(p1, p2); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3)
        { return CallImpl_(p1, p2, p3); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4)
        { return CallImpl_(p1, p2, p3, p4);}

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5)
        { return CallImpl_(p1, p2, p3, p4, p5);}

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6)
        { return CallImpl_(p1, p2, p3, p4, p5, p6);}

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7)
        { return CallImpl_(p1, p2, p3, p4, p5, p6, p7);}

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8)
        { return CallImpl_(p1, p2, p3, p4, p5, p6, p7, p8);}

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9)
        { return CallImpl_(p1, p2, p3, p4, p5, p6, p7, p8, p9);}

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10)
        { return CallImpl_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);}

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11)
        { return CallImpl_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10,
									p11);}

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12)
        { return CallImpl_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10,
									p11, p12);}

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13)
        { return CallImpl_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10,
									p11, p12, p13);}

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14)
        { return CallImpl_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10,
									p11, p12, p13, p14);}

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14, Parm15 p15)
        { return CallImpl_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10,
									p11, p12, p13, p14, p15);}

    private:
        CallImplType CallImpl_;
    };



////////////////////////////////////////////////////////////////////////////////
// class template FunctorHandler
// Wraps pointers to member functions
////////////////////////////////////////////////////////////////////////////////

    template <class ParentFunctor, typename PointerToObj,
        typename PointerToMemFn>
    class MemFunHandler : public ParentFunctor::Impl
    {
        typedef typename ParentFunctor::Impl Base;

    public:
        typedef typename Base::ResultType ResultType;
        typedef typename Base::NewRType NewRType;
		typedef typename ParentFunctor::ThreadModel ThreadModel;
		typedef typename Base::Parm1 Parm1;
        typedef typename Base::Parm2 Parm2;
        typedef typename Base::Parm3 Parm3;
        typedef typename Base::Parm4 Parm4;
        typedef typename Base::Parm5 Parm5;
        typedef typename Base::Parm6 Parm6;
        typedef typename Base::Parm7 Parm7;
        typedef typename Base::Parm8 Parm8;
        typedef typename Base::Parm9 Parm9;
        typedef typename Base::Parm10 Parm10;
        typedef typename Base::Parm11 Parm11;
        typedef typename Base::Parm12 Parm12;
        typedef typename Base::Parm13 Parm13;
        typedef typename Base::Parm14 Parm14;
        typedef typename Base::Parm15 Parm15;
	private:
		typedef typename Private::MemFunCallWrapper<ResultType>::template In
		<
			ParentFunctor,
			PointerToObj,
			PointerToMemFn
		> CallImplType;

	public:
		MemFunHandler(const PointerToObj& pObj, PointerToMemFn pMemFn)
			: CallImpl_(pObj, pMemFn)
        {}

        DEFINE_CLONE_FUNCTORIMPL(MemFunHandler)

		NewRType operator()()
        { return CallImpl_(); }

        NewRType operator()(Parm1 p1)
        { return CallImpl_(p1); }

        NewRType operator()(Parm1 p1, Parm2 p2)
        { return CallImpl_(p1, p2); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3)
        { return CallImpl_(p1, p2, p3); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4)
        { return CallImpl_(p1, p2, p3, p4); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5)
        { return CallImpl_(p1, p2, p3, p4, p5); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6)
        { return CallImpl_(p1, p2, p3, p4, p5, p6); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7)
        { return CallImpl_(p1, p2, p3, p4, p5, p6, p7); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8)
        { return CallImpl_(p1, p2, p3, p4, p5, p6, p7, p8); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9)
        { return CallImpl_(p1, p2, p3, p4, p5, p6, p7, p8, p9); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10)
        { return CallImpl_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11)
        {
            return CallImpl_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10,
                p11);
        }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12)
        {
            return CallImpl_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10,
                p11, p12);
        }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13)
        {
            return CallImpl_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10,
                p11, p12, p13);
        }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14)
        {
            return CallImpl_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10,
                p11, p12, p13, p14);
        }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14, Parm15 p15)
        {
            return CallImpl_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10,
                p11, p12, p13, p14, p15);
        }

    private:
        CallImplType CallImpl_;
    };


////////////////////////////////////////////////////////////////////////////////
// class template Functor
// A generalized functor implementation with value semantics
////////////////////////////////////////////////////////////////////////////////
struct Disambiguate
{
	Disambiguate() {}
};

    template<typename R, class TList = NullType,
			class ThreadingModel = DEFAULT_THREADING>
    class Functor
    {
    public:
        // Handy type definitions for the body type
        typedef FunctorImpl<R, TList, ThreadingModel> Impl;
        typedef R               ResultType;
        typedef TList           ParmList;
        typedef ThreadingModel	ThreadModel;
        typedef typename Select
			<	Private::IsVoid<R>::value != 0,
				Private::VoidAsType, R
			>::Result NewRType;

		typedef typename Impl::Parm1 Parm1;
        typedef typename Impl::Parm2 Parm2;
        typedef typename Impl::Parm3 Parm3;
        typedef typename Impl::Parm4 Parm4;
        typedef typename Impl::Parm5 Parm5;
        typedef typename Impl::Parm6 Parm6;
        typedef typename Impl::Parm7 Parm7;
        typedef typename Impl::Parm8 Parm8;
        typedef typename Impl::Parm9 Parm9;
        typedef typename Impl::Parm10 Parm10;
        typedef typename Impl::Parm11 Parm11;
        typedef typename Impl::Parm12 Parm12;
        typedef typename Impl::Parm13 Parm13;
        typedef typename Impl::Parm14 Parm14;
        typedef typename Impl::Parm15 Parm15;
	public:

		// Member functions
		Functor() : spImpl_()
        {}

		// we can't use loki's original approach with VC 6 because
		// this compiler will not recognize the copy-ctor
		// if a template-ctor is present.
		//template <typename Fun>
        //Functor(Fun fun)
        //: spImpl_(new FunctorHandler<Functor, Fun>(fun))
        //{}

		template <class Fun>
		Functor(Fun fun, Loki::Disambiguate)
			: spImpl_(new FunctorHandler<Functor, Fun>(fun))
		{}

		Functor(const Functor& rhs) : spImpl_(Impl::Clone(rhs.spImpl_.get()))
        {}
		// Ctor for user defined impl-classes
		Functor(std::auto_ptr<Impl> spImpl) : spImpl_(spImpl)
        {}

		// ctors for functions with up to 15 arguments.
		template <class Ret>
		Functor(Ret (*p)()) : spImpl_(new FunctorHandler<Functor, Ret(*)()>(p))
		{}

		template <class Ret, class A0>
		Functor(Ret (*p)(A0))
			: spImpl_(new FunctorHandler<Functor, Ret(*)(A0)>(p))
		{}

		template <class Ret, class A0, class A1>
		Functor(Ret (*p)(A0, A1))
			: spImpl_(new FunctorHandler<Functor, Ret(*)(A0, A1)>(p))
		{}

		template <class Ret, class A0, class A1, class A2>
		Functor(Ret (*p)(A0, A1, A2))
			: spImpl_(new FunctorHandler<Functor, Ret(*)(A0, A1, A2)>(p))
		{}

		template <class Ret, class A0, class A1, class A2, class A3>
		Functor(Ret (*p)(A0, A1, A2, A3))
			: spImpl_(new FunctorHandler<Functor, Ret(*)(A0, A1, A2, A3)>(p))
		{}

		template <class Ret, class A0, class A1, class A2, class A3, class A4>
		Functor(Ret (*p)(A0, A1, A2, A3, A4))
			: spImpl_(new FunctorHandler<Functor, Ret(*)(A0, A1, A2, A3, A4)>(p))
		{}

		template
		<
			class Ret, class A0, class A1, class A2, class A3, class A4,
			class A5
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5))
			: spImpl_(new FunctorHandler<Functor,
			Ret(*)(A0, A1, A2, A3, A4, A5)>(p))
		{}

		template
		<
			class Ret, class A0, class A1, class A2, class A3, class A4,
			class A5, class A6
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5, A6))
			: spImpl_(new FunctorHandler<Functor,
			Ret(*)(A0, A1, A2, A3, A4, A5, A6)>(p))
		{}

		template
		<
			class Ret, class A0, class A1, class A2, class A3, class A4,
			class A5, class A6, class A7
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5, A6, A7))
			: spImpl_(new FunctorHandler<Functor,
			Ret(*)(A0, A1, A2, A3, A4, A5, A6, A7)>(p))
		{}

		template
		<
			class Ret, class A0, class A1, class A2, class A3, class A4,
			class A5, class A6, class A7, class A8
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5, A6, A7, A8))
			: spImpl_(new FunctorHandler<Functor,
			Ret(*)(A0, A1, A2, A3, A4, A5, A6, A7, A8)>(p))
		{}

		template
		<
			class Ret, class A0, class A1, class A2, class A3, class A4,
			class A5, class A6, class A7, class A8, class A9
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9))
			: spImpl_(new FunctorHandler<Functor,
			Ret(*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9)>(p))
		{}


		template
		<
			class Ret, class A0, class A1, class A2, class A3, class A4,
			class A5, class A6, class A7, class A8, class A9, class A10
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
			: spImpl_(new FunctorHandler<Functor,
			Ret(*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)>(p))
		{}

		template
		<
			class Ret, class A0, class A1, class A2, class A3, class A4,
			class A5, class A6, class A7, class A8, class A9, class A10,
			class A11
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11))
			: spImpl_(new FunctorHandler<Functor,
			Ret(*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)>(p))
		{}

		template
		<
			class Ret, class A0, class A1, class A2, class A3, class A4,
			class A5, class A6, class A7, class A8, class A9, class A10,
			class A11, class A12
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12))
			: spImpl_(new FunctorHandler<Functor,
			Ret(*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)>(p))
		{}

		template
		<
			class Ret, class A0, class A1, class A2, class A3, class A4,
			class A5, class A6, class A7, class A8, class A9, class A10,
			class A11, class A12, class A13
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13))
			: spImpl_(new FunctorHandler<Functor,
			Ret(*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)>(p))
		{}

		template
		<
			class Ret, class A0, class A1, class A2, class A3, class A4,
			class A5, class A6, class A7, class A8, class A9, class A10,
			class A11, class A12, class A13, class A14
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14))
			: spImpl_(new FunctorHandler<Functor,
			Ret(*)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14)>(p))
		{}

        // alternative way to create a functor using a function pointer.
		// The two dummy parameters are needed to disambiguate this ctor
		// from the ctor for creating functors with an object and a pointer
		// to a member-function.
		// Use the macro FUNCTION_POINTER to create objects, e.g.
		// Functor<int, TYPELIST_2(int, int)> obj(pFunc, FUNCTION_POINTER);
		/*
		template <class Fun>
		Functor(Fun fun, int* pDummy1, int* pDummy2)
        : spImpl_(new FunctorHandler<Functor, Fun>(fun))
        {}
		*/
		template <class PtrObj, typename MemFn>
        Functor(const PtrObj& p, MemFn memFn)
        : spImpl_(new MemFunHandler<Functor, PtrObj, MemFn>(p, memFn))
        {}

		Functor& operator=(const Functor& rhs)
        {
            if (this == &rhs)
				return *this;
			// the auto_ptr provided by the MSVC 6.0 does not have
			// a reset-function.
			Functor copy(rhs);
			delete spImpl_.release();
			spImpl_ = copy.spImpl_;

            return *this;
        }

		NewRType operator()()
        { return (*spImpl_)(); }

		NewRType operator()(Parm1 p1)
        { return (*spImpl_)(p1); }

        NewRType operator()(Parm1 p1, Parm2 p2)
        { return (*spImpl_)(p1, p2); }

		NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3)
        { return (*spImpl_)(p1, p2, p3); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4)
        { return (*spImpl_)(p1, p2, p3, p4); }

		NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5)
        { return (*spImpl_)(p1, p2, p3, p4, p5); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6)
        { return (*spImpl_)(p1, p2, p3, p4, p5, p6); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7)
        { return (*spImpl_)(p1, p2, p3, p4, p5, p6, p7); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8)
        { return (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9)
        { return (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10)
        { return (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11)
        { return (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12)
        {
            return (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,
                p12);
        }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13)
        {
            return (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,
            p12, p13);
        }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14)
        {
            return (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,
                p12, p13, p14);
        }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14, Parm15 p15)
        {
            return (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,
                p12, p13, p14, p15);
        }
		typedef Impl * (std::auto_ptr<Impl>::*unspecified_bool_type)() const;

        operator unspecified_bool_type() const
        {
            return spImpl_.get() ? &std::auto_ptr<Impl>::get : 0;
        }
    private:
		std::auto_ptr<Impl> spImpl_;

    };

    namespace Private
    {
        template <class Fctor>
        struct BinderFirstTraits
        {
        private:
            typedef typename Fctor::ParmList   TList;

        public:
            typedef typename TL::Erase
            <
                TList,
                typename TL::TypeAt<TList, 0>::Result
            >
            ::Result ParmList;
			typedef typename Fctor::ThreadModel ThreadModel;


			typedef Functor<typename Fctor::ResultType,
				ParmList, ThreadModel> BoundFunctorType;
			typedef typename BoundFunctorType::Impl Impl;
        };

    } // namespace Private

////////////////////////////////////////////////////////////////////////////////
// class template BinderFirst
// Binds the first parameter of a Functor object to a specific value
////////////////////////////////////////////////////////////////////////////////

    template <class OriginalFunctor>
    class BinderFirst
        : public Private::BinderFirstTraits<OriginalFunctor>::Impl
    {
        typedef typename Private::BinderFirstTraits<OriginalFunctor>::Impl Base;
        typedef typename Private::BinderFirstTraits<OriginalFunctor>::ThreadModel ThreadModel;
		typedef typename OriginalFunctor::ResultType ResultType;
		typedef typename OriginalFunctor::NewRType NewRType;
		/*typedef typename Select
			<	Private::IsVoid<ResultType>::value != 0,
				Private::VoidAsType, ResultType
			>::Result NewRType;*/
        typedef typename OriginalFunctor::Parm1 BoundType;

        typedef typename OriginalFunctor::Parm2 Parm1;
        typedef typename OriginalFunctor::Parm3 Parm2;
        typedef typename OriginalFunctor::Parm4 Parm3;
        typedef typename OriginalFunctor::Parm5 Parm4;
        typedef typename OriginalFunctor::Parm6 Parm5;
        typedef typename OriginalFunctor::Parm7 Parm6;
        typedef typename OriginalFunctor::Parm8 Parm7;
        typedef typename OriginalFunctor::Parm9 Parm8;
        typedef typename OriginalFunctor::Parm10 Parm9;
        typedef typename OriginalFunctor::Parm11 Parm10;
        typedef typename OriginalFunctor::Parm12 Parm11;
        typedef typename OriginalFunctor::Parm13 Parm12;
        typedef typename OriginalFunctor::Parm14 Parm13;
        typedef typename OriginalFunctor::Parm15 Parm14;
        typedef EmptyType Parm15;
    public:
        BinderFirst(const OriginalFunctor& fun, BoundType bound)
        : f_(fun), b_(bound)
        {}

        DEFINE_CLONE_FUNCTORIMPL(BinderFirst)

        // operator() implementations for up to 15 arguments

		NewRType operator()()
        { return f_(b_); }

        NewRType operator()(Parm1 p1)
        { return f_(b_, p1); }

		NewRType operator()(Parm1 p1, Parm2 p2)
        { return f_(b_, p1, p2); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3)
        { return f_(b_, p1, p2, p3); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4)
        { return f_(b_, p1, p2, p3, p4); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5)
        { return f_(b_, p1, p2, p3, p4, p5); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6)
        { return f_(b_, p1, p2, p3, p4, p5, p6); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7)
        { return f_(b_, p1, p2, p3, p4, p5, p6, p7); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8)
        { return f_(b_, p1, p2, p3, p4, p5, p6, p7, p8); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9)
        { return f_(b_, p1, p2, p3, p4, p5, p6, p7, p8, p9); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10)
        { return f_(b_, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11)
        { return f_(b_, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12)
        { return f_(b_, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13)
        { return f_(b_, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14)
        {
            return f_(b_, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13,
                p14);
        }

        // VC7 BUG
        virtual ~BinderFirst() {}
    private:
        OriginalFunctor f_;
        BoundType b_;
    };

////////////////////////////////////////////////////////////////////////////////
// function template BindFirst
// Binds the first parameter of a Functor object to a specific value
////////////////////////////////////////////////////////////////////////////////

	// The return-type of the original library is to complicated for
	// the VC 6.0
	// template <class Fctor>
    // typename Private::BinderFirstTraits<Fctor>::BoundFunctorType
    // BindFirst(
	// const Fctor& fun,
	// typename Fctor::Parm1 bound)
    // {
    //	typedef typename Private::BinderFirstTraits<Fctor>::BoundFunctorType
	//	Outgoing;
	//
	//	return Outgoing(std::auto_ptr<Outgoing::Impl>(
	//	new BinderFirst<Fctor>(fun, bound)));
    //	}

	namespace Private
	{
		template <class TList>
		struct BinderFirstTraitsHelper
		{
			typedef typename TL::Erase
            <
                TList,
                typename TL::TypeAt<TList, 0>::Result
            >
            ::Result ParmList;
		};
	}

	template <class R, class TList, class Thread>
	Functor<R, Private::BinderFirstTraitsHelper<TList>::ParmList, Thread>
	BindFirst(const Functor<R, TList, Thread>& fun, Functor<R, TList, Thread>::Parm1 bound)
	{
		typedef typename Private::BinderFirstTraitsHelper<TList>::ParmList NewList;
		typedef typename Functor<R, NewList, Thread> Out;

		return Out(std::auto_ptr<Out::Impl>(
			new BinderFirst<Functor<R, TList, Thread> >(fun, bound)));
	}

////////////////////////////////////////////////////////////////////////////////
// class template Chainer
// Chains two functor calls one after another
////////////////////////////////////////////////////////////////////////////////

    template <typename Fun1, typename Fun2>
    class Chainer : public Fun2::Impl
    {
        typedef Fun2 Base;

    public:
        typedef typename Base::ResultType ResultType;
		typedef typename Base::ThreadModel ThreadModel;
        typedef typename Select
			<	Private::IsVoid<ResultType>::value != 0,
				Private::VoidAsType, ResultType
			>::Result NewRType;
		typedef typename Base::Parm1 Parm1;
        typedef typename Base::Parm2 Parm2;
        typedef typename Base::Parm3 Parm3;
        typedef typename Base::Parm4 Parm4;
        typedef typename Base::Parm5 Parm5;
        typedef typename Base::Parm6 Parm6;
        typedef typename Base::Parm7 Parm7;
        typedef typename Base::Parm8 Parm8;
        typedef typename Base::Parm9 Parm9;
        typedef typename Base::Parm10 Parm10;
        typedef typename Base::Parm11 Parm11;
        typedef typename Base::Parm12 Parm12;
        typedef typename Base::Parm13 Parm13;
        typedef typename Base::Parm14 Parm14;
        typedef typename Base::Parm15 Parm15;

        Chainer(const Fun1& fun1, const Fun2& fun2) : f1_(fun1), f2_(fun2) {}

        //
        // VC7 don't see this implicit constructor
        //

        virtual ~Chainer() {}

        DEFINE_CLONE_FUNCTORIMPL(Chainer)

        // operator() implementations for up to 15 arguments

        NewRType operator()()
        { return f1_(), f2_(); }

        NewRType operator()(Parm1 p1)
        { return f1_(p1), f2_(p1); }

        NewRType operator()(Parm1 p1, Parm2 p2)
        { return f1_(p1, p2), f2_(p1, p2); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3)
        { return f1_(p1, p2, p3), f2_(p1, p2, p3); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4)
        { return f1_(p1, p2, p3, p4), f2_(p1, p2, p3, p4); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5)
        { return f1_(p1, p2, p3, p4, p5), f2_(p1, p2, p3, p4, p5); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6)
        { return f1_(p1, p2, p3, p4, p5, p6), f2_(p1, p2, p3, p4, p5, p6); }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7)
        {
            return f1_(p1, p2, p3, p4, p5, p6, p7),
                f2_(p1, p2, p3, p4, p5, p6, p7);
        }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8)
        {
            return f1_(p1, p2, p3, p4, p5, p6, p7, p8),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8);
        }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9)
        {
            return f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9);
        }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10)
        {
            return f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
        }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11)
        {
            return f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
        }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12)
        {
            return f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
        }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13)
        {
            return f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
        }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14)
        {
            return f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13,
                    p14),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13,
                   p14);
        }

        NewRType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14, Parm15 p15)
        {
            return f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13,
                    p14, p15),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13,
                    p14, p15);
        }

    private:
        Fun1 f1_;
        Fun2 f2_;
    };

////////////////////////////////////////////////////////////////////////////////
// function template Chain
// Chains two functor calls one after another
////////////////////////////////////////////////////////////////////////////////


    template <class Fun1, class Fun2>
    Fun2 Chain(
        const Fun1& fun1,
        const Fun2& fun2)
    {
        //
        // VC7 don't support the auto_ptr tricks
        //
        std::auto_ptr<Fun2::Impl> apParam
        (
            new Chainer<Fun1, Fun2>(fun1, fun2)
        );

        return Fun2(apParam);
    }

} // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// May  10, 2002: ported by Rani Sharoni to VC7 (RTM - 9466)
// Oct	12, 2002: ported by Benjamin Kaufmann to MSVC 6
// Feb	22, 2003: corrected the return type template paramter of functors constructors
//					for function pointers.
//					Added Loki::Disambiguate parameter to the template conversion ctor.
//					changed the implementation of BindFirst to circumvent
//					C1001-Internal-Compiler errors.
//					B.K.
// Feb	25, 2003: added conversion unspecified_bool_type. B.K.
////////////////////////////////////////////////////////////////////////////////

#endif  // FUNCTOR_INC_
