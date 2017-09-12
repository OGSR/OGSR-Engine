 // boost::compressed_pair test program   
    
 //  (C) Copyright John Maddock 2000. Permission to copy, use, modify, sell and   
 //  distribute this software is granted provided this copyright notice appears   
 //  in all copies. This software is provided "as is" without express or implied   
 //  warranty, and with no claim as to its suitability for any purpose.   

// standalone test program for <boost/call_traits.hpp>
// 18 Mar 2002:
//    Changed some names to prevent conflicts with some new type_traits additions.
// 03 Oct 2000:
//    Enabled extra tests for VC6.

#include <cassert>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <typeinfo>
#include <boost/call_traits.hpp>

#include <boost/type_traits/type_traits_test.hpp>

// a way prevent warnings for unused variables
template<class T> inline void unused_variable(const T&) {}

//
// struct contained models a type that contains a type (for example std::pair)
// arrays are contained by value, and have to be treated as a special case:
//
template <class T>
struct contained
{
   // define our typedefs first, arrays are stored by value
   // so value_type is not the same as result_type:
   typedef typename boost::call_traits<T>::param_type       param_type;
   typedef typename boost::call_traits<T>::reference        reference;
   typedef typename boost::call_traits<T>::const_reference  const_reference;
   typedef T                                                value_type;
   typedef typename boost::call_traits<T>::value_type       result_type;

   // stored value:
   value_type v_;
   
   // constructors:
   contained() {}
   contained(param_type p) : v_(p){}
   // return byval:
   result_type value()const { return v_; }
   // return by_ref:
   reference get() { return v_; }
   const_reference const_get()const { return v_; }
   // pass value:
   void call(param_type){}

};

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
template <class T, std::size_t N>
struct contained<T[N]>
{
   typedef typename boost::call_traits<T[N]>::param_type       param_type;
   typedef typename boost::call_traits<T[N]>::reference        reference;
   typedef typename boost::call_traits<T[N]>::const_reference  const_reference;
   typedef T                                                   value_type[N];
   typedef typename boost::call_traits<T[N]>::value_type       result_type;

   value_type v_;

   contained(param_type p)
   {
      std::copy(p, p+N, v_);
   }
   // return byval:
   result_type value()const { return v_; }
   // return by_ref:
   reference get() { return v_; }
   const_reference const_get()const { return v_; }
   void call(param_type){}
};
#endif

template <class T>
contained<typename boost::call_traits<T>::value_type> test_wrap_type(const T& t)
{
   typedef typename boost::call_traits<T>::value_type ct;
   return contained<ct>(t);
}

namespace test{

template <class T1, class T2>
std::pair<
   typename boost::call_traits<T1>::value_type,
   typename boost::call_traits<T2>::value_type>
      make_pair(const T1& t1, const T2& t2)
{
   return std::pair<
      typename boost::call_traits<T1>::value_type,
      typename boost::call_traits<T2>::value_type>(t1, t2);
}

} // namespace test

using namespace std;

//
// struct call_traits_checker:
// verifies behaviour of contained example:
//
template <class T>
struct call_traits_checker
{
   typedef typename boost::call_traits<T>::param_type param_type;
   void operator()(param_type);
};

template <class T>
void call_traits_checker<T>::operator()(param_type p)
{
   T t(p);
   contained<T> c(t);
   cout << "checking contained<" << typeid(T).name() << ">..." << endl;
   assert(t == c.value());
   assert(t == c.get());
   assert(t == c.const_get());
#ifndef __ICL
   //cout << "typeof contained<" << typeid(T).name() << ">::v_ is:           " << typeid(&contained<T>::v_).name() << endl;
   cout << "typeof contained<" << typeid(T).name() << ">::value() is:      " << typeid(&contained<T>::value).name() << endl;
   cout << "typeof contained<" << typeid(T).name() << ">::get() is:        " << typeid(&contained<T>::get).name() << endl;
   cout << "typeof contained<" << typeid(T).name() << ">::const_get() is:  " << typeid(&contained<T>::const_get).name() << endl;
   cout << "typeof contained<" << typeid(T).name() << ">::call() is:       " << typeid(&contained<T>::call).name() << endl;
   cout << endl;
#endif
}

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
template <class T, std::size_t N>
struct call_traits_checker<T[N]>
{
   typedef typename boost::call_traits<T[N]>::param_type param_type;
   void operator()(param_type t)
   {
      contained<T[N]> c(t);
      cout << "checking contained<" << typeid(T[N]).name() << ">..." << endl;
      unsigned int i = 0;
      for(i = 0; i < N; ++i)
         assert(t[i] == c.value()[i]);
      for(i = 0; i < N; ++i)
         assert(t[i] == c.get()[i]);
      for(i = 0; i < N; ++i)
         assert(t[i] == c.const_get()[i]);

      cout << "typeof contained<" << typeid(T[N]).name() << ">::v_ is:         " << typeid(&contained<T[N]>::v_).name() << endl;
      cout << "typeof contained<" << typeid(T[N]).name() << ">::value is:      " << typeid(&contained<T[N]>::value).name() << endl;
      cout << "typeof contained<" << typeid(T[N]).name() << ">::get is:        " << typeid(&contained<T[N]>::get).name() << endl;
      cout << "typeof contained<" << typeid(T[N]).name() << ">::const_get is:  " << typeid(&contained<T[N]>::const_get).name() << endl;
      cout << "typeof contained<" << typeid(T[N]).name() << ">::call is:       " << typeid(&contained<T[N]>::call).name() << endl;
      cout << endl;
   }
};
#endif

