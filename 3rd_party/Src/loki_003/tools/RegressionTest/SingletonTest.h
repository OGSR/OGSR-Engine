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

#ifndef SINGLETONTEST_H
#define SINGLETONTEST_H

#include <cassert>
#include <loki/Singleton.h>
#include "UnitTest.h"

#define MAKE_TEST(name)\
    if(singletonTest && name::Instance().a != 99)\
      singletonTest=false;\
    ++name::Instance().a;\
    if(singletonTest && name::Instance().a != 100)\
      singletonTest=false;

///////////////////////////////////////////////////////////////////////////////
// SingletonTest
///////////////////////////////////////////////////////////////////////////////

template<int N>
class MyClass
{
public:
  MyClass() : a(99), wasDestroyed(false) {}

  virtual ~MyClass()
  {
    assert(!wasDestroyed);

    wasDestroyed = true;
  }

public:
  int a;
  bool wasDestroyed;
};

inline unsigned GetLongevity(MyClass<3> *) { return 6; }
inline unsigned GetLongevity(MyClass<7> *) { return 5; }
inline unsigned GetLongevity(MyClass<11> *) { return 4; }
inline unsigned GetLongevity(MyClass<15> *) { return 1; }
inline unsigned GetLongevity(MyClass<19> *) { return 2; }
inline unsigned GetLongevity(MyClass<23> *) { return 3; }

namespace
{
  using namespace Loki;

  typedef SingletonHolder<MyClass<0> > t0;

  typedef SingletonHolder<MyClass<1>, CreateUsingNew, DefaultLifetime, SingleThreaded> t1;
  typedef SingletonHolder<MyClass<2>, CreateUsingNew, PhoenixSingleton, SingleThreaded> t2;
  typedef SingletonHolder<MyClass<3>, CreateUsingNew, SingletonWithLongevity, SingleThreaded> t3;
  typedef SingletonHolder<MyClass<4>, CreateUsingNew, NoDestroy, SingleThreaded> t4;

  typedef SingletonHolder<MyClass<9>, CreateUsingMalloc, DefaultLifetime, SingleThreaded> t9;
  typedef SingletonHolder<MyClass<10>, CreateUsingMalloc, PhoenixSingleton, SingleThreaded> t10;
  typedef SingletonHolder<MyClass<11>, CreateUsingMalloc, SingletonWithLongevity, SingleThreaded> t11;
  typedef SingletonHolder<MyClass<12>, CreateUsingMalloc, NoDestroy, SingleThreaded> t12;

  typedef SingletonHolder<MyClass<17>, CreateStatic, DefaultLifetime, SingleThreaded> t17;
  typedef SingletonHolder<MyClass<18>, CreateStatic, PhoenixSingleton, SingleThreaded> t18;
  typedef SingletonHolder<MyClass<19>, CreateStatic, SingletonWithLongevity, SingleThreaded> t19;
  typedef SingletonHolder<MyClass<20>, CreateStatic, NoDestroy, SingleThreaded> t20;

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER) && !defined(__MWERKS__)

  typedef SingletonHolder<MyClass<5>, CreateUsingNew, DefaultLifetime, ClassLevelLockable> t5;
  typedef SingletonHolder<MyClass<6>, CreateUsingNew, PhoenixSingleton, ClassLevelLockable> t6;
  typedef SingletonHolder<MyClass<7>, CreateUsingNew, SingletonWithLongevity, ClassLevelLockable> t7;
  typedef SingletonHolder<MyClass<8>, CreateUsingNew, NoDestroy, ClassLevelLockable> t8;

  typedef SingletonHolder<MyClass<13>, CreateUsingMalloc, DefaultLifetime, ClassLevelLockable> t13;
  typedef SingletonHolder<MyClass<14>, CreateUsingMalloc, PhoenixSingleton, ClassLevelLockable> t14;
  typedef SingletonHolder<MyClass<15>, CreateUsingMalloc, SingletonWithLongevity, ClassLevelLockable> t15;
  typedef SingletonHolder<MyClass<16>, CreateUsingMalloc, NoDestroy, ClassLevelLockable> t16;

  typedef SingletonHolder<MyClass<21>, CreateStatic, DefaultLifetime, ClassLevelLockable> t21;
  typedef SingletonHolder<MyClass<22>, CreateStatic, PhoenixSingleton, ClassLevelLockable> t22;
  typedef SingletonHolder<MyClass<23>, CreateStatic, SingletonWithLongevity, ClassLevelLockable> t23;
  typedef SingletonHolder<MyClass<24>, CreateStatic, NoDestroy, ClassLevelLockable> t24;

#endif
}

class SingletonTest : public Test
{
public:
  SingletonTest() : Test("Singleton.h") {}

  virtual void execute(TestResult &result)
    {
    printName(result);

    using namespace Loki;

    singletonTest=true;

    MAKE_TEST(t0)

    MAKE_TEST(t1)
    MAKE_TEST(t2)
    MAKE_TEST(t3)
    MAKE_TEST(t4)

    MAKE_TEST(t9)
    MAKE_TEST(t10)
    MAKE_TEST(t11)
    MAKE_TEST(t12)

    MAKE_TEST(t17)
    MAKE_TEST(t18)
    MAKE_TEST(t19)
    MAKE_TEST(t20)

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER) && !defined(__MWERKS__)

    MAKE_TEST(t5)
    MAKE_TEST(t6)
    MAKE_TEST(t7)
    MAKE_TEST(t8)

    MAKE_TEST(t13)
    MAKE_TEST(t14)
    MAKE_TEST(t15)
    MAKE_TEST(t16)

    MAKE_TEST(t21)
    MAKE_TEST(t22)
    MAKE_TEST(t23)
    MAKE_TEST(t24)

#endif

    testAssert("Singleton",singletonTest,result);

    std::cout << '\n';
    }

private:
  bool singletonTest;
} singletonTest;

#include "../../Singleton.cpp"

#endif
