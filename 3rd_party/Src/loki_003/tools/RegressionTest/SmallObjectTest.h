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

#ifndef SMALLOBJECTTEST_H
#define SMALLOBJECTTEST_H

#include <cstdlib>
#include <loki/SmallObj.h>
#include "UnitTest.h"

///////////////////////////////////////////////////////////////////////////////
// SmallObjectTest
///////////////////////////////////////////////////////////////////////////////

class SmallObjectTest : public Test
{
public:
  SmallObjectTest() : Test("SmallObject.h") {}

  virtual void execute(TestResult &result)
    {
    printName(result);

    using namespace Loki;

    bool r;

    SmallClass* a = new SmallClass;
    delete a;

    bool smallTest1=a!=NULL;

    a = new SmallClass2;
    delete a;

    bool smallTest2=a!=NULL;

    BigClass* b = new BigClass;
    delete b;

    bool bigTest1=b!=NULL;

    b = new BigClass2;
    delete b;

    bool bigTest2=b!=NULL;

    char* buff = static_cast<char*>(Loki::SmallObject<>::operator new(10));
    Loki::SmallObject<>::operator delete(buff, 10);

    bool test=buff!=NULL;

//    stress_test();

    r=smallTest1 && smallTest2 && bigTest1 && bigTest2 && test;

    testAssert("SmallObject",r,result);

    std::cout << '\n';
    }

private:
  class SmallClass : public Loki::SmallObject<>
  {
    int a;
  };

  class SmallClass2 : public SmallClass
  {
    int b;
  };

  class BigClass : public Loki::SmallObject<>
  {
    char a[200];
  };

  class BigClass2 : public BigClass
  {
    int b;
  };

  class Base
  {
  public:
    virtual ~Base() {}
  };

  class A : public Base, public Loki::SmallObject<>
  {
    int a[1];
  };

  class B : public Base, public Loki::SmallObject<>
  {
    int a[2];
  };

  class C : public Base, public Loki::SmallObject<>
  {
    int a[3];
  };

  class D : public Base, public Loki::SmallObject<>
  {
    int a[4];
  };

  static void stress_test()
  {
    std::vector<Base*> vec;

    vec.reserve(20 * 1024);

    std::srand(1231);

    for (int i = 0; i < 10; ++i)
    {
      for (int j = 0; j < 2 * 1024; ++j)
      {
        Base* p;

        switch (std::rand() % 4)
        {
          case 0: p = new A; break;
          case 1: p = new B; break;
          case 2: p = new C; break;
          case 3: p = new D; break;
        }

        vec.push_back(p);
      }

      for (int j = 0; j < 1024; ++j)
      {
        size_t pos = std::rand() % vec.size();

        delete vec[pos];

        vec[pos] = 0;
      }
    }

    while (!vec.empty())
    {
      delete vec.back();

      vec.pop_back();
    }
  }
} smallObjectTest;

#ifndef SMALLOBJ_CPP
# define SMALLOBJ_CPP
# include "../../SmallObj.cpp"
#endif
#endif
