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

// Last update: Mar 21, 2003
// Added a new explicit template argument specification workaround for 
// FnDispatcher::Add which is more compliant with other 
// explicit template argument specification workarounds used in this port.
//
// Example usage:
// --------------
// Using the original library one writes:
// typedef FnDispatcher<Shape> Dispatcher;
// void Hatch(Rectangle& lhs, Poly& rhs) {...}
// 
// Dispatcher dis;
// disp.Add<Rectangle, Poly, &Hatch>();
//
// Using this port the example either becomes:
//	dis.Add(Dispatcher::Add<Rectangle, Poly, &Hatch>());
// or alternatively
//	Dispatcher::AddI<Rectangle, Poly, &Hatch>()(dis);
// 
// All dispatchers now have void as default value for return types.
// All dispatchers now support void as return type. 
// 
//
// The port now uses Tye2Type-parameters instead of plain pointers as
// a workaround of VC's explicit template argument specification bug.
// 
// For example:
// The original declaration of BasicDispatcher::Add looks like this:
//
//	template <class SomeLhs, class SomeRhs>
//	void Add(CallbackType fun);
// 
// and you call it like this:
//	obj.Add<Type1, Type2>(yourFun);
//
// This port uses:
//
//	template <class SomeLhs, class SomeRhs>
//	void Add(CallbackType fun, Type2Type<SomeLhs>, Type2Type<SomeRhs>);
//
// and you call it like this:
//	obj.Add(yourFun, Type2Type<Type1>(), Type2Type<Type2>());


#ifndef MULTIMETHODS_INC_
#define MULTIMETHODS_INC_

#include "Typelist.h"
#include "LokiTypeInfo.h"
#include "Functor.h"
#include "AssocVector.h"

////////////////////////////////////////////////////////////////////////////////
// IMPORTANT NOTE:
// The double dispatchers implemented below differ from the excerpts shown in
// the book - they are simpler while respecting the same interface.
////////////////////////////////////////////////////////////////////////////////

namespace Loki
{

////////////////////////////////////////////////////////////////////////////////
// Implementation helpers for StaticDispatcher
////////////////////////////////////////////////////////////////////////////////
	
namespace Private
{
	template <class SomeLhs, class SomeRhs, class Executor, typename ResultType>
	struct InvocationTraitsBase
	{
	protected:
		template <class R>
		struct BaseImpl
		{
			static ResultType 
			DoDispatch(SomeLhs& lhs, SomeRhs& rhs, Executor& exec, Int2Type<false>)
			{
				return exec.Fire(lhs, rhs);
			}
			static ResultType DoDispatch(	SomeLhs& lhs, SomeRhs& rhs, 
											Executor& exec, Int2Type<true>)
			{
				return exec.Fire(rhs, lhs);
			}
		};
		template <>
		struct BaseImpl<void>
		{
			static ResultType 
			DoDispatch(SomeLhs& lhs, SomeRhs& rhs, Executor& exec, Int2Type<false>)
			{
				exec.Fire(lhs, rhs);
			}
			static ResultType DoDispatch(	SomeLhs& lhs, SomeRhs& rhs, 
											Executor& exec, Int2Type<true>)
			{
				exec.Fire(rhs, lhs);
			}
		};
	public:
		typedef BaseImpl<ResultType> Base;
	};
	template <class SomeLhs, class SomeRhs, class Executor, typename ResultType>
	struct InvocationTraits : public InvocationTraitsBase
								<SomeLhs, SomeRhs, Executor, ResultType>::Base
	{
		
	};
	
	template<class Executor, class BaseLhs, class TypesLhs, bool symmetric,
		class BaseRhs, class TypesRhs, typename ResultType>
	class StaticDispatcherBase
	{
	private:
		// Base for ResultType != void
		template <class R>
		class In
		{
			template <class SomeLhs>
			static ResultType DispatchRhs(	SomeLhs& lhs, BaseRhs& rhs,
											Executor exec, NullType)
			{ return exec.OnError(lhs, rhs); }
    
