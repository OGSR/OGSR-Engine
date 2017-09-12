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
//
// Last update: Feb 25, 2003
// Functor no longer uses the udt VoidAsType as a workaround for void returns.
// Instead I created several pairs of base classes (one the general case and 
// for the void case)
// The original classes now derive from one of these classes depending on the
// actual return type.
// 
// If you don't like this workaround you can switch to the old version
// by defining the macro USE_OLD_FUNCTOR_VERSION
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
    
#ifdef USE_FUNCTOR_OLD_VERSION
#include "FunctorOld.h"
#else
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
			virtual ResultType operator()() = 0;
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
            typedef typename TypeTraits<P1>::ParameterType Parm1;
            virtual ResultType operator()(Parm1) = 0;
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
		   
           typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           virtual ResultType operator()(Parm1, Parm2) = 0;
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
           
		   typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           virtual ResultType operator()(Parm1, Parm2, Parm3) = 0;
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
           
		   typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           typedef typename TypeTraits<P4>::ParameterType Parm4;
           virtual ResultType operator()(Parm1, Parm2, Parm3, Parm4) = 0;
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
		   
           typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           typedef typename TypeTraits<P4>::ParameterType Parm4;
           typedef typename TypeTraits<P5>::ParameterType Parm5;
           virtual ResultType operator()(Parm1, Parm2, Parm3, Parm4, Parm5) = 0;
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
           
		   typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           typedef typename TypeTraits<P4>::ParameterType Parm4;
           typedef typename TypeTraits<P5>::ParameterType Parm5;
           typedef typename TypeTraits<P6>::ParameterType Parm6;
           virtual ResultType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6) = 0;
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
		   typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           typedef typename TypeTraits<P4>::ParameterType Parm4;
           typedef typename TypeTraits<P5>::ParameterType Parm5;
           typedef typename TypeTraits<P6>::ParameterType Parm6;
           typedef typename TypeTraits<P7>::ParameterType Parm7;
           virtual ResultType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7) = 0;
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
		   typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           typedef typename TypeTraits<P4>::ParameterType Parm4;
           typedef typename TypeTraits<P5>::ParameterType Parm5;
           typedef typename TypeTraits<P6>::ParameterType Parm6;
           typedef typename TypeTraits<P7>::ParameterType Parm7;
           typedef typename TypeTraits<P8>::ParameterType Parm8;
           virtual ResultType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8) = 0;
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
		   typedef typename TypeTraits<P1>::ParameterType Parm1;
           typedef typename TypeTraits<P2>::ParameterType Parm2;
           typedef typename TypeTraits<P3>::ParameterType Parm3;
           typedef typename TypeTraits<P4>::ParameterType Parm4;
           typedef typename TypeTraits<P5>::ParameterType Parm5;
           typedef typename TypeTraits<P6>::ParameterType Parm6;
           typedef typename TypeTraits<P7>::ParameterType Parm7;
           typedef typename TypeTraits<P8>::ParameterType Parm8;
           typedef typename TypeTraits<P9>::ParameterType Parm9;
           virtual ResultType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9) = 0;
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
           virtual ResultType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9, Parm10) = 0;
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
           virtual ResultType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9, Parm10, Parm11) = 0;
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
           virtual ResultType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9, Parm10, Parm11, Parm12) = 0;
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
           virtual ResultType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13) = 0;
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
           virtual ResultType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13, Parm14) = 0;
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
           virtual ResultType operator()(Parm1, Parm2, Parm3, Parm4, Parm5, Parm6, Parm7, Parm8, Parm9, Parm10, Parm11, Parm12, Parm13, Parm14, Parm15) = 0;
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
	// implementation class for FunctorHandler and return type != void	
	template <class ParentFunctor, typename Fun>
	struct FunctorHandlerBase : public ParentFunctor::Impl
	{
		typedef typename ParentFunctor::Impl Base;

    public:
        typedef typename ParentFunctor::ThreadModel ThreadModel;
		typedef typename Base::ResultType ResultType;
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
        
        FunctorHandlerBase(const Fun& fun) : f_(fun) {}

        // operator() implementations for up to 15 arguments
                
        ResultType operator()()
        { return f_(); }

        ResultType operator()(Parm1 p1)
        { return f_(p1); }
        
        ResultType operator()(Parm1 p1, Parm2 p2)
        { return f_(p1, p2); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3)
        { return f_(p1, p2, p3); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4)
        { return f_(p1, p2, p3, p4); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5)
        { return f_(p1, p2, p3, p4, p5); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6)
        { return f_(p1, p2, p3, p4, p5, p6); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7)
        { return f_(p1, p2, p3, p4, p5, p6, p7); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8)
        { return f_(p1, p2, p3, p4, p5, p6, p7, p8); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9)
        { return f_(p1, p2, p3, p4, p5, p6, p7, p8, p9); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10)
        { return f_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11)
        { return f_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12)
        { return f_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13)
        { return f_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14)
        {
            return f_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, 
                p14);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14, Parm15 p15)
        {
            return f_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, 
                p14, p15);
        }
	protected:
		~FunctorHandlerBase() {}
    private:
        Fun f_;
    };

	// implementation class for FunctorHandler and return type = void
	template <class ParentFunctor, typename Fun>
	struct FunctorHandlerVoidBase : public ParentFunctor::Impl
	{
		typedef typename ParentFunctor::Impl Base;

    public:
        typedef typename ParentFunctor::ThreadModel ThreadModel;
		typedef typename Base::ResultType ResultType;
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
        
        FunctorHandlerVoidBase(const Fun& fun) : f_(fun) {}
        
        // operator() implementations for up to 15 arguments
                
        ResultType operator()()
        { f_(); }

        ResultType operator()(Parm1 p1)
        { f_(p1); }
        
        ResultType operator()(Parm1 p1, Parm2 p2)
        { f_(p1, p2); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3)
        { f_(p1, p2, p3); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4)
        { f_(p1, p2, p3, p4); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5)
        { f_(p1, p2, p3, p4, p5); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6)
        { f_(p1, p2, p3, p4, p5, p6); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7)
        { f_(p1, p2, p3, p4, p5, p6, p7); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8)
        { f_(p1, p2, p3, p4, p5, p6, p7, p8); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9)
        { f_(p1, p2, p3, p4, p5, p6, p7, p8, p9); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10)
        { f_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11)
        { f_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12)
        { f_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13)
        { f_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14)
        {
            f_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, 
                p14);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14, Parm15 p15)
        {
            f_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, 
                p14, p15);
        }
	protected:
		~FunctorHandlerVoidBase() {}        
    private:
        Fun f_;
    };
	
}
	
