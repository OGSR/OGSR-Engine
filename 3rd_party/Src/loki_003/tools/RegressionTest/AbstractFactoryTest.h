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

#ifndef ABSTRACTFACTORYTEST_H
#define ABSTRACTFACTORYTEST_H

#include <memory>
#include <typeinfo>
#include <loki/AbstractFactory.h>

///////////////////////////////////////////////////////////////////////////////
// AbstractFactoryTest
///////////////////////////////////////////////////////////////////////////////

class Soldier { public: virtual ~Soldier() {} };
class Monster { public: virtual ~Monster() {} };
class SuperMonster { public: virtual ~SuperMonster() {} };

class SillySoldier : public Soldier {};
class SillyMonster : public Monster {};
class SillySuperMonster : public SuperMonster {};

class BadSoldier : public Soldier {};
class BadMonster : public Monster {};
class BadSuperMonster : public SuperMonster {};

typedef Loki::AbstractFactory<TYPELIST_3(Soldier, Monster, SuperMonster)> AbstractEnemyFactory; 

typedef Loki::ConcreteFactory<AbstractEnemyFactory, Loki::OpNewFactoryUnit,
  TYPELIST_3(SillySoldier, SillyMonster, SillySuperMonster)> EasyLevelEnemyFactory;

typedef Loki::ConcreteFactory<AbstractEnemyFactory, Loki::OpNewFactoryUnit,
  TYPELIST_3(BadSoldier, BadMonster, BadSuperMonster)> HardLevelEnemyFactory;

class AbstractFactoryTest : public Test
{
public:
  AbstractFactoryTest() : Test("AbstractFactory.h") {}

  virtual void execute(TestResult &result)
    {
    printName(result);

    using namespace Loki;

    bool r;

    std::auto_ptr<AbstractEnemyFactory> easyFactory(new EasyLevelEnemyFactory);
    std::auto_ptr<AbstractEnemyFactory> hardFactory(new HardLevelEnemyFactory);

    Soldier *s;

    s = easyFactory->Create<Soldier>();
    
    r= !!(typeid(*s)==typeid(SillySoldier)); //SGB !! eliminates bool-to-int performance warning

    delete s;

#ifndef __BORLANDC__

    s = hardFactory->Create<Soldier>(); //BCB bug!!! - always creates SillySoldier

    r=r && typeid(*s)==typeid(BadSoldier);

    delete s;

#endif

    testAssert("AbstractFactory",r,result);

    std::cout << '\n';
    }
} abstractFactoryTest;

#endif