			template <class Head, class Tail, class SomeLhs>
			static ResultType DispatchRhs(SomeLhs& lhs, BaseRhs& rhs,
											Executor exec, Typelist<Head, Tail>)
			{            
				if (Head* p2 = dynamic_cast<Head*>(&rhs))
				{
					Int2Type<(symmetric &&
							  int(TL::IndexOf<TypesRhs, Head>::value) <
							  int(TL::IndexOf<TypesLhs, SomeLhs>::value))> i2t;

					typedef Private::InvocationTraits< 
							SomeLhs, Head, Executor, ResultType> CallTraits;
                
					return CallTraits::DoDispatch(lhs, *p2, exec, i2t);
				}
				return DispatchRhs(lhs, rhs, exec, Tail());
			}

			static ResultType DispatchLhs(	BaseLhs& lhs, BaseRhs& rhs,
											Executor exec, NullType)
			{ return exec.OnError(lhs, rhs); }
    
			template <class Head, class Tail>
			static ResultType DispatchLhs(	BaseLhs& lhs, BaseRhs& rhs,
											Executor exec, Typelist<Head, Tail>)
			{            
				if (Head* p1 = dynamic_cast<Head*>(&lhs))
				{
					return DispatchRhs(*p1, rhs, exec, TypesRhs());
				}
				return DispatchLhs(lhs, rhs, exec, Tail());
			}

		protected:
			~In() {}
		public:
			static ResultType Go(	BaseLhs& lhs, BaseRhs& rhs,
								Executor exec)
			{ return DispatchLhs(lhs, rhs, exec, TypesLhs()); }
		};	// In<R>
	
		// Base for ResultType == void
		template <>
		class In<void>
		{
			template <class SomeLhs>
			static ResultType DispatchRhs(	SomeLhs& lhs, BaseRhs& rhs,
											Executor exec, NullType)
			{ exec.OnError(lhs, rhs); }
    
			template <class Head, class Tail, class SomeLhs>
			static ResultType DispatchRhs(SomeLhs& lhs, BaseRhs& rhs,
											Executor exec, Typelist<Head, Tail>)
			{            
				if (Head* p2 = dynamic_cast<Head*>(&rhs))
				{
					Int2Type<(symmetric &&
							  int(TL::IndexOf<TypesRhs, Head>::value) <
							  int(TL::IndexOf<TypesLhs, SomeLhs>::value))> i2t;

					typedef Private::InvocationTraits< 
							SomeLhs, Head, Executor, void> CallTraits;
                
					CallTraits::DoDispatch(lhs, *p2, exec, i2t);
					return;
				}
				DispatchRhs(lhs, rhs, exec, Tail());
			}

			static ResultType DispatchLhs(	BaseLhs& lhs, BaseRhs& rhs,
											Executor exec, NullType)
			{ exec.OnError(lhs, rhs); }
    
			template <class Head, class Tail>
			static ResultType DispatchLhs(	BaseLhs& lhs, BaseRhs& rhs,
											Executor exec, Typelist<Head, Tail>)
			{            
				if (Head* p1 = dynamic_cast<Head*>(&lhs))
				{
					DispatchRhs(*p1, rhs, exec, TypesRhs());
					return;
				}
				DispatchLhs(lhs, rhs, exec, Tail());
			}