////////////////////////////////////////////////////////////////////////////////
// class template FunctorHandler
// Wraps functors and pointers to functions
////////////////////////////////////////////////////////////////////////////////
	template <class ParentFunctor, typename Fun>
    class FunctorHandler
		: public Select<
			IsEqualType<typename ParentFunctor::Impl::ResultType, void>::value,
			Private::FunctorHandlerVoidBase<ParentFunctor, Fun>,
			Private::FunctorHandlerBase<ParentFunctor, Fun>
			>::Result
    {
        typedef typename Select<
			IsEqualType<typename ParentFunctor::Impl::ResultType, void>::value,
			Private::FunctorHandlerVoidBase<ParentFunctor, Fun>,
			Private::FunctorHandlerBase<ParentFunctor, Fun>
			>::Result ImplBase;
		typedef typename ParentFunctor::Impl Base;
    public:
        typedef typename ParentFunctor::ThreadModel ThreadModel;
		typedef typename Base::ResultType ResultType;
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
        
        FunctorHandler(const Fun& fun) : ImplBase(fun) {}
        
        DEFINE_CLONE_FUNCTORIMPL(FunctorHandler)

		// all function operators are implemented in the helper
		// base class
    };


////////////////////////////////////////////////////////////////////////////////
namespace Private
{
	// implementation class for MemFunHandler and return type != void
	template <class ParentFunctor, typename PointerToObj,
        typename PointerToMemFn>
    class MemFunHandlerBase : public ParentFunctor::Impl
    {
        typedef typename ParentFunctor::Impl Base;

    public:
        typedef typename ParentFunctor::ThreadModel ThreadModel;
		typedef typename Base::ResultType ResultType;
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