//
// check_wrap:
template <class W, class U>
void check_wrap(const W& w, const U& u)
{
   cout << "checking " << typeid(W).name() << "..." << endl;
   assert(w.value() == u);
}

//
// check_make_pair:
// verifies behaviour of "make_pair":
//
template <class T, class U, class V>
void check_make_pair(T c, U u, V v)
{
   cout << "checking std::pair<" << typeid(c.first).name() << ", " << typeid(c.second).name() << ">..." << endl;
   assert(c.first == u);
   assert(c.second == v);
   cout << endl;
}


struct comparible_UDT
{
   int i_;
   comparible_UDT() : i_(2){}
   comparible_UDT(const comparible_UDT& other) : i_(other.i_){}
   comparible_UDT& operator=(const comparible_UDT& other)
   { 
      i_ = other.i_;
      return *this;
   }
   bool operator == (const comparible_UDT& v){ return v.i_ == i_; }
};

int main(int argc, char *argv[ ])
{
   call_traits_checker<comparible_UDT> c1;
   comparible_UDT u;
   c1(u);
   call_traits_checker<int> c2;
   int i = 2;
   c2(i);
   int* pi = &i;
   int a[2] = {1,2};
#if defined(BOOST_MSVC6_MEMBER_TEMPLATES) && !defined(__ICL)
   call_traits_checker<int*> c3;
   c3(pi);
   call_traits_checker<int&> c4;
   c4(i);
   call_traits_checker<const int&> c5;
   c5(i);
#if !defined (BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && !defined(__MWERKS__) && !defined(__SUNPRO_CC)
   call_traits_checker<int[2]> c6;
   c6(a);
#endif
#endif

   check_wrap(test_wrap_type(2), 2);
#if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && !defined(__SUNPRO_CC)
   check_wrap(test_wrap_type(a), a);
   check_make_pair(test::make_pair(a, a), a, a);
#endif

   // cv-qualifiers applied to reference types should have no effect
   // declare these here for later use with is_reference and remove_reference:
   typedef int& r_type;
   typedef const r_type cr_type;

   type_test(comparible_UDT, boost::call_traits<comparible_UDT>::value_type)
   type_test(comparible_UDT&, boost::call_traits<comparible_UDT>::reference)
   type_test(const comparible_UDT&, boost::call_traits<comparible_UDT>::const_reference)
   type_test(const comparible_UDT&, boost::call_traits<comparible_UDT>::param_type)
   type_test(int, boost::call_traits<int>::value_type)
   type_test(int&, boost::call_traits<int>::reference)
   type_test(const int&, boost::call_traits<int>::const_reference)
   type_test(const int, boost::call_traits<int>::param_type)
   type_test(int*, boost::call_traits<int*>::value_type)
   type_test(int*&, boost::call_traits<int*>::reference)
   type_test(int*const&, boost::call_traits<int*>::const_reference)
   type_test(int*const, boost::call_traits<int*>::param_type)
#if defined(BOOST_MSVC6_MEMBER_TEMPLATES)
   type_test(int&, boost::call_traits<int&>::value_type)
   type_test(int&, boost::call_traits<int&>::reference)
   type_test(const int&, boost::call_traits<int&>::const_reference)
   type_test(int&, boost::call_traits<int&>::param_type)
#if !(defined(__GNUC__) && ((__GNUC__ < 3) || (__GNUC__ == 3) && (__GNUC_MINOR__ < 1)))
   type_test(int&, boost::call_traits<cr_type>::value_type)
   type_test(int&, boost::call_traits<cr_type>::reference)
   type_test(const int&, boost::call_traits<cr_type>::const_reference)
   type_test(int&, boost::call_traits<cr_type>::param_type)
#else
   std::cout << "Your compiler cannot instantiate call_traits<int&const>, skipping four tests (4 errors)" << std::endl;
   failures += 4;
   test_count += 4;
#endif
   type_test(const int&, boost::call_traits<const int&>::value_type)
   type_test(const int&, boost::call_traits<const int&>::reference)
   type_test(const int&, boost::call_traits<const int&>::const_reference)
   type_test(const int&, boost::call_traits<const int&>::param_type)
#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
   type_test(const int*, boost::call_traits<int[3]>::value_type)
   type_test(int(&)[3], boost::call_traits<int[3]>::reference)
   type_test(const int(&)[3], boost::call_traits<int[3]>::const_reference)
   type_test(const int*const, boost::call_traits<int[3]>::param_type)
   type_test(const int*, boost::call_traits<const int[3]>::value_type)
   type_test(const int(&)[3], boost::call_traits<const int[3]>::reference)
   type_test(const int(&)[3], boost::call_traits<const int[3]>::const_reference)
   type_test(const int*const, boost::call_traits<const int[3]>::param_type)
   // test with abstract base class:
   type_test(test_abc1, boost::call_traits<test_abc1>::value_type)
   type_test(test_abc1&, boost::call_traits<test_abc1>::reference)
   type_test(const test_abc1&, boost::call_traits<test_abc1>::const_reference)
   type_test(const test_abc1&, boost::call_traits<test_abc1>::param_type)
#else
   std::cout << "You're compiler does not support partial template specialiation, skipping 8 tests (8 errors)" << std::endl;
   failures += 12;
   test_count += 12;
#endif
#else
   std::cout << "You're compiler does not support partial template specialiation, skipping 20 tests (20 errors)" << std::endl;
   failures += 24;
   test_count += 24;
#endif
   // test with an incomplete type:
   type_test(incomplete_type, boost::call_traits<incomplete_type>::value_type)
   type_test(incomplete_type&, boost::call_traits<incomplete_type>::reference)
   type_test(const incomplete_type&, boost::call_traits<incomplete_type>::const_reference)
   type_test(const incomplete_type&, boost::call_traits<incomplete_type>::param_type)

   return check_result(argc, argv);
}