		protected:
			~In<void>() {}
		public:
			static ResultType Go(	BaseLhs& lhs, BaseRhs& rhs,
									Executor exec)
			{ DispatchLhs(lhs, rhs, exec, TypesLhs()); }
		};	// In<void>
	public:
		typedef In<ResultType> Base;
	};	// StaticDispatcherBase
	
}	// namespace Private

////////////////////////////////////////////////////////////////////////////////
// class template StaticDispatcher
// Implements an automatic static double dispatcher based on two typelists
////////////////////////////////////////////////////////////////////////////////
	template
    <
        class Executor,
        class BaseLhs, 
        class TypesLhs,
        bool symmetric = true,
        class BaseRhs = BaseLhs,
        class TypesRhs = TypesLhs,
        typename ResultType = Loki::Private::VoidWrap::type
    >
    class StaticDispatcher : public ::Loki::Private::StaticDispatcherBase
								<
									Executor, BaseLhs, TypesLhs, symmetric, 
									BaseRhs,  TypesRhs, ResultType
								>::Base
    {
        
    public:
        // member functions moved to base class
		// static ResultType Go(	BaseLhs& lhs, BaseRhs& rhs,
		// 							Executor exec)
    };    

////////////////////////////////////////////////////////////////////////////////
// Implementation helpers for BasicDispatcher
////////////////////////////////////////////////////////////////////////////////
namespace Private
{
	template <class BaseLhs, class BaseRhs, typename ResultType,
		typename CallbackType> 
	class BasicDispatcherBase
	{
	private:
		// Common (independent of the result type) code for BasicDispatcher
		class Common
		{
		private:
			void DoAdd(TypeInfo lhs, TypeInfo rhs, CallbackType fun)
			{
				callbackMap_[KeyType(lhs, rhs)] = fun;
			}
			bool DoRemove(TypeInfo lhs, TypeInfo rhs)
			{
				return callbackMap_.erase(KeyType(lhs, rhs)) == 1;
			}
		protected:
			typedef std::pair<TypeInfo,TypeInfo> KeyType;
			typedef CallbackType MappedType;
			typedef AssocVector<KeyType, MappedType> MapType;
			MapType callbackMap_;
			~Common() {}
		public:
			template <class SomeLhs, class SomeRhs>
			void Add(CallbackType fun, ::Loki::Type2Type<SomeLhs>,
											::Loki::Type2Type<SomeRhs>)
			{
				DoAdd(typeid(SomeLhs), typeid(SomeRhs), fun);
			}


			template <class SomeLhs, class SomeRhs>
			bool Remove(::Loki::Type2Type<SomeLhs>, ::Loki::Type2Type<SomeRhs>)
			{
				return DoRemove(typeid(SomeLhs), typeid(SomeRhs));
			}
		};	// Common
		template <class R>	// Base for ResultType != void
		class In : public Common
		{
		public:	
			ResultType Go(BaseLhs& lhs, BaseRhs& rhs)
			{
				typename MapType::key_type k(typeid(lhs),typeid(rhs));
				typename MapType::iterator i = Common::callbackMap_.find(k);
				if (i == Common::callbackMap_.end())
				{
					throw std::runtime_error("Function not found");
				}
				return (i->second)(lhs, rhs);
			}
		protected:
			~In() {}	
		};	// In
		template <>	// Base for ResultType == void
		class In<void> : public Common
		{
		public:	
			ResultType Go(BaseLhs& lhs, BaseRhs& rhs)
			{
				typename MapType::key_type k(typeid(lhs),typeid(rhs));
				typename MapType::iterator i = Common::callbackMap_.find(k);
				if (i == Common::callbackMap_.end())
				{
					throw std::runtime_error("Function not found");
				}
				(i->second)(lhs, rhs);
			}
		protected:
			~In<void>() {}	
		};	// In
		
	public:
		typedef In<ResultType> Base;
	};	// BasicDispatcherBase
	
} // namespace Private

////////////////////////////////////////////////////////////////////////////////
// class template BasicDispatcher
// Implements a logarithmic double dispatcher for functors (or functions)
// Doesn't offer automated casts or symmetry
////////////////////////////////////////////////////////////////////////////////
	template
    <
        class BaseLhs,
        class BaseRhs = BaseLhs,
        typename ResultType = Loki::Private::VoidWrap::type,
        typename CallbackType = ResultType (*)(BaseLhs&, BaseRhs&)
    >
    class BasicDispatcher : public ::Loki::Private::BasicDispatcherBase
							<
								BaseLhs, BaseRhs, ResultType, CallbackType
							>::Base
    {
        public:
			// member functions moved to base class
			// template <class SomeLhs, class SomeRhs>
			// void Add(CallbackType fun,	::Loki::Type2Type<SomeLhs>,
			//								::Loki::Type2Type<SomeRhs>)
			//
			// template <class SomeLhs, class SomeRhs>
			// bool Remove(::Loki::Type2Type<SomeLhs>, ::Loki::Type2Type<SomeRhs>)
			//
			// ResultType Go(BaseLhs& lhs, BaseRhs& rhs);
    };


	struct BasicDispatcherWrapper
	{
		template <class T, class U, class V, class W>
		struct In
		{
			typedef BasicDispatcher<T, U, V, W> type;
		};
	};
////////////////////////////////////////////////////////////////////////////////
// class template StaticCaster
// Implementation of the CastingPolicy used by FunctorDispatcher
////////////////////////////////////////////////////////////////////////////////