        MemFunHandlerBase(const PointerToObj& pObj, PointerToMemFn pMemFn) 
        : pObj_(pObj), pMemFn_(pMemFn)
        {}
        
        ResultType operator()()
        { return ((*pObj_).*pMemFn_)(); }

        ResultType operator()(Parm1 p1)
        { return ((*pObj_).*pMemFn_)(p1); }
        
        ResultType operator()(Parm1 p1, Parm2 p2)
        { return ((*pObj_).*pMemFn_)(p1, p2); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3)
        { return ((*pObj_).*pMemFn_)(p1, p2, p3); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4)
        { return ((*pObj_).*pMemFn_)(p1, p2, p3, p4); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5)
        { return ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6)
        { return ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7)
        { return ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6, p7); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8)
        { return ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6, p7, p8); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9)
        { return ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6, p7, p8, p9); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10)
        { return ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11)
        {
            return ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, 
                p11);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12)
        {
            return ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, 
                p11, p12);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13)
        {
            return ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, 
                p11, p12, p13);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14)
        {
            return ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, 
                p11, p12, p13, p14);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14, Parm15 p15)
        {
            return ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, 
                p11, p12, p13, p14, p15);
        }
        
    protected:
		~MemFunHandlerBase() {}
	private:
        PointerToObj pObj_;
        PointerToMemFn pMemFn_;
    };
	
	// implementation class for MemFunHandler and return type = void
	template <class ParentFunctor, typename PointerToObj,
        typename PointerToMemFn>
    class MemFunHandlerVoidBase : public ParentFunctor::Impl
    {
        typedef typename ParentFunctor::Impl Base;

    public:
        typedef typename ParentFunctor::ThreadModel ThreadModel;
		typedef typename Base::ResultType ResultType;
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

        MemFunHandlerVoidBase(const PointerToObj& pObj, PointerToMemFn pMemFn) 
        : pObj_(pObj), pMemFn_(pMemFn)
        {}
        
        ResultType operator()()
        { ((*pObj_).*pMemFn_)(); }

        ResultType operator()(Parm1 p1)
        { ((*pObj_).*pMemFn_)(p1); }
        
        ResultType operator()(Parm1 p1, Parm2 p2)
        { ((*pObj_).*pMemFn_)(p1, p2); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3)
        { ((*pObj_).*pMemFn_)(p1, p2, p3); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4)
        { ((*pObj_).*pMemFn_)(p1, p2, p3, p4); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5)
        { ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6)
        { ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7)
        { ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6, p7); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8)
        { ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6, p7, p8); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9)
        { ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6, p7, p8, p9); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10)
        { ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11)
        {
            ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, 
                p11);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12)
        {
            ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, 
                p11, p12);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13)
        {
            ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, 
                p11, p12, p13);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14)
        {
            ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, 
                p11, p12, p13, p14);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14, Parm15 p15)
        {
            ((*pObj_).*pMemFn_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, 
                p11, p12, p13, p14, p15);
        }
	protected:
		~MemFunHandlerVoidBase() {}
    private:
        PointerToObj pObj_;
        PointerToMemFn pMemFn_;
    };

}
////////////////////////////////////////////////////////////////////////////////
// class template FunctorHandler
// Wraps pointers to member functions
////////////////////////////////////////////////////////////////////////////////

    template <class ParentFunctor, typename PointerToObj,
        typename PointerToMemFn>
    class MemFunHandler 
		: public Select<
			IsEqualType<typename ParentFunctor::Impl::ResultType, void>::value,
			Private::MemFunHandlerVoidBase<ParentFunctor, PointerToObj, PointerToMemFn>,
			Private::MemFunHandlerBase<ParentFunctor, PointerToObj, PointerToMemFn>
			>::Result
    {
        typedef typename Select<
			IsEqualType<typename ParentFunctor::Impl::ResultType, void>::value,
			Private::MemFunHandlerVoidBase<ParentFunctor, PointerToObj, PointerToMemFn>,
			Private::MemFunHandlerBase<ParentFunctor, PointerToObj, PointerToMemFn>
			>::Result ImplBase;
		
		typedef typename ParentFunctor::Impl Base;

    public:
        typedef typename Base::ResultType ResultType;
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
	
	public:
		MemFunHandler(const PointerToObj& pObj, PointerToMemFn pMemFn) 
			: ImplBase(pObj, pMemFn)
        {}
        
        DEFINE_CLONE_FUNCTORIMPL(MemFunHandler)

		// all function operators are implemented in the helper
		// base class
    };
        
