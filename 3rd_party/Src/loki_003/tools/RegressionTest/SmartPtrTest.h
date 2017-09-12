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

#ifndef SMARTPTRTEST_H
#define SMARTPTRTEST_H

#include <loki/SmartPtr.h>
#include "UnitTest.h"

///////////////////////////////////////////////////////////////////////////////
// SmartPtrTest
///////////////////////////////////////////////////////////////////////////////

class TestClass
{
public:
  TestClass() : references(1)
  {
    ++instances;
  }

  ~TestClass()
  {
    --instances;
  }

  void AddRef()
  {
    ++references;
  }

  void Release()
  {
  --references;

  if (references <= 0)
    delete this;
  }

  TestClass* Clone()
  {
    return new TestClass(*this);
  }

public:
  static int instances;

  int references;
};

int TestClass::instances = 0;

class SmartPtrTest : public Test
{
public:
  SmartPtrTest() : Test("SmartPtr.h") {}

  virtual void execute(TestResult &result)
    {
    printName(result);

    using namespace Loki;

    { SmartPtr<TestClass> p = new TestClass; }

    bool test1=TestClass::instances == 0;

    { p0 p(new TestClass); }
    { p1 p(new TestClass); }
//    { p2 p(new TestClass); }
    { p3 p(new TestClass); }
    { p4 p(new TestClass); }
    { p5 p(new TestClass); }
    { p6 p(new TestClass); }
    { p7 p(new TestClass); }
    { p8 p(new TestClass); }
    { p9 p(new TestClass); }
//    { p10 p(new TestClass); }
    { p11 p(new TestClass); }
    { p12 p(new TestClass); }
    { p13 p(new TestClass); }
    { p14 p(new TestClass); }
    { p15 p(new TestClass); }
    { p16 p(new TestClass); }
    { p17 p(new TestClass); }
//    { p18 p(new TestClass); }
    { p19 p(new TestClass); }
    { p20 p(new TestClass); }
    { p21 p(new TestClass); }
    { p22 p(new TestClass); }
    { p23 p(new TestClass); }
    { p24 p(new TestClass); }
    { p25 p(new TestClass); }
//    { p26 p(new TestClass); }
    { p27 p(new TestClass); }
    { p28 p(new TestClass); }
    { p29 p(new TestClass); }
    { p30 p(new TestClass); }
    { p31 p(new TestClass); }
    { p40 p(new TestClass); }
    { p41 p(new TestClass); }
//    { p42 p(new TestClass); }
    { p43 p(new TestClass); }
    { p44 p(new TestClass); }
    { p45 p(new TestClass); }
    { p46 p(new TestClass); }
    { p47 p(new TestClass); }
    { p48 p(new TestClass); }
    { p49 p(new TestClass); }
//    { p50 p(new TestClass); }
    { p51 p(new TestClass); }
    { p52 p(new TestClass); }
    { p53 p(new TestClass); }
    { p54 p(new TestClass); }
    { p55 p(new TestClass); }
    { p56 p(new TestClass); }
    { p57 p(new TestClass); }
//    { p58 p(new TestClass); }
    { p59 p(new TestClass); }
    { p60 p(new TestClass); }
    { p61 p(new TestClass); }
    { p62 p(new TestClass); }
    { p63 p(new TestClass); }
    { p64 p(new TestClass); }
    { p65 p(new TestClass); }
//    { p66 p(new TestClass); }
    { p67 p(new TestClass); }
    { p68 p(new TestClass); }
    { p69 p(new TestClass); }
    { p70 p(new TestClass); }
    { p71 p(new TestClass); }
    { p72 p(new TestClass); }
    { p73 p(new TestClass); }
//    { p74 p(new TestClass); }
    { p75 p(new TestClass); }
    { p76 p(new TestClass); }
    { p77 p(new TestClass); }
    { p78 p(new TestClass); }
    { p79 p(new TestClass); }
    { p80 p(new TestClass); }
    { p81 p(new TestClass); }
//    { p82 p(new TestClass); }
    { p83 p(new TestClass); }
    { p84 p(new TestClass); }
    { p85 p(new TestClass); }
    { p86 p(new TestClass); }
    { p87 p(new TestClass); }
    { p88 p(new TestClass); }
    { p89 p(new TestClass); }
//    { p90 p(new TestClass); }
    { p91 p(new TestClass); }
    { p92 p(new TestClass); }
    { p93 p(new TestClass); }
    { p94 p(new TestClass); }
    { p95 p(new TestClass); }
    { p96 p(new TestClass); }
    { p97 p(new TestClass); }
//    { p98 p(new TestClass); }
    { p99 p(new TestClass); }
    { p100 p(new TestClass); }
    { p101 p(new TestClass); }
    { p102 p(new TestClass); }
    { p103 p(new TestClass); }

    bool test2=TestClass::instances==0;

    bool r=test1 && test2;

    testAssert("SmartPtr",r,result);

    std::cout << '\n';
    }

private:
  typedef SmartPtr<TestClass, DeepCopy, DisallowConversion, AssertCheck, DefaultSPStorage> p0;
  typedef SmartPtr<TestClass, RefCounted, DisallowConversion, AssertCheck, DefaultSPStorage> p1;
  //typedef SmartPtr<TestClass, RefCountedMT, DisallowConversion, AssertCheck, DefaultSPStorage> p2;
  typedef SmartPtr<TestClass, COMRefCounted, DisallowConversion, AssertCheck, DefaultSPStorage> p3;
  typedef SmartPtr<TestClass, RefLinked, DisallowConversion, AssertCheck, DefaultSPStorage> p4;
  typedef SmartPtr<TestClass, DestructiveCopy, DisallowConversion, AssertCheck, DefaultSPStorage> p5;
  typedef SmartPtr<TestClass, NoCopy, DisallowConversion, AssertCheck, DefaultSPStorage> p6;
  typedef SmartPtr<TestClass, NoCopy, DisallowConversion, AssertCheck, DefaultSPStorage> p7;