//
// define call_traits tests to check that the assertions in the docs do actually work
// this is an instantiate only set of tests:
//
template <typename T, bool isarray = false>
struct call_traits_test
{
   typedef ::boost::call_traits<T> ct;
   typedef typename ct::param_type param_type;
   typedef typename ct::reference reference;
   typedef typename ct::const_reference const_reference;
   typedef typename ct::value_type value_type;
   static void assert_construct(param_type val);
};

template <typename T, bool isarray>
void call_traits_test<T, isarray>::assert_construct(typename call_traits_test<T, isarray>::param_type val)
{
   //
   // this is to check that the call_traits assertions are valid:
   T t(val);
   value_type v(t);
   reference r(t);
   const_reference cr(t);
   param_type p(t);
   value_type v2(v);
   value_type v3(r);
   value_type v4(p);
   reference r2(v);
   reference r3(r);
   const_reference cr2(v);
   const_reference cr3(r);
   const_reference cr4(cr);
   const_reference cr5(p);
   param_type p2(v);
   param_type p3(r);
   param_type p4(p);
   
   unused_variable(v2);
   unused_variable(v3);
   unused_variable(v4);
   unused_variable(r2);
   unused_variable(r3);
   unused_variable(cr2);
   unused_variable(cr3);
   unused_variable(cr4);
   unused_variable(cr5);
   unused_variable(p2);
   unused_variable(p3);
   unused_variable(p4);
}
#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
template <typename T>
struct call_traits_test<T, true>
{
   typedef ::boost::call_traits<T> ct;
   typedef typename ct::param_type param_type;
   typedef typename ct::reference reference;
   typedef typename ct::const_reference const_reference;
   typedef typename ct::value_type value_type;
   static void assert_construct(param_type val);
};

template <typename T>
void call_traits_test<T, true>::assert_construct(typename boost::call_traits<T>::param_type val)
{
   //
   // this is to check that the call_traits assertions are valid:
   T t;
   value_type v(t);
   value_type v5(val);
   reference r = t;
   const_reference cr = t;
   reference r2 = r;
   #ifndef __BORLANDC__
   // C++ Builder buglet:
   const_reference cr2 = r;
   #endif
   param_type p(t);
   value_type v2(v);
   const_reference cr3 = cr;
   value_type v3(r);
   value_type v4(p);
   param_type p2(v);
   param_type p3(r);
   param_type p4(p);
   
   unused_variable(v2);
   unused_variable(v3);
   unused_variable(v4);
   unused_variable(v5);
#ifndef __BORLANDC__
   unused_variable(r2);
   unused_variable(cr2);
#endif
   unused_variable(cr3);
   unused_variable(p2);
   unused_variable(p3);
   unused_variable(p4);
}
#endif //BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
//
// now check call_traits assertions by instantiating call_traits_test:
template struct call_traits_test<int>;
template struct call_traits_test<const int>;
template struct call_traits_test<int*>;
#if defined(BOOST_MSVC6_MEMBER_TEMPLATES)
template struct call_traits_test<int&>;
template struct call_traits_test<const int&>;
#if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && !defined(__SUNPRO_CC)
template struct call_traits_test<int[2], true>;
#endif
#endif

#if defined(BOOST_MSVC) && _MSC_VER <= 1300
unsigned int expected_failures = 14;
#elif defined(__SUNPRO_CC)
#if(__SUNPRO_CC <= 0x520)
unsigned int expected_failures = 18;
#elif(__SUNPRO_CC < 0x530)
unsigned int expected_failures = 17;
#else
unsigned int expected_failures = 6;
#endif
#elif defined(__BORLANDC__)
unsigned int expected_failures = 2;
#elif (defined(__GNUC__) && ((__GNUC__ < 3) || (__GNUC__ == 3) && (__GNUC_MINOR__ < 1)))
unsigned int expected_failures = 4;
#elif defined(__HP_aCC)
unsigned int expected_failures = 24;
#else
unsigned int expected_failures = 0;
#endif