////////////////////////////////////////////////////////////////////////////////
template<typename R, class TList = NullType,
			class ThreadingModel = DEFAULT_THREADING>
			class Functor;
// Helper type used to disambiguate Functor's first template ctor
// from the copy-ctor.
struct Disambiguate
{
	Disambiguate() {}
};
namespace Private
{
	// implementation class for Functor with return type != void
	template <class R, class Impl>
	class FunctorBase
	{
		typedef R ResultType;

	public:
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
		FunctorBase() : spImpl_() {}
		
		template <class Fun, class MyFunctor>
		FunctorBase(Fun fun, Loki::Type2Type<MyFunctor>) 
			: spImpl_(new FunctorHandler<MyFunctor, Fun>(fun))
		{}

		FunctorBase(const FunctorBase& rhs) 
			: spImpl_(Impl::Clone(rhs.spImpl_.get()))
        {}
		
		// Ctor for user defined impl-classes
		FunctorBase(std::auto_ptr<Impl> spImpl) : spImpl_(spImpl)
        {}

		template <class PtrObj, typename MemFn, class MyFunctor>
		FunctorBase(const PtrObj& p, MemFn memFn, Loki::Type2Type<MyFunctor>)
        : spImpl_(new MemFunHandler<MyFunctor, PtrObj, MemFn>(p, memFn))
        {}

		ResultType operator()()
        { return (*spImpl_)(); }

		ResultType operator()(Parm1 p1)
        { return (*spImpl_)(p1); }
        
