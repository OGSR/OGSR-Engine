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

// Last update: Feb 20, 2003
// replaced pointer-dummy parameters with Type2Type-parameters.
// See readme.txt (notes: C) if you don't know why the dummy parameters are needed
//
// replaced all template template parameters with 'normal' parameters
// For each Factory-Unit there is now a wrapper-class (non template class) 
// containing a nested template class called In which
// provides a typedef (type) to the real unit-class.
// Use one of the wrapper-classes to instantiate a factory.
//
// Covariant return types had to go, too.

#ifndef ABSTRACTFACTORY_INC_
#define ABSTRACTFACTORY_INC_

#include "Typelist.h"
#include "TypeManip.h"
#include "HierarchyGenerators.h"
#include "MSVC6Helpers.h"

#include <cassert>

#define  ETAS_HELPER(Type) (::Loki::Type2Type<Type>())

namespace Loki
{

////////////////////////////////////////////////////////////////////////////////
// class template AbstractFactoryUnit
// The building block of an Abstract Factory
////////////////////////////////////////////////////////////////////////////////

    template <class T>
    class AbstractFactoryUnit
    {
    public:
        virtual T* DoCreate(Type2Type<T>) = 0;
        virtual ~AbstractFactoryUnit() {}
    };

	// Wrapper for AbstractFactoryUnit.
	struct AbstractFactoryUnitWrapper
	{
		template <class T>
		struct In
		{
			typedef AbstractFactoryUnit<T> type;
		};
	};
////////////////////////////////////////////////////////////////////////////////
// class template AbstractFactory
// Defines an Abstract Factory interface starting from a typelist
////////////////////////////////////////////////////////////////////////////////
//	VC 6.0 changes:
//	Because the VC 6.0 does not support explicit template argument specification (14.8.1)
//	the member-function Create takes a dummy argument of type Type2Type<T> whose sole
//	responsibility is to help the compiler in deducing the type of T.
//	Using this port the call:
//	ConcProduct* p = aFactory.Create<ConcProduct>();
//	therefore becomes:
//	ConcProduct* p = aFactory.Create(Type2Type<ConcProduct>());

    template
    <
        class TList,
        class Unit = AbstractFactoryUnitWrapper
    >
    class AbstractFactory : public GenScatterHierarchy<TList, Unit>
    {
    public:
        typedef TList ProductList;
        
        template <class T> T* Create(Type2Type<T>)
        {
            ApplyInnerType<Unit, T>::type& unit = *this;
            return unit.DoCreate(Type2Type<T>());
        }
    };
    
////////////////////////////////////////////////////////////////////////////////
// class template OpNewFactoryUnit
// Creates an object by invoking the new operator
////////////////////////////////////////////////////////////////////////////////

    template <class ConcreteProduct, class Base>
    class OpNewFactoryUnit : public Base
    {
        typedef typename Base::ProductList BaseProductList;
    
    protected:
        typedef typename BaseProductList::Tail ProductList;
    
    public:
        typedef typename BaseProductList::Head AbstractProduct;
        // VC does not support covariant return types
		AbstractProduct* DoCreate(Type2Type<AbstractProduct>)
        {
            return new ConcreteProduct;
        }
    };

	// Wrapper for OpNewFactoryUnit
	struct OpNewFactoryUnitWrapper
	{
		template <class T, class Base>
		struct In
		{
			typedef OpNewFactoryUnit<T, Base> type;
		};
	};

////////////////////////////////////////////////////////////////////////////////
// class template PrototypeFactoryUnit
// Creates an object by cloning a prototype
// There is a difference between the implementation herein and the one described
//     in the book: GetPrototype and SetPrototype use the helper friend 
//     functions DoGetPrototype and DoSetPrototype. The friend functions avoid
//     name hiding issues. Plus, GetPrototype takes a reference to pointer
//     instead of returning the pointer by value.
////////////////////////////////////////////////////////////////////////////////

    template <class ConcreteProduct, class Base>
    class PrototypeFactoryUnit : public Base
    {
        typedef typename Base::ProductList BaseProductList;
    
    protected:
        typedef typename BaseProductList::Tail ProductList;

    public:
        typedef typename BaseProductList::Head AbstractProduct;

        PrototypeFactoryUnit(AbstractProduct* p = 0)
            : pPrototype_(p)
        {}
        
        friend void DoGetPrototype(const PrototypeFactoryUnit& me,
            AbstractProduct*& pPrototype)
        { pPrototype = me.pPrototype_; }
        
        friend void DoSetPrototype(PrototypeFactoryUnit& me, 
            AbstractProduct* pObj)
        { me.pPrototype_ = pObj; }
        
        template <class U>
        void GetPrototype(AbstractProduct*& p)
        { return DoGetPrototype(*this, p); }
        
        template <class U>
        void SetPrototype(U* pObj)
        { DoSetPrototype(*this, pObj); }
        
        AbstractProduct* DoCreate(Type2Type<AbstractProduct>)
        {
            assert(pPrototype_);
            // vc does not support covariant return types thus
			// Clone *always* returns a base-pointer. 
			// if DoCreate is called from a ConcreteFactory-object we
			// need to down-cast.
			// Is the static_cast always safe?
			return static_cast<AbstractProduct*>(pPrototype_->Clone());
        }
        
    private:
        AbstractProduct* pPrototype_;
    };

	// Wrapper for PrototypeFactoryUnit
	struct PrototypeFactoryUnitWrapper
	{
		template <class T, class Base>
		struct In
		{
			typedef PrototypeFactoryUnit<T, Base> type;
		};
	};
////////////////////////////////////////////////////////////////////////////////
// class template ConcreteFactory
// Implements an AbstractFactory interface
////////////////////////////////////////////////////////////////////////////////

    template
    <
        class AbstractFact,
        class Creator = OpNewFactoryUnit,
        class TList = /*VC 6.0 does not like typename here*/ AbstractFact::ProductList
    >
    class ConcreteFactory
        : public GenLinearHierarchy<
            typename TL::Reverse<TList>::Result, Creator, AbstractFact>
    {
    public:
        typedef typename AbstractFact::ProductList ProductList;
        typedef TList ConcreteProductList;
    };

} // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// Oct  24, 2002: ported by Benjamin Kaufmann to MSVC 6.0
// Feb	20, 2003: replaced pointer-dummy parameters with Type2Type-parameters. B.K.
////////////////////////////////////////////////////////////////////////////////

#endif // ABSTRACTFACTORY_INC_
