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

#ifndef ASSOCVECTORTEST_H
#define ASSOCVECTORTEST_H

#include <cstdio>
#include <cstdlib>
#include <loki/AssocVector.h>
#include "UnitTest.h"

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER) && !defined(__MWERKS__)
 #define C_Namespace
#else
 #define C_Namespace std
#endif

///////////////////////////////////////////////////////////////////////////////
// AssocVectorTest
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// STL compatible allocator
///////////////////////////////////////////////////////////////////////////////

template <class T> class TestAllocator : public std::allocator<T>
{
public:
  typedef T                      value_type;
  typedef value_type*            pointer;
  typedef const value_type*      const_pointer;
  typedef value_type&            reference;
  typedef const value_type&      const_reference;
  typedef C_Namespace::size_t    size_type;
  typedef C_Namespace::ptrdiff_t difference_type;

  template <class U>
  struct rebind { typedef TestAllocator<U> other; };

  TestAllocator() {}
  TestAllocator(const TestAllocator&) {}

#if !(defined(_MSC_VER) && !defined(__INTEL_COMPILER) && !defined(__MWERKS__))

  template <class U>
  TestAllocator(const TestAllocator<U>&) {}

#endif

  ~TestAllocator() {}

  pointer address(reference x) const { return &x; }
  const_pointer address(const_reference x) const {
    return x;
  }

  pointer allocate(size_type n, const_pointer = 0) {
    return static_cast<pointer>(::operator new(n * sizeof(T)));
  }

  void deallocate(pointer p, size_type size) {
    ::operator delete(p);
  }

  size_type max_size() const {
    return static_cast<size_type>(-1) / sizeof(T);
  }

  void construct(pointer p, const value_type& x) {
    new(p) value_type(x);
  }
  void destroy(pointer p) {
#ifndef _USE_OLD_RW_STL  // ?? failed to compile when RogueWave is enabled !?!
    p->~value_type();
#endif
  }

private:
  void operator=(const TestAllocator&);
};

///////////////////////////////////////////////////////////////////////////////
// AVTestClass
///////////////////////////////////////////////////////////////////////////////

class AVTestClass
{
public:
    AVTestClass(int value) : value_(value) {}
    AVTestClass(const AVTestClass& other) : value_(other.value_) {}
    AVTestClass& operator=(const AVTestClass& other) {
        value_ = other.value_;
        return *this;
    }

    int value_;
};

bool operator<(const AVTestClass& lhs, const AVTestClass& rhs)
{
    return lhs.value_ < rhs.value_;
}

///////////////////////////////////////////////////////////////////////////////
// str_less
///////////////////////////////////////////////////////////////////////////////

struct str_less : public std::binary_function<const char*, const char*, bool> {
    bool operator()(const char* x, const char* y) const {
        return strcmp(x, y) < 0;
    }
};

///////////////////////////////////////////////////////////////////////////////
// int_less
///////////////////////////////////////////////////////////////////////////////

unsigned int_test_count = 0; // to verify usage
struct int_less : public std::less<int>
{
    bool operator()(int x, int y) const {
        ++int_test_count;
        return x < y;
    }
};

///////////////////////////////////////////////////////////////////////////////
// is_sorted
///////////////////////////////////////////////////////////////////////////////