        ResultType operator()(Parm1 p1, Parm2 p2)
        { return (*spImpl_)(p1, p2); }
        
		ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3)
        { return (*spImpl_)(p1, p2, p3); }

        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4)
        { return (*spImpl_)(p1, p2, p3, p4); }
        
		ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5)
        { return (*spImpl_)(p1, p2, p3, p4, p5); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6)
        { return (*spImpl_)(p1, p2, p3, p4, p5, p6); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7)
        { return (*spImpl_)(p1, p2, p3, p4, p5, p6, p7); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8)
        { return (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9)
        { return (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10)
        { return (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11)
        { return (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12)
        {
            return (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, 
                p12);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13)
        {
            return (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,
            p12, p13);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14)
        {
            return (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, 
                p12, p13, p14);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14, Parm15 p15)
        {
            return (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, 
                p12, p13, p14, p15);
        }	
		protected:
			std::auto_ptr<Impl> spImpl_;
			~FunctorBase() {}
	};

	// implementation class for Functor with return type = void
	template <class Impl>
	class FunctorVoidBase
	{
		typedef void ResultType;
	public:
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

		FunctorVoidBase() : spImpl_() {}
		
		template <class Fun, class MyFunctor>
		FunctorVoidBase(Fun fun, Loki::Type2Type<MyFunctor>) 
			: spImpl_(new FunctorHandler<MyFunctor, Fun>(fun))
		{}

		FunctorVoidBase(const FunctorVoidBase& rhs) 
			: spImpl_(Impl::Clone(rhs.spImpl_.get()))
        {}
		
		// Ctor for user defined impl-classes
		FunctorVoidBase(std::auto_ptr<Impl> spImpl) : spImpl_(spImpl)
        {}

		template <class PtrObj, typename MemFn, class MyFunctor>
		FunctorVoidBase(const PtrObj& p, MemFn memFn, Loki::Type2Type<MyFunctor>)
        : spImpl_(new MemFunHandler<MyFunctor, PtrObj, MemFn>(p, memFn))
        {}
		
		
		ResultType operator()()
        { (*spImpl_)(); }

		ResultType operator()(Parm1 p1)
        { (*spImpl_)(p1); }
        
        ResultType operator()(Parm1 p1, Parm2 p2)
        { (*spImpl_)(p1, p2); }
        
		ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3)
        { (*spImpl_)(p1, p2, p3); }

        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4)
        { (*spImpl_)(p1, p2, p3, p4); }
        
		ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5)
        { (*spImpl_)(p1, p2, p3, p4, p5); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6)
        { (*spImpl_)(p1, p2, p3, p4, p5, p6); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7)
        { (*spImpl_)(p1, p2, p3, p4, p5, p6, p7); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8)
        { (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9)
        { (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10)
        { (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11)
        { (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12)
        {
            (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, 
                p12);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13)
        {
            (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11,
            p12, p13);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14)
        {
            (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, 
                p12, p13, p14);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14, Parm15 p15)
        {
            (*spImpl_)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, 
                p12, p13, p14, p15);
        }	
		protected:
			std::auto_ptr<Impl> spImpl_;
			~FunctorVoidBase() {}
	};
}


////////////////////////////////////////////////////////////////////////////////
// class template Functor
// A generalized functor implementation with value semantics
////////////////////////////////////////////////////////////////////////////////
    template<typename R, class TList = NullType,
			class ThreadingModel = DEFAULT_THREADING>
	class Functor : public Select<
						IsEqualType<R, void>::value,
						Private::FunctorVoidBase<FunctorImpl<R, TList, ThreadingModel> >,
						Private::FunctorBase<R, FunctorImpl<R, TList, ThreadingModel> >
						>::Result
    {
    typedef typename Select<
			IsEqualType<R, void>::value,
			Private::FunctorVoidBase<FunctorImpl<R, TList, ThreadingModel> >,
			Private::FunctorBase<R, FunctorImpl<R, TList, ThreadingModel> >
			>::Result ImplBase;
	public:
        
		// Handy type definitions for the body type
        typedef FunctorImpl<R, TList, ThreadingModel> Impl;
        typedef R               ResultType;
        typedef TList           ParmList;
        typedef ThreadingModel	ThreadModel;
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
		Functor() : ImplBase()
        {}
		        
		// we can't use loki's original ctor with VC 6 because
		// this compiler will not recognize the copy-ctor 
		// if this template-ctor is present.
		//template <typename Fun>
        //Functor(Fun fun)
        //: spImpl_(new FunctorHandler<Functor, Fun>(fun))
        //{}
		// The Loki::Disambiguate parameters makes this ctor distingushable
		// from the copy-ctor. 
		template <class Fun>
		Functor(Fun fun, Loki::Disambiguate) 
			: ImplBase(fun, Type2Type<Functor>())
		{}

		Functor(const Functor& rhs) : ImplBase(rhs)
        {}
		
		// Ctor for user defined impl-classes
		Functor(std::auto_ptr<Impl> spImpl) : ImplBase(spImpl) 
        {}
		
		// ctors for functions with up to 15 arguments.
		template <class Ret>
		Functor(Ret (*p)()) : ImplBase(p, Type2Type<Functor>())
		{}
        
		template <class Ret, class A0>
		Functor(Ret (*p)(A0)) 
			: ImplBase(p, Type2Type<Functor>())
		{}

		template <class Ret, class A0, class A1>
		Functor(Ret (*p)(A0, A1)) 
			: ImplBase(p, Type2Type<Functor>())
		{}

		template <class Ret, class A0, class A1, class A2>
		Functor(Ret (*p)(A0, A1, A2)) 
			: ImplBase(p, Type2Type<Functor>())
		{}

		template <class Ret, class A0, class A1, class A2, class A3>
		Functor(Ret (*p)(A0, A1, A2, A3)) 
			: ImplBase(p, Type2Type<Functor>())
		{}

		template <class Ret, class A0, class A1, class A2, class A3, class A4>
		Functor(Ret (*p)(A0, A1, A2, A3, A4)) 
			: ImplBase(p, Type2Type<Functor>())
		{}

		template 
		<	
			class Ret, class A0, class A1, class A2, class A3, class A4, 
			class A5
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5)) 
			: ImplBase(p, Type2Type<Functor>())
		{}

		template 
		<
			class Ret, class A0, class A1, class A2, class A3, class A4, 
			class A5, class A6
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5, A6)) 
			: ImplBase(p, Type2Type<Functor>())
		{}

		template 
		<	
			class Ret, class A0, class A1, class A2, class A3, class A4, 
			class A5, class A6, class A7
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5, A6, A7)) 
			: ImplBase(p, Type2Type<Functor>())
		{}

		template 
		<	
			class Ret, class A0, class A1, class A2, class A3, class A4, 
			class A5, class A6, class A7, class A8
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5, A6, A7, A8)) 
			: ImplBase(p, Type2Type<Functor>())
		{}

		template 
		<	
			class Ret, class A0, class A1, class A2, class A3, class A4, 
			class A5, class A6, class A7, class A8, class A9
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9)) 
			: ImplBase(p, Type2Type<Functor>())
		{}

		
		template 
		<	
			class Ret, class A0, class A1, class A2, class A3, class A4, 
			class A5, class A6, class A7, class A8, class A9, class A10
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)) 
			: ImplBase(p, Type2Type<Functor>())
		{}

		template 
		<	
			class Ret, class A0, class A1, class A2, class A3, class A4, 
			class A5, class A6, class A7, class A8, class A9, class A10, 
			class A11
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)) 
			: ImplBase(p, Type2Type<Functor>())
		{}

		template 
		<	
			class Ret, class A0, class A1, class A2, class A3, class A4, 
			class A5, class A6, class A7, class A8, class A9, class A10, 
			class A11, class A12
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)) 
			: ImplBase(p, Type2Type<Functor>())
		{}

		template 
		<	
			class Ret, class A0, class A1, class A2, class A3, class A4, 
			class A5, class A6, class A7, class A8, class A9, class A10, 
			class A11, class A12, class A13
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)) 
			: ImplBase(p, Type2Type<Functor>())
		{}

		template 
		<	
			class Ret, class A0, class A1, class A2, class A3, class A4, 
			class A5, class A6, class A7, class A8, class A9, class A10, 
			class A11, class A12, class A13, class A14
		>
		Functor(Ret (*p)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14)) 
			: ImplBase(p, Type2Type<Functor>())
		{}
		
        template <class PtrObj, typename MemFn>
        Functor(const PtrObj& p, MemFn memFn)
        : ImplBase(p, memFn, Type2Type<Functor>())
        {}
		
		Functor& operator=(const Functor& rhs)
        {
            if (this == &rhs) 
				return *this;
			// the auto_ptr provided by the MSVC 6.0 does not have
			// a reset-function.
			Functor copy(rhs);
			delete ImplBase::spImpl_.release();
			ImplBase::spImpl_ = copy.ImplBase::spImpl_;
			
            return *this;
        }

		typedef Impl * (std::auto_ptr<Impl>::*unspecified_bool_type)() const;

        operator unspecified_bool_type() const
        {
            return spImpl_.get() ? &std::auto_ptr<Impl>::get : 0;
        }
        
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
namespace Private
{
	// implementation class for BinderFirst and return type != void
	template <class OriginalFunctor>
	struct BinderFirstBase : public Private::BinderFirstTraits<OriginalFunctor>::Impl
	{
		typedef typename Private::BinderFirstTraits<OriginalFunctor>::Impl Base;
        typedef typename Private::BinderFirstTraits<OriginalFunctor>::ThreadModel ThreadModel;
		typedef typename OriginalFunctor::ResultType ResultType;
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