    template <class To, class From>
    struct StaticCaster
    {
        static To& Cast(From& obj)
        {
            return static_cast<To&>(obj);
        }
    };
	struct StaticCasterWrapper
	{
		template <class T, class U>
		struct In
		{
			typedef StaticCaster<T, U> type;
		};
	};
////////////////////////////////////////////////////////////////////////////////
// class template DynamicCaster
// Implementation of the CastingPolicy used by FunctorDispatcher
////////////////////////////////////////////////////////////////////////////////

    template <class To, class From>
    struct DynamicCaster
    {
        static To& Cast(From& obj)
        {
			return dynamic_cast<To&>(obj);
        }
    };
	struct DynamicCasterWrapper
	{
		template <class T, class U>
		struct In
		{
			typedef DynamicCaster<T, U> type;
		};
	};

////////////////////////////////////////////////////////////////////////////////
// class template Private::FnDispatcherHelper
// Implements trampolines and argument swapping used by FnDispatcher
////////////////////////////////////////////////////////////////////////////////
	namespace Private
    {
		template <class BaseLhs, class BaseRhs, typename ResultType,
			class DispatcherBackend>
		class FnDispatcherBase
		{
		private:
			// Implementation for ResultType != void
			template <class R>
			class In
			{
			public:
				ApplyInnerType4<DispatcherBackend, BaseLhs, BaseRhs, ResultType,
				ResultType (*)(BaseLhs&, BaseRhs&)>::type backEnd_;

				ResultType Go(BaseLhs& lhs, BaseRhs& rhs)
				{
					return backEnd_.Go(lhs, rhs);
				}
			protected:
				~In() {}	
			};	// In
			// Implementation for ResultType == void
			template <>
			class In<void>
			{
			public:
				ApplyInnerType4<DispatcherBackend, BaseLhs, BaseRhs, ResultType,
				ResultType (*)(BaseLhs&, BaseRhs&)>::type backEnd_;

				ResultType Go(BaseLhs& lhs, BaseRhs& rhs)
				{
					backEnd_.Go(lhs, rhs);
				}
			protected:
				~In<void>() {}	
			};	// In<void>
		public:
			typedef In<ResultType> Base;
		};	// FnDispatcherBase
	
		template< class BaseLhs, class BaseRhs, class SomeLhs, class SomeRhs,
				class ResultType, class CastLhs, class CastRhs, 
				void (*Callback)(SomeLhs&, SomeRhs&)>
		class FnDispatcherHelperBase
		{
		private:
			template <class R>
			class In
			{
			public:
				static ResultType Trampoline(BaseLhs& lhs, BaseRhs& rhs)
				{
					return Callback(CastLhs::Cast(lhs), CastRhs::Cast(rhs));
				}
				static ResultType TrampolineR(BaseRhs& rhs, BaseLhs& lhs)
				{
					return Trampoline(lhs, rhs);
				}
			protected:
				~In() {}
			};
			template <>
			class In<void>
			{
			public:
				static ResultType Trampoline(BaseLhs& lhs, BaseRhs& rhs)
				{
					Callback(CastLhs::Cast(lhs), CastRhs::Cast(rhs));
				}
				static ResultType TrampolineR(BaseRhs& rhs, BaseLhs& lhs)
				{
					Trampoline(lhs, rhs);
				}
			protected:
				~In<void>() {}
			};
		public:
			typedef In<ResultType> Base;
		};

		template
		<
			class BaseLhs, class BaseRhs, class SomeLhs, class SomeRhs,
			typename ResultType, class CastLhs, class CastRhs,
			ResultType (*Callback)(SomeLhs&, SomeRhs&)
		>
        struct FnDispatcherHelper : public FnDispatcherHelperBase
									<
										BaseLhs, BaseRhs, SomeLhs, SomeRhs,
										ResultType, CastLhs, CastRhs, Callback
									>::Base
        {};
    }
////////////////////////////////////////////////////////////////////////////////
// class template FnDispatcher
// Implements an automatic logarithmic double dispatcher for functions
// Features automated conversions
////////////////////////////////////////////////////////////////////////////////
    template <class BaseLhs, class BaseRhs = BaseLhs,
              typename ResultType = Loki::Private::VoidWrap::type,
              class CastingPolicy = DynamicCasterWrapper,
              class DispatcherBackend = BasicDispatcherWrapper>
	class FnDispatcher : public ::Loki::Private::FnDispatcherBase
							<
								BaseLhs, BaseRhs, 
								ResultType, DispatcherBackend
							>::Base
	