template<class Vect>
bool is_sorted(const Vect& v) {
    if (v.size() < 2) return true;
    typename Vect::const_iterator it = v.begin();
    typename Vect::key_type previous = it->first;
    ++it;
    while (it != v.end()) {
        typename Vect::key_type current = it->first;
        if (!v.key_comp()(previous, current)) return false;
        previous = current;
        ++it;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////

typedef Loki::AssocVector<int, int, std::less<int>, TestAllocator<std::pair<int, int> > > test_vect1_t;
typedef Loki::AssocVector<const char*, int, str_less, TestAllocator<std::pair<const char*, int> > > test_vect2_t;
typedef Loki::AssocVector<std::string, std::string> test_vect3_t;
typedef Loki::AssocVector<AVTestClass, AVTestClass, std::less<AVTestClass>, TestAllocator<std::pair<AVTestClass, AVTestClass> > > test_vect4_t;
typedef Loki::AssocVector<int, const char*> test_vect5_t;

///////////////////////////////////////////////////////////////////////////////

void check_insert1(test_vect1_t& v)
{
    C_Namespace::srand(10);
    for (unsigned i = 0; i < 100; ++i) {
        int x = C_Namespace::rand();
        v.insert(std::make_pair(x, x));
        assert(is_sorted(v));
    }
    assert(v.size() == 100);
}

///////////////////////////////////////////////////////////////////////////////
// check_swap
///////////////////////////////////////////////////////////////////////////////

template <class Vect>
void check_swap(Vect& v1, Vect& v2)
{
    unsigned size1 = v1.size();
    unsigned size2 = v2.size();
    v1.swap(v2);
    assert(v1.size() == size2);
    assert(v2.size() == size1);
}

///////////////////////////////////////////////////////////////////////////////
// test_vect1
///////////////////////////////////////////////////////////////////////////////

void test_vect1()
{
    test_vect1_t vec11;
    assert(vec11.size() == 0);

    check_insert1(vec11);
    size_t size1 = vec11.size();
    assert(size1);

    test_vect1_t vec12(vec11.begin(), vec11.end());
    assert(vec12.size() == vec11.size());
    assert(vec11.size() == size1);

    test_vect1_t vec13;
    vec13 = vec11;
    assert(vec13.size() == vec11.size());
    assert(vec11.size() == size1);

    // this doesn't compile on Borland C++ 6.0 !?!
    //test_vect1_t vec99(test_vect1_t::key_compare(), TestAllocator<test_vect1_t::value_type>());
    //assert(vec99.size() == 0); //- here ir cries

    test_vect1_t::key_compare comp = test_vect1_t::key_compare();
    test_vect1_t vec14(comp, TestAllocator<test_vect1_t::value_type>());
    assert(vec14.size() == 0);

    check_swap(vec11, vec14);
    assert(vec14.size() == size1);
    assert(vec11.size() == 0);

    // this compiles, unlike object on stack
    test_vect1_t* vec15 = new test_vect1_t(test_vect1_t::key_compare(), TestAllocator<test_vect1_t::value_type>());
    assert(vec15->size() == 0);
    check_insert1(*vec15);
    delete vec15;

    // different comparator test - doesn't work for Loki
    //comp = int_less();
    //test_vect1_t vec16(comp);
    //assert(vec16.size() == 0);
    //assert(int_test_count == 0);
    //check_insert1(vec16);
    //assert(int_test_count != 0);
}

///////////////////////////////////////////////////////////////////////////////
// test_vect2
///////////////////////////////////////////////////////////////////////////////

void test_vect2()
{
    test_vect2_t vec21;
    vec21.insert(std::make_pair("abc", 1));
    vec21.insert(std::make_pair("xyz", 3));
    vec21.insert(std::make_pair("def", 2));
    assert(vec21.size() == 3);
    assert(is_sorted(vec21));

    test_vect2_t::iterator it = vec21.find("xyz");
    assert(it != vec21.end());
    assert(it->second == 3);

    std::pair<test_vect2_t::iterator, bool> aux = vec21.insert(std::make_pair("xyz", 99));
    assert(!aux.second);
	assert(aux.first->first == "xyz");
	assert(aux.first->second == 3);

    it = vec21.find("xyz");
    assert(it != vec21.end());
    assert(it->second == 3);

    vec21.erase(it);
    assert(vec21.size() == 2);
    it = vec21.find("xyz");
    assert(it == vec21.end());
    vec21.erase("xyz");
    vec21.erase("abc");
    assert(vec21.size() == 1);

    vec21.clear();
    assert(vec21.size() == 0);
}

///////////////////////////////////////////////////////////////////////////////
// test_vect3
///////////////////////////////////////////////////////////////////////////////

void test_vect3()
{
    // To use string, you need:
    // 1) enable _STLP_USE_NEWALLOC in include\stl\_site_config.h
    // 2) either disable use of any dynamic RTL (menu Project | Options | Linker)
    // 3) or recompile STLPort DLL.
    // This all is related to bug in STLPort allocator.
    test_vect3_t vec31;
    C_Namespace::srand(111);
    // a stress test
    for (unsigned i = 0; i < 2 * 1000; ++i) {
        char buffer[17];
        C_Namespace::sprintf(buffer, "string%d", C_Namespace::rand());
        std::string s(buffer);
        vec31.insert(std::make_pair(s, s));
    }
}

///////////////////////////////////////////////////////////////////////////////
// test_vect4
///////////////////////////////////////////////////////////////////////////////

void test_vect4()
{
    test_vect4_t vec41;
    for (int i = 0; i < 100; ++i) {
        vec41.insert(std::make_pair(AVTestClass(i), AVTestClass(i)));
    }
    assert(vec41.size() == 100);

    vec41.insert(std::make_pair(AVTestClass(300), AVTestClass(300)));
    vec41.insert(std::make_pair(AVTestClass(200), AVTestClass(200)));
    assert(vec41.size() == 102);

    test_vect4_t::iterator it = vec41.find(AVTestClass(22));
    assert(it != vec41.end());
    assert(it->second.value_ == 22);

    test_vect4_t vec42;
    vec42.swap(vec41);
    assert(vec41.size() == 0);
    assert(vec42.size() == 102);

    vec42.erase(vec42.begin(), vec42.end());
    assert(vec42.size() == 0);
}

///////////////////////////////////////////////////////////////////////////////
// test_vect5
///////////////////////////////////////////////////////////////////////////////

void test_vect5()
{
    test_vect5_t vec51;
    vec51.insert(test_vect5_t::value_type(3, "XXX"));
    vec51.insert(std::make_pair(1, "X"));
    vec51.insert(std::make_pair(2, "XX"));

    test_vect5_t::const_iterator it = vec51.begin();
    int count=1;

    while (it != vec51.end()) {
        assert(std::string(it->second).length()==count);

        ++count;

        it++; // intentionally
    }
}

class AssocVectorTest : public Test
{
public:
  AssocVectorTest() : Test("AssocVector.h") {}

  virtual void execute(TestResult &result)
    {
    printName(result);

    test_vect1();
    test_vect2();
    test_vect3();
    test_vect4();
    test_vect5();

    testAssert("AssocVector",true,result),

    std::cout << '\n';
    }
} assocVectorTest;

#endif