		BinderFirstBase(const OriginalFunctor& fun, BoundType bound)
			: f_(fun), b_(bound)
		{}

		ResultType operator()()
        { return f_(b_); }

        ResultType operator()(Parm1 p1)
        { return f_(b_, p1); }

		ResultType operator()(Parm1 p1, Parm2 p2)
        { return f_(b_, p1, p2); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3)
        { return f_(b_, p1, p2, p3); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4)
        { return f_(b_, p1, p2, p3, p4); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5)
        { return f_(b_, p1, p2, p3, p4, p5); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6)
        { return f_(b_, p1, p2, p3, p4, p5, p6); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7)
        { return f_(b_, p1, p2, p3, p4, p5, p6, p7); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8)
        { return f_(b_, p1, p2, p3, p4, p5, p6, p7, p8); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9)
        { return f_(b_, p1, p2, p3, p4, p5, p6, p7, p8, p9); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10)
        { return f_(b_, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11)
        { return f_(b_, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12)
        { return f_(b_, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13)
        { return f_(b_, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14)
        {
            return f_(b_, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, 
                p14);
        }

	protected:
		~BinderFirstBase() {}
    private:
        OriginalFunctor f_;
        BoundType b_;
	};
	
	// implementation class for BinderFirst and return type = void
	template <class OriginalFunctor>
	struct BinderFirstVoidBase : public Private::BinderFirstTraits<OriginalFunctor>::Impl
	{
		typedef typename Private::BinderFirstTraits<OriginalFunctor>::Impl Base;
        typedef typename Private::BinderFirstTraits<OriginalFunctor>::ThreadModel ThreadModel;
		typedef typename OriginalFunctor::ResultType ResultType;
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

		BinderFirstVoidBase(const OriginalFunctor& fun, BoundType bound)
			: f_(fun), b_(bound)
		{}
		ResultType operator()()
        { f_(b_); }

        ResultType operator()(Parm1 p1)
        { f_(b_, p1); }

		ResultType operator()(Parm1 p1, Parm2 p2)
        { f_(b_, p1, p2); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3)
        { f_(b_, p1, p2, p3); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4)
        { f_(b_, p1, p2, p3, p4); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5)
        { f_(b_, p1, p2, p3, p4, p5); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6)
        { f_(b_, p1, p2, p3, p4, p5, p6); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7)
        { f_(b_, p1, p2, p3, p4, p5, p6, p7); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8)
        { f_(b_, p1, p2, p3, p4, p5, p6, p7, p8); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9)
        { f_(b_, p1, p2, p3, p4, p5, p6, p7, p8, p9); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10)
        { f_(b_, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11)
        { f_(b_, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12)
        { f_(b_, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13)
        { f_(b_, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14)
        {
            f_(b_, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, 
                p14);
        }

        protected:
			~BinderFirstVoidBase() {}
    private:
        OriginalFunctor f_;
        BoundType b_;
	};
}
////////////////////////////////////////////////////////////////////////////////
// class template BinderFirst
// Binds the first parameter of a Functor object to a specific value
////////////////////////////////////////////////////////////////////////////////

    template <class OriginalFunctor>
    class BinderFirst 
        : public Select<
			IsEqualType<OriginalFunctor::ResultType, void>::value,
			Private::BinderFirstVoidBase<OriginalFunctor>,
			Private::BinderFirstBase<OriginalFunctor>
			>::Result
    {
        typedef typename Select<
			IsEqualType<OriginalFunctor::ResultType, void>::value,
			Private::BinderFirstVoidBase<OriginalFunctor>,
			Private::BinderFirstBase<OriginalFunctor>
			>::Result ImplBase;

		typedef typename Private::BinderFirstTraits<OriginalFunctor>::Impl Base;
        typedef typename Private::BinderFirstTraits<OriginalFunctor>::ThreadModel ThreadModel;
		typedef typename OriginalFunctor::ResultType ResultType;
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
			: ImplBase(fun,bound)
        {}
        
		DEFINE_CLONE_FUNCTORIMPL(BinderFirst)
		
		// all function operators are implemented in the base class

		// VC7 BUG
        virtual ~BinderFirst() {}
        
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
namespace Private
{
	// implementation class for Chainer and return type != void
	template <class Fun1, class Fun2>
	struct ChainerBase : public Fun2::Impl
	{
		typedef Fun2 Base;
	public:
		typedef typename Base::ResultType ResultType;
		typedef typename Base::ThreadModel ThreadModel;
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
        
        ChainerBase(const Fun1& fun1, const Fun2& fun2) 
			: f1_(fun1), f2_(fun2) {}

        
        // operator() implementations for up to 15 arguments

        ResultType operator()()
        { return f1_(), f2_(); }

        ResultType operator()(Parm1 p1)
        { return f1_(p1), f2_(p1); }
        
        ResultType operator()(Parm1 p1, Parm2 p2)
        { return f1_(p1, p2), f2_(p1, p2); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3)
        { return f1_(p1, p2, p3), f2_(p1, p2, p3); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4)
        { return f1_(p1, p2, p3, p4), f2_(p1, p2, p3, p4); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5)
        { return f1_(p1, p2, p3, p4, p5), f2_(p1, p2, p3, p4, p5); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6)
        { return f1_(p1, p2, p3, p4, p5, p6), f2_(p1, p2, p3, p4, p5, p6); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7)
        {
            return f1_(p1, p2, p3, p4, p5, p6, p7),
                f2_(p1, p2, p3, p4, p5, p6, p7);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8)
        {
            return f1_(p1, p2, p3, p4, p5, p6, p7, p8),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9)
        {
            return f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10)
        {
            return f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11)
        {
            return f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12)
        {
            return f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13)
        {
            return f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14)
        {
            return f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, 
                    p14),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, 
                   p14);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14, Parm15 p15)
        {
            return f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, 
                    p14, p15),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, 
                    p14, p15);
        }
        
    protected:
        Fun1 f1_;
        Fun2 f2_;
		~ChainerBase() {}
 
	};
	
	// implementation class for Chainer and return type = void
	template <class Fun1, class Fun2>
	struct ChainerVoidBase : public Fun2::Impl
	{
		typedef Fun2 Base;
	public:
		typedef typename Base::ResultType ResultType;
		typedef typename Base::ThreadModel ThreadModel;
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
        
        ChainerVoidBase(const Fun1& fun1, const Fun2& fun2) 
			: f1_(fun1), f2_(fun2) {}

        // operator() implementations for up to 15 arguments

        ResultType operator()()
        { f1_(), f2_(); }

        ResultType operator()(Parm1 p1)
        { f1_(p1), f2_(p1); }
        
        ResultType operator()(Parm1 p1, Parm2 p2)
        { f1_(p1, p2), f2_(p1, p2); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3)
        { f1_(p1, p2, p3), f2_(p1, p2, p3); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4)
        { f1_(p1, p2, p3, p4), f2_(p1, p2, p3, p4); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5)
        { f1_(p1, p2, p3, p4, p5), f2_(p1, p2, p3, p4, p5); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6)
        { f1_(p1, p2, p3, p4, p5, p6), f2_(p1, p2, p3, p4, p5, p6); }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7)
        {
            f1_(p1, p2, p3, p4, p5, p6, p7),
                f2_(p1, p2, p3, p4, p5, p6, p7);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8)
        {
            f1_(p1, p2, p3, p4, p5, p6, p7, p8),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9)
        {
            f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10)
        {
            f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11)
        {
            f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12)
        {
            f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13)
        {
            f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14)
        {
            f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, 
                    p14),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, 
                   p14);
        }
        
        ResultType operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5,
            Parm6 p6, Parm7 p7, Parm8 p8, Parm9 p9, Parm10 p10, Parm11 p11,
            Parm12 p12, Parm13 p13, Parm14 p14, Parm15 p15)
        {
            f1_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, 
                    p14, p15),
                f2_(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, 
                    p14, p15);
        }
        