  typedef SmartPtr<TestClass, DeepCopy, AllowConversion, AssertCheck, DefaultSPStorage> p8;
  typedef SmartPtr<TestClass, RefCounted, AllowConversion, AssertCheck, DefaultSPStorage> p9;
  //typedef SmartPtr<TestClass, RefCountedMT, AllowConversion, AssertCheck, DefaultSPStorage> p10;
  typedef SmartPtr<TestClass, COMRefCounted, AllowConversion, AssertCheck, DefaultSPStorage> p11;
  typedef SmartPtr<TestClass, RefLinked, AllowConversion, AssertCheck, DefaultSPStorage> p12;
  typedef SmartPtr<TestClass, DestructiveCopy, AllowConversion, AssertCheck, DefaultSPStorage> p13;
  typedef SmartPtr<TestClass, NoCopy, AllowConversion, AssertCheck, DefaultSPStorage> p14;
  typedef SmartPtr<TestClass, NoCopy, AllowConversion, AssertCheck, DefaultSPStorage> p15;

  typedef SmartPtr<TestClass, DeepCopy, DisallowConversion, AssertCheckStrict, DefaultSPStorage> p16;
  typedef SmartPtr<TestClass, RefCounted, DisallowConversion, AssertCheckStrict, DefaultSPStorage> p17;
  //typedef SmartPtr<TestClass, RefCountedMT, DisallowConversion, AssertCheckStrict, DefaultSPStorage> p18;
  typedef SmartPtr<TestClass, COMRefCounted, DisallowConversion, AssertCheckStrict, DefaultSPStorage> p19;
  typedef SmartPtr<TestClass, RefLinked, DisallowConversion, AssertCheckStrict, DefaultSPStorage> p20;
  typedef SmartPtr<TestClass, DestructiveCopy, DisallowConversion, AssertCheckStrict, DefaultSPStorage> p21;
  typedef SmartPtr<TestClass, NoCopy, DisallowConversion, AssertCheckStrict, DefaultSPStorage> p22;
  typedef SmartPtr<TestClass, NoCopy, DisallowConversion, AssertCheckStrict, DefaultSPStorage> p23;

  typedef SmartPtr<TestClass, DeepCopy, AllowConversion, AssertCheckStrict, DefaultSPStorage> p24;
  typedef SmartPtr<TestClass, RefCounted, AllowConversion, AssertCheckStrict, DefaultSPStorage> p25;
  //typedef SmartPtr<TestClass, RefCountedMT, AllowConversion, AssertCheckStrict, DefaultSPStorage> p26;
  typedef SmartPtr<TestClass, COMRefCounted, AllowConversion, AssertCheckStrict, DefaultSPStorage> p27;
  typedef SmartPtr<TestClass, RefLinked, AllowConversion, AssertCheckStrict, DefaultSPStorage> p28;
  typedef SmartPtr<TestClass, DestructiveCopy, AllowConversion, AssertCheckStrict, DefaultSPStorage> p29;
  typedef SmartPtr<TestClass, NoCopy, AllowConversion, AssertCheckStrict, DefaultSPStorage> p30;
  typedef SmartPtr<TestClass, NoCopy, AllowConversion, AssertCheckStrict, DefaultSPStorage> p31;