    {
        typedef typename  ::Loki::Private::FnDispatcherBase
		<
			BaseLhs, BaseRhs, ResultType, DispatcherBackend
		>::Base Base;    
    public:
        template <class SomeLhs, class SomeRhs>
        void Add(ResultType (*pFun)(BaseLhs&, BaseRhs&), 
				::Loki::Type2Type<SomeLhs>, 
				::Loki::Type2Type<SomeRhs>)
        {
			Base::backEnd_.Add(pFun, ::Loki::Type2Type<SomeLhs>(), 
				::Loki::Type2Type<SomeRhs>());
        }        
        

		// two different workarounds for FnDispatcher::Add
		// Using the first one writes:
		//	DisType dispatcher;
		//	dispatcher.Add(DisType::Etas<SomeLhs, SomeRhs, &AFunc>());
        // using the second workaround the call becomes:
		//	DisType dispatcher;
		//	DisTyp::AddI<SomeLhs, SomeRhs, &AFunc>()(dispatcher);

		// Helper-class for the first workaround.
		// When calling FnDispatcher::Add provide an object of this type
		// as argument.
		template <class SomeLhs, class SomeRhs, 
			ResultType (*callback)(SomeLhs&, SomeRhs&), bool symmetric = false>
		struct Etas
		{
			typedef Private::FnDispatcherHelper<
                    BaseLhs, BaseRhs, SomeLhs, SomeRhs, ResultType,
                    ApplyInnerType2<CastingPolicy, SomeLhs,BaseLhs>::type, 
                    ApplyInnerType2<CastingPolicy,SomeRhs,BaseRhs>::type, 
                    callback> Local;
			enum {sym = symmetric};
			typedef SomeLhs Lhs;
			typedef SomeRhs Rhs;
		};
		
		// EtasType has to be a template parameter. If one tries to use
		// a parameter of type Etas the MSVC 6.0 won't generate correct
		// code.
		template <class EtasType>
        void Add(EtasType EtasObj)
        {
			typedef typename EtasType::Local Local;	
			typedef typename EtasType::Lhs SomeLhs;
			typedef typename EtasType::Rhs SomeRhs;

            Add(&Local::Trampoline, ::Loki::Type2Type<SomeLhs>(),
				::Loki::Type2Type<SomeRhs>());
            if (EtasType::sym)
            {
                Add(&Local::TrampolineR, ::Loki::Type2Type<SomeRhs>(),
					::Loki::Type2Type<SomeLhs>());
            }
        }

		// alternative workaround for FnDispatcher::Add
		template <class SomeLhs, class SomeRhs, 
			ResultType (*callback)(SomeLhs&, SomeRhs&), bool symmetric = false>
		struct AddI
		{
			void operator()(FnDispatcher<BaseLhs, BaseRhs, ResultType, 
							CastingPolicy,DispatcherBackend>& f)
			{
				typedef Private::FnDispatcherHelper<
                    BaseLhs, BaseRhs, SomeLhs, SomeRhs, ResultType,
                    ApplyInnerType2<CastingPolicy, SomeLhs,BaseLhs>::type, 
                    ApplyInnerType2<CastingPolicy,SomeRhs,BaseRhs>::type, 
                    callback> Local;
				f.Add(&Local::Trampoline, ::Loki::Type2Type<SomeLhs>(), 
					::Loki::Type2Type<SomeRhs>());
				if (symmetric)
				{
					f.Add(&Local::TrampolineR, ::Loki::Type2Type<SomeRhs>(), 
					::Loki::Type2Type<SomeLhs>());
				}
			}
		};
		
        template <class SomeLhs, class SomeRhs>
        void Remove(::Loki::Type2Type<SomeLhs>, ::Loki::Type2Type<SomeRhs>)
        {
			Base::backEnd_.Remove(::Loki::Type2Type<SomeLhs>(), 
				::Loki::Type2Type<SomeRhs>());
        }

        
		// moved to base class
		// ResultType Go(BaseLhs& lhs, BaseRhs& rhs);
    };
	
////////////////////////////////////////////////////////////////////////////////
// class template FunctorDispatcherAdaptor
// permits use of FunctorDispatcher under gcc.2.95.2/3
///////////////////////////////////////////////////////////////////////////////