    protected:
        Fun1 f1_;
        Fun2 f2_;
		~ChainerVoidBase() {}
	};
}
////////////////////////////////////////////////////////////////////////////////
// class template Chainer
// Chains two functor calls one after another
////////////////////////////////////////////////////////////////////////////////

    template <typename Fun1, typename Fun2>
	class Chainer 
		: public Select<
			IsEqualType<typename Fun2::ResultType, void>::value,
			Private::ChainerVoidBase<Fun1, Fun2>,
			Private::ChainerBase<Fun1, Fun2>
			>::Result

    {
        typedef typename Select<
			IsEqualType<typename Fun2::ResultType, void>::value,
			Private::ChainerVoidBase<Fun1, Fun2>,
			Private::ChainerBase<Fun1, Fun2>
			>::Result ImplBase;
		
		typedef Fun2 Base;

    public:
        typedef typename Base::ResultType ResultType;
		typedef typename Base::ThreadModel ThreadModel;
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
        
        Chainer(const Fun1& fun1, const Fun2& fun2) : ImplBase(fun1,fun2) {}

        //
        // VC7 don't see this implicit constructor
        //

        virtual ~Chainer() {}

        DEFINE_CLONE_FUNCTORIMPL(Chainer)
		
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
//					Replaced the void return workaround.
//					B.K.
// Feb	25, 2003: added conversion unspecified_bool_type. B.K.
// Mar	06, 2003: added protected destructors to private implementation classes B.K.
////////////////////////////////////////////////////////////////////////////////
#endif  // FUNCTOR_INC_
#endif	// #ifdef USE_FUNCTOR_OLD_VERSION