  typedef SmartPtr<TestClass, DeepCopy, AllowConversion, RejectNullStatic, DefaultSPStorage> p40;
  typedef SmartPtr<TestClass, RefCounted, AllowConversion, RejectNullStatic, DefaultSPStorage> p41;
  //typedef SmartPtr<TestClass, RefCountedMT, AllowConversion, RejectNullStatic, DefaultSPStorage> p42;
  typedef SmartPtr<TestClass, COMRefCounted, AllowConversion, RejectNullStatic, DefaultSPStorage> p43;
  typedef SmartPtr<TestClass, RefLinked, AllowConversion, RejectNullStatic, DefaultSPStorage> p44;
  typedef SmartPtr<TestClass, DestructiveCopy, AllowConversion, RejectNullStatic, DefaultSPStorage> p45;
  typedef SmartPtr<TestClass, NoCopy, AllowConversion, RejectNullStatic, DefaultSPStorage> p46;
  typedef SmartPtr<TestClass, NoCopy, AllowConversion, RejectNullStatic, DefaultSPStorage> p47;

  typedef SmartPtr<TestClass, DeepCopy, DisallowConversion, RejectNullStatic, DefaultSPStorage> p48;
  typedef SmartPtr<TestClass, RefCounted, DisallowConversion, RejectNullStatic, DefaultSPStorage> p49;
  //typedef SmartPtr<TestClass, RefCountedMT, DisallowConversion, RejectNullStatic, DefaultSPStorage> p50;
  typedef SmartPtr<TestClass, COMRefCounted, DisallowConversion, RejectNullStatic, DefaultSPStorage> p51;
  typedef SmartPtr<TestClass, RefLinked, DisallowConversion, RejectNullStatic, DefaultSPStorage> p52;
  typedef SmartPtr<TestClass, DestructiveCopy, DisallowConversion, RejectNullStatic, DefaultSPStorage> p53;
  typedef SmartPtr<TestClass, NoCopy, DisallowConversion, RejectNullStatic,DefaultSPStorage> p54;
  typedef SmartPtr<TestClass, NoCopy, DisallowConversion, RejectNullStatic,DefaultSPStorage> p55;

  typedef SmartPtr<TestClass, DeepCopy, AllowConversion, RejectNull, DefaultSPStorage> p56;
  typedef SmartPtr<TestClass, RefCounted, AllowConversion, RejectNull, DefaultSPStorage> p57;
  //typedef SmartPtr<TestClass, RefCountedMT, AllowConversion, RejectNull, DefaultSPStorage> p58;
  typedef SmartPtr<TestClass, COMRefCounted, AllowConversion, RejectNull, DefaultSPStorage> p59;
  typedef SmartPtr<TestClass, RefLinked, AllowConversion, RejectNull, DefaultSPStorage> p60;
  typedef SmartPtr<TestClass, DestructiveCopy, AllowConversion, RejectNull, DefaultSPStorage> p61;
  typedef SmartPtr<TestClass, NoCopy, AllowConversion, RejectNull, DefaultSPStorage> p62;
  typedef SmartPtr<TestClass, NoCopy, AllowConversion, RejectNull, DefaultSPStorage> p63;

  typedef SmartPtr<TestClass, DeepCopy, DisallowConversion, RejectNull,  DefaultSPStorage> p64;
  typedef SmartPtr<TestClass, RefCounted, DisallowConversion, RejectNull,  DefaultSPStorage> p65;
  //typedef SmartPtr<TestClass, RefCountedMT, DisallowConversion, RejectNull,  DefaultSPStorage> p66;
  typedef SmartPtr<TestClass, COMRefCounted, DisallowConversion, RejectNull,  DefaultSPStorage> p67;
  typedef SmartPtr<TestClass, RefLinked, DisallowConversion, RejectNull,  DefaultSPStorage> p68;
  typedef SmartPtr<TestClass, DestructiveCopy, DisallowConversion, RejectNull,  DefaultSPStorage> p69;
  typedef SmartPtr<TestClass, NoCopy, DisallowConversion, RejectNull,  DefaultSPStorage> p70;
  typedef SmartPtr<TestClass, NoCopy, DisallowConversion, RejectNull,  DefaultSPStorage> p71;

