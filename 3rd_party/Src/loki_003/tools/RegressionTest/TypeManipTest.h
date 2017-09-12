///////////////////////////////////////////////////////////////////////////////
// Unit Test for Loki
//
// Copyright Terje Slettebø and Pavel Vozenilek 2002.
//
// Permission to use, copy, modify, and distribute this software for any
// purpose is hereby granted without fee, provided that this copyright and
// permissions notice appear in all copies and derivatives.
//
// This software is provided "as is" without express or implied warranty.
//
// Last update: September 16, 2002
///////////////////////////////////////////////////////////////////////////////

#ifndef TYPEMANIPTEST_H
#define TYPEMANIPTEST_H

#include <loki/TypeManip.h>
#include "UnitTest.h"

///////////////////////////////////////////////////////////////////////////////
// TypeManipTest
///////////////////////////////////////////////////////////////////////////////

class TypeManipTest : public Test
{
public:
  TypeManipTest() : Test("TypeManip.h") {}

  virtual void execute(TestResult &result)
    {
    printName(result);

    using namespace Loki;

    bool r;

    r=Int2Type<1>::value==1;

    testAssert("Int2Type",r,result);

    r=SameType<Type2Type<char>::OriginalType,char>::value;

    testAssert("Type2Type",r,result);

    r=SameType<Select<true,char,int>::Result,char>::value &&
      SameType<Select<false,char,int>::Result,int>::value;

    testAssert("Select",r,result);

    r=Conversion<char,int>::exists &&
      Conversion<char,int>::exists2Way &&
      !Conversion<char,int>::sameType &&
      Conversion<char,char>::exists &&
      Conversion<char,char>::exists2Way &&
      Conversion<char,char>::sameType &&
      Conversion<void,void>::exists &&
      !Conversion<void,char>::exists &&
      Conversion<char,void>::exists &&
      Conversion<Derived2,Base>::exists &&
      Conversion<Derived1,Base>::exists &&
      Conversion<Base,Base>::exists &&
      !Conversion<Base,Derived2>::exists &&
      !Conversion<Base,Derived1>::exists &&
      Conversion<Derived2 *,Base *>::exists &&
      Conversion<Derived1 *,Base *>::exists &&
      !Conversion<Base *,Derived2 *>::exists &&
      !Conversion<Base *,Derived1 *>::exists &&
      Conversion<Base *,void *>::exists &&
      !Conversion<void *,Base *>::exists;

    testAssert("Conversion",r,result);

    r=SuperSubclass<Base,Derived1>::value &&
      SuperSubclass<Base,Derived2>::value &&
      SuperSubclass<Base,Base>::value &&
      !SuperSubclass<Derived1,Base>::value &&
      !SuperSubclass<Derived2,Base>::value &&
      !SuperSubclass<void,Base>::value;
    
    testAssert("SuperSubclass",r,result);

    r=SuperSubclassStrict<Base,Derived1>::value &&
      SuperSubclassStrict<Base,Derived2>::value &&
      !SuperSubclassStrict<Base,Base>::value &&
      !SuperSubclassStrict<Derived1,Base>::value &&
      !SuperSubclassStrict<Derived2,Base>::value &&
      !SuperSubclassStrict<void,Base>::value;

    testAssert("SuperSubclassStrict",r,result);

    r=SUPERSUBCLASS(Base,Derived1) &&
      SUPERSUBCLASS(Base,Derived2) &&
      SUPERSUBCLASS(Base,Base) &&
      !SUPERSUBCLASS(Derived1,Base) &&
      !SUPERSUBCLASS(Derived2,Base) &&
      !SUPERSUBCLASS(void,Base);

    testAssert("SUPERSUBCLASS",r,result);

    r=SUPERSUBCLASS_STRICT(Base,Derived1) &&
      SUPERSUBCLASS_STRICT(Base,Derived2) &&
      !SUPERSUBCLASS_STRICT(Base,Base) &&
      !SUPERSUBCLASS_STRICT(Derived1,Base) &&
      !SUPERSUBCLASS_STRICT(Derived2,Base) &&
      !SUPERSUBCLASS_STRICT(void,Base);

    testAssert("SUPERSUBCLASS_STRICT",r,result);

    std::cout << '\n';
    }

private:
  struct Base { char c; };
  struct Derived1 : Base { char c; };
  struct Derived2 : Derived1 { char c; };
} typeManipTest;

#endif
