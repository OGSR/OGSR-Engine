///////////////////////////////////////////////////////////////////////////////
// Unit Test framework
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

#ifndef UNITTEST_H
#define UNITTEST_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>

///////////////////////////////////////////////////////////////////////////////
// SameType
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER) && !defined(__MWERKS__)

// Rani Sharoni's SameType
//
// Non-conforming workaround for MSVC

template<class T1,class T2>
struct SameType
{
private:
  template<class>
  struct In 
  { enum { value = false }; };

  template<>
  struct In<T1>
  { enum { value = true };  };

public:
  enum { value = In<T2>::value };
};

#else

template<class T1,class T2>
struct SameType
{
  static const bool value=false;
};

template<class T>
struct SameType<T,T>
{
  static const bool value=true;
};

#endif

///////////////////////////////////////////////////////////////////////////////
// TestResult
///////////////////////////////////////////////////////////////////////////////

class TestResult
{
public:
  TestResult() : pos(0),passed(0),failed(0),notSupported(0) {}

  int pos;
  int passed;
  int failed;
  int notSupported;
};


///////////////////////////////////////////////////////////////////////////////
// Test
///////////////////////////////////////////////////////////////////////////////

class Test
{
typedef std::vector<Test*> tests_type;
static tests_type tests;

public:
  explicit Test(const std::string &n) : name(n)
    {
    Test::tests.push_back(this);
    }

  virtual void execute(TestResult &) =0;
  
protected:
  virtual ~Test() {}

  void printName(const TestResult &result) const
    {
    if(name.length()!=0)
      std::cout << std::string(result.pos,' ') << name << '\n';
    }

  void testAssert(const std::string &s,bool assertion,TestResult &result,bool supported =true,
                  const std::string &failStr =emptyStr())
    {
    std::string str=std::string(result.pos+2,' ')+s;

    str+=std::string(offset-str.length(),' ');

    if(supported)
      {
      if(assertion)
        {
        std::cout << str << "Passed\n";

        ++result.passed;
        }
      else
        {
        std::cout << str << (failStr==emptyStr() ? std::string("Failed") : "Failed - "+failStr) << '\n';

        ++result.failed;
        }
      }
    else
      {
      std::cout << str << "Not Supported\n";

      ++result.notSupported;
      }
    }

  static std::string emptyStr()
    {
    return std::string();
    }

public:
  enum { offset=63 };

protected:
  const std::string name;
  
public:
  static int run(const std::string &title)
    {
    std::cout << title << std::string(Test::offset-title.length(),' ') << "Result\n";
    std::cout << std::string(76,'-') << '\n';
    
    TestResult testResult;
    
    tests_type::iterator it;
    tests_type::const_iterator itEnd = Test::tests.end();
    for(it=Test::tests.begin(); it!=itEnd; ++it)
      {
      Test* test = *it;
      test->execute(testResult);
      }
    
    std::cout << std::string(76,'-') << '\n';

    const int total=testResult.passed+testResult.failed;
    const int totalAll=total+testResult.notSupported;

    if(total!=0)
      std::cout << "Total - " << testResult.passed << '/' << total << (total==1 ? " test, " : " tests, ")
                << testResult.passed*100/total << "% Passed\n";

    if(testResult.notSupported!=0)
      std::cout << "Not Supported - " << testResult.notSupported << '/' << totalAll << ", "
                << testResult.notSupported*100/totalAll << "%\n";

    return testResult.failed;
    }

};


#endif