  typedef SmartPtr<TestClass, DeepCopy, AllowConversion, RejectNullStrict, DefaultSPStorage> p72;
  typedef SmartPtr<TestClass, RefCounted, AllowConversion, RejectNullStrict, DefaultSPStorage> p73;
  //typedef SmartPtr<TestClass, RefCountedMT, AllowConversion, RejectNullStrict, DefaultSPStorage> p74;
  typedef SmartPtr<TestClass, COMRefCounted, AllowConversion, RejectNullStrict, DefaultSPStorage> p75;
  typedef SmartPtr<TestClass, RefLinked, AllowConversion, RejectNullStrict, DefaultSPStorage> p76;
  typedef SmartPtr<TestClass, DestructiveCopy, AllowConversion, RejectNullStrict, DefaultSPStorage> p77;
  typedef SmartPtr<TestClass, NoCopy, AllowConversion, RejectNullStrict, DefaultSPStorage> p78;
  typedef SmartPtr<TestClass, NoCopy, AllowConversion, RejectNullStrict, DefaultSPStorage> p79;

  typedef SmartPtr<TestClass, DeepCopy, DisallowConversion, RejectNullStrict, DefaultSPStorage> p80;
  typedef SmartPtr<TestClass, RefCounted, DisallowConversion, RejectNullStrict, DefaultSPStorage> p81;
  //typedef SmartPtr<TestClass, RefCountedMT, DisallowConversion, RejectNullStrict, DefaultSPStorage> p82;
  typedef SmartPtr<TestClass, COMRefCounted, DisallowConversion, RejectNullStrict, DefaultSPStorage> p83;
  typedef SmartPtr<TestClass, RefLinked, DisallowConversion, RejectNullStrict, DefaultSPStorage> p84;
  typedef SmartPtr<TestClass, DestructiveCopy, DisallowConversion, RejectNullStrict, DefaultSPStorage> p85;
  typedef SmartPtr<TestClass, NoCopy, DisallowConversion, RejectNullStrict, DefaultSPStorage> p86;
  typedef SmartPtr<TestClass, NoCopy, DisallowConversion, RejectNullStrict, DefaultSPStorage> p87;

  typedef SmartPtr<TestClass, DeepCopy, AllowConversion, NoCheck, DefaultSPStorage> p88;
  typedef SmartPtr<TestClass, RefCounted, AllowConversion, NoCheck, DefaultSPStorage> p89;
  //typedef SmartPtr<TestClass, RefCountedMT, AllowConversion, NoCheck, DefaultSPStorage> p90;
  typedef SmartPtr<TestClass, COMRefCounted, AllowConversion, NoCheck, DefaultSPStorage> p91;
  typedef SmartPtr<TestClass, RefLinked, AllowConversion, NoCheck, DefaultSPStorage> p92;
  typedef SmartPtr<TestClass, DestructiveCopy, AllowConversion, NoCheck, DefaultSPStorage> p93;
  typedef SmartPtr<TestClass, NoCopy, AllowConversion, NoCheck, DefaultSPStorage> p94;
  typedef SmartPtr<TestClass, NoCopy, AllowConversion, NoCheck, DefaultSPStorage> p95;

  typedef SmartPtr<TestClass, DeepCopy, DisallowConversion, NoCheck, DefaultSPStorage> p96;
  typedef SmartPtr<TestClass, RefCounted, DisallowConversion, NoCheck, DefaultSPStorage> p97;
  //typedef SmartPtr<TestClass, RefCountedMT, DisallowConversion, NoCheck, DefaultSPStorage> p98;
  typedef SmartPtr<TestClass, COMRefCounted, DisallowConversion, NoCheck, DefaultSPStorage> p99;
  typedef SmartPtr<TestClass, RefLinked, DisallowConversion, NoCheck, DefaultSPStorage> p100;
  typedef SmartPtr<TestClass, DestructiveCopy, DisallowConversion, NoCheck, DefaultSPStorage> p101;
  typedef SmartPtr<TestClass, NoCopy, DisallowConversion, NoCheck, DefaultSPStorage> p102;
  typedef SmartPtr<TestClass, NoCopy, DisallowConversion, NoCheck, DefaultSPStorage> p103;
} smartPtrTest;

#endif
