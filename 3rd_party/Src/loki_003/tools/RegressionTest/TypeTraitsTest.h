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

#ifndef TYPETRAITSTEST_H
#define TYPETRAITSTEST_H

#include <loki/TypeTraits.h>

///////////////////////////////////////////////////////////////////////////////
// TypeTraitsTest
///////////////////////////////////////////////////////////////////////////////

class TypeTraitsTest : public Test
{
public:
  TypeTraitsTest() : Test("TypeTraits.h") {}

  virtual void execute(TestResult &result)
    {
    printName(result);

    using namespace Loki;

    bool r;

    r=TypeTraits<int *>::isPointer &&
      !TypeTraits<int>::isPointer &&
      SameType<TypeTraits<int *>::PointeeType,int>::value &&
      SameType<TypeTraits<int>::PointeeType,NullType>::value &&

      TypeTraits<int &>::isReference &&
      !TypeTraits<int>::isReference &&
      SameType<TypeTraits<int &>::ReferredType,int>::value &&
      SameType<TypeTraits<int>::ReferredType,int>::value &&

      TypeTraits<int Test::*>::isMemberPointer &&
      !TypeTraits<int>::isMemberPointer &&

      TypeTraits<unsigned int>::isStdUnsignedInt &&
      !TypeTraits<int>::isStdUnsignedInt &&

      TypeTraits<int>::isStdSignedInt &&
      !TypeTraits<unsigned int>::isStdSignedInt &&

      TypeTraits<int>::isStdIntegral &&
      !TypeTraits<double>::isStdIntegral &&

      TypeTraits<double>::isStdFloat &&
      !TypeTraits<int>::isStdFloat &&

      TypeTraits<int>::isStdArith &&
      !TypeTraits<void>::isStdArith &&

      TypeTraits<void>::isStdFundamental &&
      !TypeTraits<Test>::isStdFundamental &&

      TypeTraits<unsigned int>::isUnsignedInt &&
      !TypeTraits<int>::isUnsignedInt &&

      TypeTraits<int>::isSignedInt &&
      !TypeTraits<unsigned int>::isSignedInt &&

      TypeTraits<int>::isIntegral &&
      !TypeTraits<double>::isIntegral &&

      TypeTraits<double>::isFloat &&
      !TypeTraits<int>::isFloat &&

      TypeTraits<char>::isArith &&
      TypeTraits<int>::isArith &&
      TypeTraits<double>::isArith &&
      !TypeTraits<void>::isArith &&

      TypeTraits<void>::isFundamental &&
      !TypeTraits<Test>::isFundamental &&

#ifndef __BORLANDC__

      TypeTraits<const int>::isConst &&
      !TypeTraits<int>::isConst &&
      SameType<TypeTraits<const int>::NonConstType,int>::value &&
      SameType<TypeTraits<int>::NonConstType,int>::value &&

      TypeTraits<volatile int>::isVolatile &&
      !TypeTraits<int>::isVolatile &&
      SameType<TypeTraits<volatile int>::NonVolatileType,int>::value &&
      SameType<TypeTraits<int>::NonVolatileType,int>::value &&

      SameType<TypeTraits<const volatile int>::UnqualifiedType,int>::value &&

#endif

      SameType<TypeTraits<char>::ParameterType,char>::value &&
      SameType<TypeTraits<int>::ParameterType,int>::value &&
      SameType<TypeTraits<double>::ParameterType,double>::value &&
      SameType<TypeTraits<Test>::ParameterType,Test &>::value;

    testAssert("TypeTraits",r,result);

    std::cout << '\n';
    }
} typeTraitsTest;

#endif