    namespace Private
    {
		template <class BaseLhs, class BaseRhs, class SomeLhs, class SomeRhs,
			typename ResultType, class CastLhs, class CastRhs, class Fun, bool SwapArgs>
        class FunctorDispatcherHelperBase
		{
			template <class R>
			class In
			{
				Fun fun_;
				ResultType Fire(BaseLhs& lhs, BaseRhs& rhs,Int2Type<false>)
				{
					return fun_(CastLhs::Cast(lhs), CastRhs::Cast(rhs));
				}
				ResultType Fire(BaseLhs& rhs, BaseRhs& lhs,Int2Type<true>)
				{
					return fun_(CastLhs::Cast(lhs), CastRhs::Cast(rhs));
				}
			public:
				In(const Fun& fun) : fun_(fun) {}

				ResultType operator()(BaseLhs& lhs, BaseRhs& rhs)
				{
					return Fire(lhs,rhs,Int2Type<SwapArgs>());
				}
			};
			template <>
			class In<void>
			{
				Fun fun_;
				ResultType Fire(BaseLhs& lhs, BaseRhs& rhs,Int2Type<false>)
				{
					fun_(CastLhs::Cast(lhs), CastRhs::Cast(rhs));
				}
				ResultType Fire(BaseLhs& rhs, BaseRhs& lhs,Int2Type<true>)
				{
					fun_(CastLhs::Cast(lhs), CastRhs::Cast(rhs));
				}
			public:
				In<void>(const Fun& fun) : fun_(fun) {}

				ResultType operator()(BaseLhs& lhs, BaseRhs& rhs)
				{
					Fire(lhs,rhs,Int2Type<SwapArgs>());
				}
			};
		public:
			typedef In<ResultType> Base;
		};	// FunctorDispatcherHelperBase
		
		template <class BaseLhs, class BaseRhs, class SomeLhs, class SomeRhs,
			typename ResultType, class CastLhs, class CastRhs, class Fun, bool SwapArgs>
		class FunctorDispatcherHelper : public FunctorDispatcherHelperBase
										< 
											BaseLhs, BaseRhs, SomeLhs, SomeRhs,
											ResultType, CastLhs, CastRhs, Fun,
											SwapArgs
										>::Base
		{
		private:	
			typedef typename FunctorDispatcherHelperBase
			< 
				BaseLhs, BaseRhs, SomeLhs, SomeRhs,
				ResultType, CastLhs, CastRhs, Fun, SwapArgs
			>::Base Base;
		public:
			FunctorDispatcherHelper(const Fun& f) : Base(f)
			{}
		};
		
		template <typename ResultType, class BaseLhs, class BaseRhs, 
			class DispatcherBackend>
		class FunctorDispatcherBase
		{
		private:
			class Common
			{
			protected:
				typedef TYPELIST_2(BaseLhs&, BaseRhs&) ArgsList;
				typedef Functor<ResultType, ArgsList, DEFAULT_THREADING> FunctorType;

				ApplyInnerType4<DispatcherBackend,BaseLhs, BaseRhs, ResultType, 
					FunctorType>::type backEnd_;
				~Common() {}
			};

			template <class R>
			class In : public Common
			{
			public:
				typedef typename Common::ArgsList ArgsList;
				typedef typename Common::FunctorType FunctorType;
				ResultType Go(BaseLhs& lhs, BaseRhs& rhs)
				{
					return Common::backEnd_.Go(lhs, rhs);
				}
			protected:
				In() {}
			};
			template <>
			class In<void> : public Common
			{
			public:
				typedef typename Common::ArgsList ArgsList;
				typedef typename Common::FunctorType FunctorType;
				ResultType Go(BaseLhs& lhs, BaseRhs& rhs)
				{
					Common::backEnd_.Go(lhs, rhs);
				}
			protected:
				In<void>() {}
			};
		public:
			typedef In<ResultType> Base;
		};
		
    }

////////////////////////////////////////////////////////////////////////////////
// class template FunctorDispatcher
// Implements a logarithmic double dispatcher for functors
// Features automated casting
////////////////////////////////////////////////////////////////////////////////

