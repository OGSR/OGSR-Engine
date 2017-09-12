// Copyright (c) 2000
// Cadenza New Zealand Ltd
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without
// fee, provided that the above copyright notice appears in all copies
// and that both the copyright notice and this permission notice
// appear in supporting documentation.  Cadenza New Zealand Ltd makes
// no representations about the suitability of this software for any
// purpose.  It is provided "as is" without express or implied
// warranty.

//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_NO_POINTER_TO_MEMBER_CONST
//  TITLE:         pointers to const member functions
//  DESCRIPTION:   The compiler does not correctly handle
//                 pointers to const member functions, preventing use
//                 of these in overloaded function templates.
//                 See boost/functional.hpp for example.

#include <functional>

namespace boost_no_pointer_to_member_const{

template <class S, class T>
class const_mem_fun_t : public std::unary_function<const T*, S>
{
public:
  explicit const_mem_fun_t(S (T::*p)() const)
      :
      ptr(p)
  {}
  S operator()(const T* p) const
  {
      return (p->*ptr)();
  }
private:
  S (T::*ptr)() const;        
};

template <class S, class T, class A>
class const_mem_fun1_t : public std::binary_function<const T*, A, S>
{
public:
  explicit const_mem_fun1_t(S (T::*p)(A) const)
      :
      ptr(p)
  {}
  S operator()(const T* p, const A& x) const
  {
      return (p->*ptr)(x);
  }
private:
  S (T::*ptr)(A) const;
};

template<class S, class T>
inline const_mem_fun_t<S,T> mem_fun(S (T::*f)() const)
{
  return const_mem_fun_t<S,T>(f);
}

template<class S, class T, class A>
inline const_mem_fun1_t<S,T,A> mem_fun(S (T::*f)(A) const)
{
  return const_mem_fun1_t<S,T,A>(f);
}

class tester
{
public:
   void foo1()const{}
   int foo2(int i)const{ return i*2; }
};


int test()
{
   boost_no_pointer_to_member_const::mem_fun(&tester::foo1);
   boost_no_pointer_to_member_const::mem_fun(&tester::foo2);
   return 0;
}

}



