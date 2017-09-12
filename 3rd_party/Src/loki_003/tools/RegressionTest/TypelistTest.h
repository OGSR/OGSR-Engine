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

#ifndef TYPELISTTEST_H
#define TYPELISTTEST_H

#include <loki/Typelist.h>

///////////////////////////////////////////////////////////////////////////////
// TypelistTest
///////////////////////////////////////////////////////////////////////////////

class TypelistTest : public Test
{
public:
  TypelistTest() : Test("Typelist.h") {}

  virtual void execute(TestResult &result)
    {
    printName(result);

    using namespace Loki;
    using namespace Loki::TL;

    typedef TYPELIST_1(char) CharList;
    typedef TYPELIST_3(char,int,double) CharIntDoubleList;
    typedef TYPELIST_4(char,int,double,char) CharIntDoubleCharList;
    typedef TYPELIST_3(Base,Derived1,Derived2) BaseDerived1Derived2List;
    typedef TYPELIST_3(Derived2,Derived1,Base) Derived2Derived1BaseList;
    typedef TYPELIST_4(Base,Derived1,Base,Derived2) BaseDerived1BaseDerived2List;
    typedef TYPELIST_4(Derived1,Base,Derived1,Derived2) Derived1BaseDerived1Derived2List;

    bool r;

    r=Length<NullType>::value==0 &&
      Length<CharList>::value==1 &&
      Length<CharIntDoubleList>::value==3;

    testAssert("Length",r,result);

    r=SameType<TypeAt<CharList,0>::Result,char>::value &&
      SameType<TypeAt<CharIntDoubleList,2>::Result,double>::value;

    testAssert("TypeAt",r,result);

    #if !(_MSC_VER && !__INTEL_COMPILER && !__MWERKS__ && _MSC_VER < 1300)

    // TypeAtNonStrict works like TypeAt on MSVC 6.0

    r=SameType<TypeAtNonStrict<NullType,0>::Result,NullType>::value &&
      SameType<TypeAtNonStrict<CharList,0>::Result,char>::value &&
      SameType<TypeAtNonStrict<CharIntDoubleList,2>::Result,double>::value &&
      SameType<TypeAtNonStrict<CharIntDoubleList,3>::Result,NullType>::value &&
      SameType<TypeAtNonStrict<CharList,1,long>::Result,long>::value;

    testAssert("TypeAtNonStrict",r,result);

    #else

    testAssert("TypeAtNonStrict",false,result,false);

    #endif

    r=IndexOf<NullType,char>::value==-1 &&
      IndexOf<CharList,char>::value==0 &&
      IndexOf<CharIntDoubleList,double>::value==2 &&
      IndexOf<CharIntDoubleList,long>::value==-1;

    testAssert("IndexOf",r,result);

    #if !(_MSC_VER && !__INTEL_COMPILER && !__MWERKS__ && _MSC_VER < 1300)

    // Append, Erase, EraseAll, NoDuplicates, Replace, ReplaceAll, Reverse,
    // MostDerived and DerivedToFront doesn't work on MSVC 6.0

    r=SameType<Append<NullType,NullType>::Result,NullType>::value &&
      SameType<Append<NullType,char>::Result,TYPELIST_1(char)>::value &&
      SameType<Append<NullType,CharList>::Result,CharList>::value &&
      SameType<Append<CharList,NullType>::Result,CharList>::value &&
      SameType<Append<CharList,int>::Result,TYPELIST_2(char,int)>::value &&
      SameType<Append<CharList,CharIntDoubleList>::Result,TYPELIST_4(char,char,int,double)>::value;

    testAssert("Append",r,result);

    r=SameType<Erase<NullType,char>::Result,NullType>::value &&
      SameType<Erase<CharList,char>::Result,NullType>::value &&
      SameType<Erase<CharList,long>::Result,CharList>::value &&
      SameType<Erase<CharIntDoubleList,int>::Result,TYPELIST_2(char,double)>::value &&
      SameType<Erase<CharIntDoubleList,double>::Result,TYPELIST_2(char,int)>::value;

    testAssert("Erase",r,result);

    r=SameType<EraseAll<NullType,char>::Result,NullType>::value &&
      SameType<EraseAll<CharList,char>::Result,NullType>::value &&
      SameType<EraseAll<CharList,long>::Result,CharList>::value &&
      SameType<EraseAll<CharIntDoubleList,int>::Result,TYPELIST_2(char,double)>::value &&
      SameType<EraseAll<CharIntDoubleList,double>::Result,TYPELIST_2(char,int)>::value &&
      SameType<EraseAll<CharIntDoubleCharList,char>::Result,TYPELIST_2(int,double)>::value &&
      SameType<EraseAll<CharIntDoubleCharList,int>::Result,TYPELIST_3(char,double,char)>::value &&
      SameType<EraseAll<CharIntDoubleCharList,double>::Result,TYPELIST_3(char,int,char)>::value;

    testAssert("EraseAll",r,result);

    r=SameType<NoDuplicates<NullType>::Result,NullType>::value &&
      SameType<NoDuplicates<CharList>::Result,CharList>::value &&
      SameType<NoDuplicates<CharIntDoubleList>::Result,CharIntDoubleList>::value &&
      SameType<NoDuplicates<CharIntDoubleCharList>::Result,CharIntDoubleList>::value;

    testAssert("NoDuplicates",r,result);

    r=SameType<Replace<NullType,char,long>::Result,NullType>::value &&
      SameType<Replace<CharList,char,long>::Result,TYPELIST_1(long)>::value &&
      SameType<Replace<CharList,int,long>::Result,CharList>::value &&
      SameType<Replace<CharIntDoubleList,char,long>::Result,TYPELIST_3(long,int,double)>::value &&
      SameType<Replace<CharIntDoubleList,long,char[16]>::Result,CharIntDoubleList>::value &&
      SameType<Replace<CharIntDoubleCharList,char,long>::Result,TYPELIST_4(long,int,double,char)>::value;

    testAssert("Replace",r,result);

    r=SameType<ReplaceAll<NullType,char,long>::Result,NullType>::value &&
      SameType<ReplaceAll<CharList,char,long>::Result,TYPELIST_1(long)>::value &&
      SameType<ReplaceAll<CharList,int,long>::Result,CharList>::value &&
      SameType<ReplaceAll<CharIntDoubleList,char,long>::Result,TYPELIST_3(long,int,double)>::value &&
      SameType<ReplaceAll<CharIntDoubleList,long,char[16]>::Result,CharIntDoubleList>::value &&
      SameType<ReplaceAll<CharIntDoubleCharList,char,long>::Result,TYPELIST_4(long,int,double,long)>::value;

    testAssert("ReplaceAll",r,result);

    r=SameType<Reverse<NullType>::Result,NullType>::value &&
      SameType<Reverse<CharList>::Result,CharList>::value &&
      SameType<Reverse<CharIntDoubleList>::Result,TYPELIST_3(double,int,char)>::value;

    testAssert("Reverse",r,result);

    r=SameType<MostDerived<NullType,Base>::Result,Base>::value &&
      SameType<MostDerived<BaseDerived1Derived2List,Base>::Result,Derived2>::value &&
      SameType<MostDerived<BaseDerived1Derived2List,Derived1>::Result,Derived2>::value &&
      SameType<MostDerived<BaseDerived1Derived2List,Derived2>::Result,Derived2>::value &&
      SameType<MostDerived<Derived2Derived1BaseList,Base>::Result,Derived2>::value &&
      SameType<MostDerived<Derived2Derived1BaseList,Derived1>::Result,Derived2>::value &&
      SameType<MostDerived<Derived2Derived1BaseList,Derived2>::Result,Derived2>::value;

    testAssert("MostDerived",r,result);

    r=SameType<DerivedToFront<NullType>::Result,NullType>::value &&
      SameType<DerivedToFront<CharList>::Result,CharList>::value &&
      SameType<DerivedToFront<CharIntDoubleList>::Result,CharIntDoubleList>::value &&
      SameType<DerivedToFront<CharIntDoubleCharList>::Result,CharIntDoubleCharList>::value &&
      SameType<DerivedToFront<BaseDerived1Derived2List>::Result,Derived2Derived1BaseList>::value &&
      SameType<DerivedToFront<Derived2Derived1BaseList>::Result,Derived2Derived1BaseList>::value &&
      SameType<DerivedToFront<BaseDerived1BaseDerived2List>::Result,TYPELIST_4(Derived2,Derived1,Base,Base)>::value &&
      SameType<DerivedToFront<Derived1BaseDerived1Derived2List>::Result,TYPELIST_4(Derived2,Derived1,Derived1,Base)>::value;

    testAssert("DerivedToFront",r,result);

    #else

    testAssert("Append",false,result,false);
    testAssert("Erase",false,result,false);
    testAssert("EraseAll",false,result,false);
    testAssert("NoDuplicates",false,result,false);
    testAssert("Replace",false,result,false);
    testAssert("Reverse",false,result,false);
    testAssert("MostDerived",false,result,false);
    testAssert("DerivedToFront",false,result,false);

    #endif

    std::cout << '\n';
    }

private:
  struct Base { char c; };
  struct Derived1 : Base { char c; };
  struct Derived2 : Derived1 { char c; };
} typelistTest;
#endif