    template <class BaseLhs, class BaseRhs = BaseLhs,
              typename ResultType = Loki::Private::VoidWrap::type,
              class CastingPolicy = DynamicCasterWrapper, 
              class DispatcherBackend = BasicDispatcherWrapper>
	class FunctorDispatcher : public ::Loki::Private::FunctorDispatcherBase
								<
									ResultType, BaseLhs, BaseRhs, DispatcherBackend
								>::Base
    {
        typedef typename ::Loki::Private::FunctorDispatcherBase
		<
			ResultType, BaseLhs, BaseRhs, DispatcherBackend
		>::Base Base;
    public:
        typedef Base::ArgsList ArgsList;
        typedef Base::FunctorType FunctorType;
		template <class SomeLhs, class SomeRhs, class Fun>
		void Add(const Fun& fun, ::Loki::Type2Type<SomeLhs>, 
								::Loki::Type2Type<SomeRhs>)
        {
            typedef typename ApplyInnerType2<CastingPolicy,SomeLhs, BaseLhs>::type CastOne;
			typedef typename ApplyInnerType2<CastingPolicy,SomeRhs, BaseRhs>::type CastTwo;
			typedef typename 
				Private::FunctorDispatcherHelper
				<
					BaseLhs, BaseRhs, SomeLhs, SomeRhs,
					ResultType,CastOne,CastTwo,Fun, false
				>::Base Adapter;
			Base::backEnd_.Add(FunctorType(Adapter(fun), Loki::Disambiguate()), 
				Type2Type<SomeLhs>(), Type2Type<SomeRhs>());
		}
        template <class SomeLhs, class SomeRhs, class Fun>
		void Add(const Fun& fun, ::Loki::Type2Type<SomeLhs>, 
								::Loki::Type2Type<SomeRhs>, bool symmetric)
        {
			Add(fun, Type2Type<SomeLhs>(), Type2Type<SomeRhs>());

			if (symmetric)
			{
			// Note: symmetry only makes sense where BaseLhs==BaseRhs
				typedef typename ApplyInnerType2<CastingPolicy,SomeLhs, BaseLhs>::type CastOne;
				typedef typename ApplyInnerType2<CastingPolicy,SomeRhs, BaseLhs>::type CastTwo;
				typedef typename 
					Private::FunctorDispatcherHelper
				<
					BaseLhs, BaseRhs, SomeLhs, SomeRhs,
					ResultType,CastOne,CastTwo,Fun, true
				>::Base AdapterR;

				Base::backEnd_.Add(FunctorType(AdapterR(fun), Loki::Disambiguate()), 
						Type2Type<SomeRhs>(), Type2Type<SomeLhs>());
			}
        }
        
        template <class SomeLhs, class SomeRhs>
		void Remove(::Loki::Type2Type<SomeLhs>, ::Loki::Type2Type<SomeRhs>)
        {
			Base::backEnd_.Remove(Type2Type<SomeLhs>(), Type2Type<SomeRhs>());
        }

        // moved to base class
		// ResultType Go(BaseLhs& lhs, BaseRhs& rhs);
		
    };
} // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// May  10, 2002: ported by Rani Sharoni to VC7 (RTM - 9466)
// Oct  28, 2002: ported by Benjamin Kaufmann to MSVC 6
// Feb	19, 2003: replaced pointer-Dummies with Type2Type-Parameters and added 
//					support for return type void. B.K.
// Mar	06, 2003: Changed default values for return types to void.
//				  Added protected destructors to private implementation classes B.K.
// Mar	20. 2003: Fixed Bugs in FnDispatcherHelperBase, FnDispatcher::Add and
//					FunctorDispatcher::Add.
//					New Interface for FnDispatcher::Add.B.K.
// Mar	21, 2003: Added new explicit template argument specification workaround
//					for FnDispatcher::Add B.K.
////////////////////////////////////////////////////////////////////////////////

#endif
